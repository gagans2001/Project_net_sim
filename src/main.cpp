#include <vector>
#include <memory>
#include <stdexcept>
#include <thread>
#include <future>

#include "../include/basicIO.h"
#include "../include/CommunicationStandard.h"
#include "../include/2G.h"
#include "../include/3G.h"
#include "../include/4G.h"
#include "../include/5G.h"
#include "../include/UserDevice.h"
#include "../include/CellTower.h"
#include "../include/CellularCore.h"

// Template helper (still allowed)
template<typename T>
T clamp_val(T v, T lo, T hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

// Worker function (no I/O allowed inside threads)
static int compute_messages_range(CommunicationStandard* standard, int count) {
    if (!standard) return 0;
    return count * standard->messagesPerUser();
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

    CellTower tower(standard.get());
    CellularCore core(standard.get());

    io.outputstring("Enter number of user devices to add:\n");
    int n = io.inputint();
    n = clamp_val(n, 0, 200000);

    for (int i = 0; i < n; ++i) {
        tower.addUser(UserDevice(i + 1));
    }

    int totalUsers = tower.currentUserCount();
    int supported  = tower.totalSupportedUsers();
    int channels   = tower.numChannels();

    // ----------------------------
    // MULTITHREADED MESSAGE SUM
    // ----------------------------
    unsigned int hw_threads = std::thread::hardware_concurrency();
    if (hw_threads == 0) hw_threads = 2;

    int workers = (int)hw_threads;
    if (workers > totalUsers) workers = totalUsers;
    if (workers <= 0) workers = 1;

    std::vector<std::future<int>> fut;
    fut.reserve(workers);

    int base = totalUsers / workers;
    int rem  = totalUsers % workers;

    for (int w = 0; w < workers; ++w) {
        int count = base + (w < rem ? 1 : 0);
        fut.push_back(std::async(std::launch::async,
                                 compute_messages_range,
                                 standard.get(),
                                 count));
    }

    long long msgs_sum = 0;
    for (size_t i = 0; i < fut.size(); ++i) {
        msgs_sum += fut[i].get();
    }

    // ----------------------------
    // OUTPUT RESULTS
    // ----------------------------
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

    // First channel user list
    io.outputstring("Users in first channel (IDs): ");
    std::vector<UserDevice> first = tower.usersInFirstChannel();
    for (size_t i = 0; i < first.size(); ++i) {
        io.outputint(first[i].getID());
        io.outputstring(" ");
    }
    io.outputstring("\n");

    // Extra: 4G core estimate (allowed)
    FourG* fg = dynamic_cast<FourG*>(standard.get());
    if (fg != 0) {
        int fullUsers = supported;
        int coreCap = 1000;
        int need = (compute_messages_range(standard.get(), fullUsers) + coreCap - 1) / coreCap;

        io.outputstring("4G approx cores needed (1000 msg/core): ");
        io.outputint(need);
        io.outputstring("\n");
    }

    // ===========================
    // SPEED INFORMATION
    // ===========================

    io.outputstring("Speed of selected technology: ");
    int s10 = standard->speedMbps();  // stored as Mbps × 10
    io.outputint(s10 / 10);   // whole part
    io.outputstring(".");
    io.outputint(s10 % 10);   // decimal part
    io.outputstring(" Mbps\n");

    // Relative comparison with 2G (base = 0.1 Mbps stored as 1)
    io.outputstring("Relative speed compared to 2G: ");
    int factor = s10 / 1;   // since 2G = 1 (0.1 Mbps)
    io.outputint(factor);
    io.outputstring("x faster\n");

    io.terminate();
    return 0;
}
