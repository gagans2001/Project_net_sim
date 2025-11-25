#include <vector>
#include <memory>
#include <stdexcept>
#include <thread>
#include <future>
#include <algorithm>

#include "../include/basicIO.h"
#include "../include/CommunicationStandard.h"
#include "../include/2G.h"
#include "../include/3G.h"
#include "../include/4G.h"
#include "../include/5G.h"
#include "../include/UserDevice.h"
#include "../include/CellTower.h"
#include "../include/CellularCore.h"

// Helper template
template<typename T>
T clamp_val(T v, T lo, T hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

// Worker function (no I/O inside)
struct ThreadResult {
    int users;
    int messages;
    std::thread::id tid;
};

static ThreadResult compute_messages_range(CommunicationStandard* standard, int count) {
    ThreadResult r;
    r.users = count;
    r.messages = (standard ? count * standard->messagesPerUser() : 0);
    r.tid = std::this_thread::get_id();
    // small simulated work (no I/O)
    return r;
}

int main() {
    io.outputstring("Cellular Network Simulator\n");
    io.outputstring("Select standard:\n");
    io.outputstring("1 = 2G\n");
    io.outputstring("2 = 3G\n");
    io.outputstring("3 = 4G\n");
    io.outputstring("4 = 5G\n");

    int choice = io.inputint();

    std::unique_ptr<CommunicationStandard> standard;

    if (choice == 1)      standard.reset(new TwoG());
    else if (choice == 2) standard.reset(new ThreeG());
    else if (choice == 3) standard.reset(new FourG());
    else if (choice == 4) standard.reset(new FiveG());
    else {
        io.errorstring("Invalid choice\n");
        return 1;
    }

    // Create tower/core using provided standard
    CellTower tower(standard.get());
    CellularCore core(standard.get());

    int requestedUsers = -1;
    io.outputstring("Enter number of user devices to add:\n");

    while (true) {
        requestedUsers = io.inputint();

        // basicIO returns 0 for bad input or if user types 0
        if (requestedUsers > 0) {
            break; // valid number
        }

        io.errorstring("Invalid input. Please enter a positive integer:\n");
    }

    requestedUsers = clamp_val(requestedUsers, 0, 10000000); // safety cap

    // Compute capacity
    int supported = tower.totalSupportedUsers();

    // If user requested more than supported, auto-limit and warn
    int usersToAdd = requestedUsers;
    if (requestedUsers > supported) {
        io.outputstring("Warning: Tower supports max ");
        io.outputint(supported);
        io.outputstring(" users. You requested ");
        io.outputint(requestedUsers);
        io.outputstring(". Adding only the maximum supported users.\n");
        usersToAdd = supported;
    }

    // Add (possibly limited) users
    for (int i = 0; i < usersToAdd; ++i) {
        tower.addUser(UserDevice(i + 1));
    }

    int totalUsers = tower.currentUserCount();
    int channels   = tower.numChannels();

    // MULTITHREAD: split work across hardware threads
    unsigned int hw_threads = std::thread::hardware_concurrency();
    if (hw_threads == 0) hw_threads = 2;

    int workers = (int)hw_threads;
    if (workers > totalUsers) workers = totalUsers;
    if (workers <= 0) workers = 1;

    std::vector<std::future<ThreadResult>> futures;
    futures.reserve(workers);

    int base = totalUsers / workers;
    int rem  = totalUsers % workers;

    for (int w = 0; w < workers; ++w) {
        int count = base + (w < rem ? 1 : 0);
        futures.push_back(std::async(std::launch::async,
                    compute_messages_range, standard.get(), count));
    }

    long long msgs_sum = 0;
    std::vector<ThreadResult> results;
    for (auto &f : futures) {
        ThreadResult r = f.get();
        results.push_back(r);
        msgs_sum += r.messages;
    }

    // OUTPUT RESULTS
    io.outputstring("--- Simulation Results ---\n");

    io.outputstring("Total channels: ");
    io.outputint(channels);
    io.outputstring("\n");

    io.outputstring("Tower supports (max) users: ");
    io.outputint(supported);
    io.outputstring("\n");

    io.outputstring("Current users added: ");
    io.outputint(totalUsers);
    io.outputstring("\n");

    io.outputstring("Messages generated (parallel): ");
    io.outputint((int)msgs_sum);
    io.outputstring("\n");

    io.outputstring("Users in first channel (IDs): ");
    std::vector<UserDevice> first = tower.usersInFirstChannel();
    for (size_t i = 0; i < first.size(); ++i) {
        io.outputint(first[i].getID());
        io.outputstring(" ");
    }
    io.outputstring("\n");

    // Per-thread stats (no I/O inside threads; main prints)
    io.outputstring("Per-thread processing:\n");
    for (size_t i = 0; i < results.size(); ++i) {
        io.outputstring(" Thread ");
        io.outputint((int)i + 1);
        io.outputstring(": processed ");
        io.outputint(results[i].users);
        io.outputstring(" users; messages=");
        io.outputint(results[i].messages);
        io.outputstring("\n");
    }

    // 4G extra cores estimation (if applicable)
    FourG* fg = dynamic_cast<FourG*>(standard.get());
    if (fg != 0) {
        int fullUsers = supported;
        int coreCapacity = 1000;
        int coresNeeded = (fullUsers * fg->messagesPerUser() + coreCapacity - 1) / coreCapacity;
        io.outputstring("For 4G, approx cores needed (assuming 1000 msg/core): ");
        io.outputint(coresNeeded);
        io.outputstring("\n");
    }

    // ===========================
    // SPEED INFORMATION
    // ===========================
    io.outputstring("Speed of selected technology: ");
    int s10 = standard->speedMbpsTimes10();  // Mbps × 10
    io.outputint(s10 / 10);
    io.outputstring(".");
    io.outputint(s10 % 10);
    io.outputstring(" Mbps\n");

    // Relative comparison with 2G (base = 2G stored as 1)
    io.outputstring("Relative speed compared to 2G: ");
    int factor = s10 / 1;   // 2G stored as 1
    io.outputint(factor);
    io.outputstring("x faster\n");

    io.terminate();
    return 0;
}
//garima
