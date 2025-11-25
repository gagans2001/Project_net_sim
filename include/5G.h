#pragma once
#include "CommunicationStandard.h"

class FiveG : public CommunicationStandard {
public:
    // 5G simplified: 100 MHz total, 1 MHz "logical" channels, high MIMO
    int usersPerChannel() const override { return 30; }     // per 1 MHz
    int messagesPerUser() const override { return 10; }
    int channelBandwidthKHz() const override { return 1000; }   // 1 MHz logical slice
    int totalBandwidthKHz() const override { return 100000; }   // 100 MHz
    int antennas() const override { return 64; }                // massive MIMO (64)
    int speedMbpsTimes10() const override { return 1000; }      // 100.0 Mbps -> stored as 1000
};
//garima