#pragma once
#ifndef FIVE_G_H
#define FIVE_G_H
#include "CommunicationStandard.h"

class FiveG : public CommunicationStandard {
public:
    int usersPerChannel() const override { return 30; }    // per 1 MHz
    int messagesPerUser() const override { return 10; }
    int channelBandwidthKHz() const override { return 1000; } // 1 MHz
    int totalBandwidthKHz() const override { return 10000; }  // 10 MHz per PDF
    int antennas() const override { return 16; }              // 16 antennas as required
    int speedMbpsTimes10() const override { return 1000; }   // 100 Mbps (×10)
    int overheadPer100Messages() const override { return 30; } // 30%
};

#endif
//Updated by Gagandeep
