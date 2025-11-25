#pragma once
#ifndef COMMUNICATIONSTANDARD_H
#define COMMUNICATIONSTANDARD_H

class CommunicationStandard {
public:
    virtual int usersPerChannel() const = 0;
    virtual int messagesPerUser() const = 0;
    virtual int channelBandwidthKHz() const = 0;
    virtual int totalBandwidthKHz() const = 0;
    virtual int antennas() const { return 1; }
    virtual int speedMbpsTimes10() const = 0;
    virtual int overheadPer100Messages() const = 0;
    virtual ~CommunicationStandard() {}
};

#endif
//Updated by Gagandeep
