#pragma once
#ifndef CELLTOWER_H
#define CELLTOWER_H

#include "UserDevice.h"
#include "CommunicationStandard.h"

class CellTower {
    CommunicationStandard* standard;
    UserDevice* usersArr; // dynamic array
    int arrCap;
    int arrCount;
public:
    CellTower(CommunicationStandard* s): standard(s), usersArr(nullptr), arrCap(0), arrCount(0) {}
    ~CellTower() {
        if (usersArr) delete [] usersArr;
        usersArr = nullptr;
        arrCap = 0;
        arrCount = 0;
    }

    void reserve(int n) {
        if (n <= arrCap) return;
        UserDevice* newArr = new UserDevice[n];
        for (int i = 0; i < arrCount; ++i) newArr[i] = usersArr[i];
        if (usersArr) delete [] usersArr;
        usersArr = newArr;
        arrCap = n;
    }

    void addUser(const UserDevice& u) {
        if (arrCount >= arrCap) {
            int newCap = (arrCap == 0) ? 16 : arrCap * 2;
            reserve(newCap);
        }
        usersArr[arrCount++] = u;
    }

    int currentUserCount() const { return arrCount; }

    const UserDevice* userArray() const { return usersArr; }

    int numChannels() const {
        int ch = standard->totalBandwidthKHz() / standard->channelBandwidthKHz();
        return (ch > 0) ? ch : 0;
    }

    int usersPerChanCapacity() const {
        return standard->usersPerChannel() * standard->antennas();
    }

    int totalSupportedUsers() const {
        long long ch = numChannels();
        long long perChan = usersPerChanCapacity();
        long long total = ch * perChan;
        if (total > 2147483647LL) return 2147483647;
        return (int)total;
    }

    // copies first-channel users into outBuffer (outBuf must be large enough)
    int usersInFirstChannel(UserDevice* outBuf, int bufSize) const {
        if (!outBuf || bufSize <= 0) return 0;
        int take = usersPerChanCapacity();
        if (take > arrCount) take = arrCount;
        if (take > bufSize) take = bufSize;
        for (int i = 0; i < take; ++i) outBuf[i] = usersArr[i];
        return take;
    }
};

#endif
//Updated by Gagandeep