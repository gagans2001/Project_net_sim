#pragma once
#include "CommunicationStandard.h"


class FourG : public CommunicationStandard {
public:
int usersPerChannel() const override { return 30; } // per 10 kHz
int messagesPerUser() const override { return 10; }
int channelBandwidthKHz() const override { return 10; }
int totalBandwidthKHz() const override { return 1000; } // example: 1 MHz default
int antennas() const override { return 4; }
int speedMbps() const override { return 200; }  // means 20 Mbps

};
