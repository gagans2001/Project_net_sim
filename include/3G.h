#pragma once
#ifndef THREE_G_H
#define THREE_G_H
#include "CommunicationStandard.h"

class ThreeG : public CommunicationStandard {
public:
    int usersPerChannel() const override { return 32; } // per 200 kHz
    int messagesPerUser() const override { return 10; }
    int channelBandwidthKHz() const override { return 200; }
    int totalBandwidthKHz() const override { return 5000; } // 5 MHz
    int speedMbpsTimes10() const override { return 20; } // 2.0 Mbps
    int overheadPer100Messages() const override { return 4; } // 4%
};

#endif
//Updated by Gagandeep
