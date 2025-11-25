#pragma once
#include "CommunicationStandard.h"


class ThreeG : public CommunicationStandard {
public:
int usersPerChannel() const override { return 32; } // per 200 kHz
int messagesPerUser() const override { return 10; }
int channelBandwidthKHz() const override { return 200; }
int totalBandwidthKHz() const override { return 1000; }
int speedMbps() const override { return 20; }   // means 2 Mbps

};
