#pragma once

class CommunicationStandard {
public:
    virtual int usersPerChannel() const = 0;     // base users per channel (per antenna)
    virtual int messagesPerUser() const = 0;     // messages generated per user
    virtual int channelBandwidthKHz() const = 0; // channel width in kHz
    virtual int totalBandwidthKHz() const = 0;   // total allocated bandwidth in kHz
    virtual int antennas() const { return 1; }   // default single antenna

    // speed in Mbps * 10 (integer) to avoid floating I/O
    virtual int speedMbpsTimes10() const = 0;

    virtual ~CommunicationStandard() {}
};
//garima
