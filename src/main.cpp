#include "../include/basicIO.h"
#include "../include/CommunicationStandard.h"
#include "../include/2G.h"
#include "../include/3G.h"
#include "../include/4G.h"
#include "../include/5G.h"
#include "../include/UserDevice.h"
#include "../include/CellTower.h"
#include "../include/CellularCore.h"

#include "../include/Exception.h"
#include "../include/TemplateUtilities.h"
#include "../include/ChannelRange.h"

#include <pthread.h>
#include <unistd.h>   // sysconf()

// ==================== Helpers =====================
static long long div_up(long long a, long long b) {
    if (b == 0) return 0;
    return (a + b - 1) / b;
}

const long long CORE_MSG_RATE = 1000LL;

// sequential simulated time (1 core)
static long long simulated_time_ms_nonthread(long long totalMsgs) {
    long long secs = div_up(totalMsgs, CORE_MSG_RATE);
    return secs * 1000LL;
}

// ==================== Worker Thread Structs =====================
struct WorkerArg {
    const UserDevice* arr;
    int startIndex;
    int endIndex;
    int msgsPerUser;
};

struct WorkerResult {
    int usersProcessed;
    long long messagesProcessed;
};

void* worker_func(void* v) {
    WorkerArg* a = (WorkerArg*)v;
    WorkerResult* r = new WorkerResult;
    r->usersProcessed = 0;
    r->messagesProcessed = 0;

    for (int i = a->startIndex; i < a->endIndex; i++) {
        r->usersProcessed++;
        r->messagesProcessed += a->msgsPerUser;
    }
    return (void*)r;
}

// ==================== MAIN =====================
int main() {
    try {

        io.outputstring("Cellular Network Simulator\n");
        io.outputstring("Project PDF: /mnt/data/OOPD____Project____2025.pdf\n");

        io.outputstring("Select standard:\n");
        io.outputstring("1 = 2G\n2 = 3G\n3 = 4G\n4 = 5G\n");

        int choice = io.inputint();

        if (choice < 1 || choice > 4) {
            throw InvalidInputException("Invalid standard choice (must be 1–4)");
        }

        CommunicationStandard* stds[4];
        stds[0] = new TwoG();
        stds[1] = new ThreeG();
        stds[2] = new FourG();
        stds[3] = new FiveG();

        CommunicationStandard* st = stds[choice - 1];

        io.outputstring("Enter number of user devices:\n");
        int requested = -1;

        while (requested <= 0) {
            requested = io.inputint();
            if (requested <= 0)
                io.errorstring("Enter positive integer:\n");
        }

        // Template usage (does not change functionality)
        debugPrint("DEBUG: Users requested = ", requested);

        // tower + core objects
        CellTower tower(st);
        CellularCore core(st);

        // limits
        int msgsPerUser = st->messagesPerUser();
        int overhead = st->overheadPer100Messages();
        int bandwidthCap = tower.totalSupportedUsers();

        long long coreMsgCap = 200000LL;
        long long coreMaxUsers = (coreMsgCap * 100LL) /
                                 ((long long)msgsPerUser * (100LL + overhead));

        long long finalAllowed = bandwidthCap < coreMaxUsers ? bandwidthCap : coreMaxUsers;

        if (requested > finalAllowed) {
            io.outputstring("Warning: Requested exceeds capacity.\n");
            io.outputstring("Bandwidth limit: "); io.outputint(bandwidthCap); io.outputstring("\n");
            io.outputstring("Core limit: "); io.outputint((int)coreMaxUsers); io.outputstring("\n");
            io.outputstring("Using only "); io.outputint((int)finalAllowed); io.outputstring(" users.\n");
            requested = (int)finalAllowed;
        }

        if (requested > bandwidthCap) {
            throw CapacityExceededException("User count exceeds tower capacity!");
        }

        tower.reserve(requested);
        for (int i = 0; i < requested; i++)
            tower.addUser(UserDevice(i + 1));

        int totalUsers = tower.currentUserCount();
        int channels = tower.numChannels();
        int perChanCap = tower.usersPerChanCapacity();

        long long totalMessages = totalUsers * msgsPerUser;

        // ============================================================
        // Thread count using sysconf()
        // ============================================================
        long sc = sysconf(_SC_NPROCESSORS_ONLN);
        int threads = (sc > 0) ? (int)sc : 4;
        if (threads > totalUsers) threads = totalUsers;
        if (threads <= 0) threads = 1;

        // ============================================================
        // ======== OUTPUT BASIC RESULTS ===============================
        // ============================================================
        io.outputstring("--- Simulation Results ---\n");
        io.outputstring("Total channels: "); io.outputint(channels); io.outputstring("\n");
        io.outputstring("Tower capacity: "); io.outputint(bandwidthCap); io.outputstring("\n");
        io.outputstring("Core capacity: "); io.outputint((int)coreMaxUsers); io.outputstring("\n");
        io.outputstring("Final users added: "); io.outputint(totalUsers); io.outputstring("\n");
        io.outputstring("Messages generated: "); io.outputint((int)totalMessages); io.outputstring("\n");

        // FIRST CHANNEL USERS
        io.outputstring("Users in first channel: ");
        int firstCap = perChanCap < totalUsers ? perChanCap : totalUsers;
        if (firstCap > 0) {
            UserDevice* buf = new UserDevice[firstCap];
            int k = tower.usersInFirstChannel(buf, firstCap);
            for (int i = 0; i < k; i++) {
                io.outputint(buf[i].getID());
                io.outputstring(" ");
            }
            delete[] buf;
        }
        io.outputstring("\n");

        // CHANNEL PRINTING
        io.outputstring("=== CHANNEL INFORMATION ===\n");
        io.outputstring("Total possible channels: ");
        io.outputint(channels);
        io.outputstring("\n\n");

        io.outputstring("=== ACTIVE CHANNELS ONLY ===\n");
        const UserDevice* arr = tower.userArray();
        int printed = 0;

        for (int ch = 0; ch < channels; ch++) {
            int start = ch * perChanCap;
            if (start >= totalUsers) break;

            int end = start + perChanCap;
            if (end > totalUsers) end = totalUsers;

            ChannelRange<int> cr(arr[start].getID(), arr[end - 1].getID());
            cr.print(ch + 1);


            printed++;
        }

        io.outputstring("Unused channels: ");
        io.outputint(channels - printed);
        io.outputstring("\n");

        // ============================================================
        // ========== REAL THREADING (pthread) ========================
        // ============================================================
        io.outputstring("--- Threading Comparison (real pthreads) ---\n");
        io.outputstring("Threads used: "); io.outputint(threads); io.outputstring("\n");

        pthread_t* tarr = new pthread_t[threads];
        WorkerArg* args = new WorkerArg[threads];
        WorkerResult* res = new WorkerResult[threads];

        int base = totalUsers / threads;
        int rem = totalUsers % threads;

        int start = 0;
        for (int t = 0; t < threads; t++) {
            int chunk = base + (t < rem ? 1 : 0);
            args[t].arr = arr;
            args[t].startIndex = start;
            args[t].endIndex = start + chunk;
            args[t].msgsPerUser = msgsPerUser;

            if (pthread_create(&tarr[t], NULL, worker_func, &args[t]) != 0) {
                throw ThreadException("Failed to create thread");
            }

            start += chunk;
        }

        long long maxMsgs = 0;

        for (int t = 0; t < threads; t++) {
            void* p;
            pthread_join(tarr[t], &p);
            WorkerResult* wr = (WorkerResult*)p;

            res[t] = *wr;
            if (wr->messagesProcessed > maxMsgs)
                maxMsgs = wr->messagesProcessed;

            delete wr;
        }

        long long timeNonThread_ms = simulated_time_ms_nonthread(totalMessages);
        long long timeThread_ms = simulated_time_ms_nonthread(maxMsgs);

        io.outputstring("Simulated time (non-threaded): ");
        io.outputint((int)timeNonThread_ms);
        io.outputstring(" ms\n");

        io.outputstring("Simulated time (threaded): ");
        io.outputint((int)timeThread_ms);
        io.outputstring(" ms\n");

        io.outputstring("Speedup: ");
        io.outputint((int)(timeNonThread_ms / timeThread_ms));
        io.outputstring("x\n");

        io.outputstring("Per-thread distribution:\n");
        for (int t = 0; t < threads; t++) {
            io.outputstring(" Thread ");
            io.outputint(t + 1);
            io.outputstring(": users=");
            io.outputint(res[t].usersProcessed);
            io.outputstring(" messages=");
            io.outputint((int)res[t].messagesProcessed);
            io.outputstring("\n");
        }

        delete[] tarr;
        delete[] args;
        delete[] res;

        // SPEED
        io.outputstring("Speed of selected technology: ");
        int s10 = st->speedMbpsTimes10();
        io.outputint(s10 / 10); io.outputstring(".");
        io.outputint(s10 % 10); io.outputstring(" Mbps\n");

        io.outputstring("Relative to 2G: ");
        io.outputint(s10);
        io.outputstring("x faster\n");

        // COMPARISON ALL STANDARDS
        io.outputstring("--- Comparison across all standards ---\n");
        for (int i = 0; i < 4; i++) {
            CommunicationStandard* s = stds[i];
            long long chn = s->totalBandwidthKHz() / s->channelBandwidthKHz();
            long long perC = (long long)s->usersPerChannel() * s->antennas();

            long long bcap = chn * perC;
            long long d2 = (long long)s->messagesPerUser() * (100 + s->overheadPer100Messages());
            long long ccap = (d2 > 0) ? (coreMsgCap * 100LL) / d2 : 0;

            io.outputstring("Standard ");
            io.outputint(i + 1);
            io.outputstring(": channels=");
            io.outputint((int)chn);
            io.outputstring(", bandwidth_cap=");
            io.outputint((int)bcap);
            io.outputstring(", core_cap=");
            io.outputint((int)ccap);
            io.outputstring(", speed=");
            io.outputint(s->speedMbpsTimes10() / 10);
            io.outputstring(".");
            io.outputint(s->speedMbpsTimes10() % 10);
            io.outputstring(" Mbps\n");
        }

        delete stds[0]; delete stds[1]; delete stds[2]; delete stds[3];

        io.terminate();
        return 0;

    } catch (BaseException& e) {
        io.errorstring("EXCEPTION: ");
        io.errorstring(e.what());
        io.errorstring("\n");
        return 1;
    }
}

//Kinshuk
