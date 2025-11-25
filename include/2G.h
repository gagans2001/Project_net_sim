#pragma once
#ifndef TWO_G_H
#define TWO_G_H
#include "CommunicationStandard.h"

class TwoG : public CommunicationStandard {
public:
    int usersPerChannel() const override { return 16; }    // per 200 kHz
    int messagesPerUser() const override { return 5 + 15; } // 20
    int channelBandwidthKHz() const override { return 200; }
    int totalBandwidthKHz() const override { return 1000; } // 1 MHz
    int speedMbpsTimes10() const override { return 1; } // 0.1 Mbps
    int overheadPer100Messages() const override { return 2; } // 2%
};

#endif
//Updated by Gagandeep
