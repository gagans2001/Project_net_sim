#pragma once
#ifndef FOUR_G_H
#define FOUR_G_H
#include "CommunicationStandard.h"

class FourG : public CommunicationStandard {
public:
    int usersPerChannel() const override { return 30; }    // per 10 kHz
    int messagesPerUser() const override { return 10; }
    int channelBandwidthKHz() const override { return 10; }  // 10 kHz
    int totalBandwidthKHz() const override { return 30000; } // e.g., 30 MHz (reasonable)
    int antennas() const override { return 4; }
    int speedMbpsTimes10() const override { return 200; }   // 20 Mbps (×10)
    int overheadPer100Messages() const override { return 15; } // 15%
};

#endif
//Updated by Gagandeep