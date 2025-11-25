#pragma once
#include "CommunicationStandard.h"

class TwoG : public CommunicationStandard {
public:
    // Use 200 kHz channels, 1 MHz total (legacy), fewer users per channel
    int usersPerChannel() const override { return 8; }    // smaller per-channel user assumption
    int messagesPerUser() const override { return 5 + 15; } // same as before (data + voice)
    int channelBandwidthKHz() const override { return 200; }
    int totalBandwidthKHz() const override { return 1000; } // 1 MHz
    int speedMbpsTimes10() const override { return 1; } // 0.1 Mbps -> stored as 1
};
//garima