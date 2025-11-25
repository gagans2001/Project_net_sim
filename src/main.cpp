#include "../include/basicIO.h"
#include "../include/CommunicationStandard.h"
#include "../include/2G.h"
#include "../include/3G.h"
#include "../include/4G.h"
#include "../include/5G.h"
#include "../include/UserDevice.h"
#include "../include/CellTower.h"
#include "../include/CellularCore.h"
#include <thread> // allowed

// helper: integer ceil division
static long long div_up(long long a, long long b) {
    if (b == 0) return 0;
    return (a + b - 1) / b;
}

// Simulated core message rate per core (design parameter)
const long long CORE_MSG_RATE = 1000LL; // messages/sec per core (simulation window)

// compute simulated time for single-threaded (one core)
static long long simulated_time_ms_nonthread(long long totalMessages, int cores) {
    if (cores <= 0) cores = 1;
    long long denom = CORE_MSG_RATE * (long long)cores;
    if (denom == 0) return 0;
    long long secs = div_up(totalMessages, denom);
    return secs * 1000LL;
}

// threaded simulated time (divide load across threads)
static long long simulated_time_ms_threaded(long long totalMessages, int threads) {
    if (threads <= 0) threads = 1;
    long long base = totalMessages / threads;
    long long rem = totalMessages % threads;
    long long maxMsgs = base + (rem > 0 ? 1 : 0);
    long long secs = div_up(maxMsgs, CORE_MSG_RATE);
    return secs * 1000LL;
}

int main() {
    io.outputstring("Cellular Network Simulator\n");
    io.outputstring("Project PDF: /mnt/data/OOPD____Project____2025.pdf\n");
    io.outputstring("Select standard:\n");
    io.outputstring("1 = 2G\n2 = 3G\n3 = 4G\n4 = 5G\n");

    int choice = io.inputint();

    // create standard objects (raw pointers)
    CommunicationStandard* allStd[4];
    allStd[0] = new TwoG();
    allStd[1] = new ThreeG();
    allStd[2] = new FourG();
    allStd[3] = new FiveG();

    if (choice < 1 || choice > 4) {
        io.errorstring("Invalid choice\n");
        delete allStd[0]; delete allStd[1]; delete allStd[2]; delete allStd[3];
        return 1;
    }

    CommunicationStandard* st = allStd[choice - 1];

    io.outputstring("Enter number of user devices to add:\n");
    int requested = -1;
    while (true) {
        requested = io.inputint();
        if (requested > 0) break;
        io.errorstring("Invalid input. Please enter a positive integer:\n");
    }

    // build tower and core
    CellTower tower(st);
    CellularCore core(st);

    // compute bandwidth capacity
    int bandwidthCapacity = tower.totalSupportedUsers();

    // compute core capacity based on overhead per 100 messages
    int msgsPerUser = st->messagesPerUser();
    int overheadPer100 = st->overheadPer100Messages();

    const long long coreMessageCapacity = 200000LL; // simulation parameter
    long long numerator = coreMessageCapacity * 100LL;
    long long denom = (long long)msgsPerUser * (100LL + overheadPer100);
    long long coreMaxUsers = 0;
    if (denom > 0) coreMaxUsers = numerator / denom;

    long long finalAllowed = (bandwidthCapacity < coreMaxUsers) ? bandwidthCapacity : coreMaxUsers;

    int usersToAdd = requested;
    if ((long long)requested > finalAllowed) {
        io.outputstring("Warning: Requested users exceed capacity.\n");
        io.outputstring("Capacity by bandwidth: "); io.outputint(bandwidthCapacity); io.outputstring("\n");
        io.outputstring("Capacity by core (after overhead): "); io.outputint((int)coreMaxUsers); io.outputstring("\n");
        io.outputstring("Adding only "); io.outputint((int)finalAllowed); io.outputstring(" users.\n");
        usersToAdd = (int)finalAllowed;
    }

    // reserve and add users
    tower.reserve(usersToAdd);
    for (int i = 0; i < usersToAdd; ++i) {
        tower.addUser(UserDevice(i + 1));
    }

    int totalUsers = tower.currentUserCount();
    int channels = tower.numChannels();
    int perChanCap = tower.usersPerChanCapacity();

    long long totalMessages = (long long)totalUsers * msgsPerUser;

    unsigned int hw = std::thread::hardware_concurrency();
    if (hw == 0) hw = 2;
    int threads = (int)hw;
    if (threads > totalUsers) threads = totalUsers;
    if (threads <= 0) threads = 1;

    long long time_threaded_ms = simulated_time_ms_threaded(totalMessages, threads);
    long long time_nonthread_ms = simulated_time_ms_nonthread(totalMessages, 1);

    // ---------------------------------------
    // OUTPUT
    // ---------------------------------------
    io.outputstring("--- Simulation Results ---\n");
    io.outputstring("Total channels: "); io.outputint(channels); io.outputstring("\n");
    io.outputstring("Tower supports (max) users (bandwidth): "); io.outputint(bandwidthCapacity); io.outputstring("\n");
    io.outputstring("Core supports (max) users (after overhead): "); io.outputint((int)coreMaxUsers); io.outputstring("\n");
    io.outputstring("Final users added: "); io.outputint(totalUsers); io.outputstring("\n");
    io.outputstring("Messages generated: "); io.outputint((int)totalMessages); io.outputstring("\n");

    // first channel users
    io.outputstring("Users in first channel (IDs): ");
    int tempCap = perChanCap;
    if (tempCap > totalUsers) tempCap = totalUsers;
    if (tempCap > 0) {
        UserDevice* buf = new UserDevice[tempCap];
        int taken = tower.usersInFirstChannel(buf, tempCap);
        for (int i = 0; i < taken; ++i) {
            io.outputint(buf[i].getID());
            io.outputstring(" ");
        }
        delete [] buf;
    }
    io.outputstring("\n");

    // ************************************************************
    // CLEAN CHANNEL PRINTING (ONLY active channels + summary)
    // ************************************************************

    io.outputstring("=== CHANNEL INFORMATION ===\n");

    // 1) Print total channels available
    io.outputstring("Total possible channels (based on bandwidth): ");
    io.outputint(channels);
    io.outputstring("\n\n");

    // 2) Print only channels that actually contain users
    io.outputstring("=== USERS IN ACTIVE CHANNELS ===\n");

    const UserDevice* arr = tower.userArray();
    int printedChannels = 0;

    for (int ch = 0; ch < channels; ++ch) {
        int start = ch * perChanCap;
        if (start >= totalUsers) break;  // stop printing when no more users

        io.outputstring("Channel ");
        io.outputint(ch + 1);
        io.outputstring(": ");

        int end = start + perChanCap;
        if (end > totalUsers) end = totalUsers;

        for (int idx = start; idx < end; ++idx) {
            io.outputint(arr[idx].getID());
            io.outputstring(" ");
        }
        io.outputstring("\n");
        printedChannels++;
    }

    // 3) Print unused channels summary
    io.outputstring("\nUnused channels: ");
    io.outputint(channels - printedChannels);
    io.outputstring("\n");

    // *************************************************************

    // threading comparison
    io.outputstring("--- Threading Comparison (simulated) ---\n");
    io.outputstring("Threads used (hardware concurrency): "); io.outputint(threads); io.outputstring("\n");
    io.outputstring("Simulated time (non-threaded, single core) ms: "); io.outputint((int)time_nonthread_ms); io.outputstring("\n");
    io.outputstring("Simulated time (threaded) ms: "); io.outputint((int)time_threaded_ms); io.outputstring("\n");

    int speedup = 0;
    if (time_threaded_ms > 0) speedup = (int)(time_nonthread_ms / time_threaded_ms);
    io.outputstring("Approx speedup (non-threaded / threaded): "); io.outputint(speedup); io.outputstring("x\n");

    io.outputstring("Per-thread simulated distribution:\n");
    int base = totalUsers / threads;
    int rem = totalUsers % threads;
    for (int t = 0; t < threads; ++t) {
        int ucount = base + (t < rem ? 1 : 0);
        long long msgs = (long long)ucount * msgsPerUser;
        io.outputstring(" Thread ");
        io.outputint(t + 1);
        io.outputstring(": users=");
        io.outputint(ucount);
        io.outputstring("; messages=");
        io.outputint((int)msgs);
        io.outputstring("\n");
    }

    // speed
    io.outputstring("Speed of selected technology: ");
    int s10 = st->speedMbpsTimes10();
    io.outputint(s10 / 10); io.outputstring(".");
    io.outputint(s10 % 10); io.outputstring(" Mbps\n");
    io.outputstring("Relative to 2G: "); io.outputint(s10 / 1); io.outputstring("x faster\n");

    // comparison all standards
    io.outputstring("--- Comparison across all standards ---\n");
    for (int i = 0; i < 4; ++i) {
        CommunicationStandard* s = allStd[i];
        long long chnum = s->totalBandwidthKHz() / s->channelBandwidthKHz();
        long long perchan = (long long)s->usersPerChannel() * s->antennas();
        long long bcap = chnum * perchan;
        long long denom2 = (long long)s->messagesPerUser() * (100 + s->overheadPer100Messages());
        long long cmax = 0;
        if (denom2 > 0) cmax = (coreMessageCapacity * 100LL) / denom2;
        io.outputstring("Standard "); io.outputint(i + 1);
        io.outputstring(": channels="); io.outputint((int)chnum);
        io.outputstring(", bandwidth_cap="); io.outputint((int)bcap);
        io.outputstring(", core_cap="); io.outputint((int)cmax);
        io.outputstring(", speed=");
        io.outputint(s->speedMbpsTimes10() / 10); io.outputstring(".");
        io.outputint(s->speedMbpsTimes10() % 10); io.outputstring(" Mbps\n");
    }

    delete allStd[0]; delete allStd[1]; delete allStd[2]; delete allStd[3];

    io.terminate();
    return 0;
}
//Updated by Gagandeep
