#pragma once
#include "CommunicationStandard.h"

class FourG : public CommunicationStandard {
public:
    // LTE-like simplified model
    int usersPerChannel() const override { return 20; }  // per RB-like 180kHz
    int messagesPerUser() const override { return 10; }
    int channelBandwidthKHz() const override { return 180; }  // approximates LTE RB
    int totalBandwidthKHz() const override { return 20000; }  // 20 MHz
    int antennas() const override { return 4; }              // MIMO 4x
    int speedMbpsTimes10() const override { return 200; }    // 20.0 Mbps -> stored as 200
};
//garima