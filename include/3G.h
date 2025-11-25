#pragma once
#include "CommunicationStandard.h"

class ThreeG : public CommunicationStandard {
public:
    // 3G cell modeled with 5 MHz total bandwidth (typical carrier)
    int usersPerChannel() const override { return 32; } // users per 200 kHz chunk
    int messagesPerUser() const override { return 10; }
    int channelBandwidthKHz() const override { return 200; }
    int totalBandwidthKHz() const override { return 5000; } // 5 MHz
    int speedMbpsTimes10() const override { return 20; } // 2.0 Mbps -> stored as 20
};
//garima