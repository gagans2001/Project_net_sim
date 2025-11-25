#pragma once
#include "CommunicationStandard.h"


class FiveG : public CommunicationStandard {
public:
int usersPerChannel() const override { return 30; } // per 1 MHz at 1800 MHz
int messagesPerUser() const override { return 10; }
int channelBandwidthKHz() const override { return 1000; } // 1 MHz
int totalBandwidthKHz() const override { return 10000; } // 10 MHz example
int antennas() const override { return 16; }
int speedMbps() const override { return 1000; } // means 100 Mbps

};
