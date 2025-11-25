#pragma once
#include <stdexcept>


class CommunicationStandard {
public:
virtual int usersPerChannel() const = 0; // per channel (base per antenna)
virtual int messagesPerUser() const = 0; // messages generated per user
virtual int channelBandwidthKHz() const = 0; // channel width in kHz
virtual int totalBandwidthKHz() const = 0; // total allocated bandwidth in kHz
virtual int antennas() const { return 1; } // default
virtual int speedMbps() const = 0;
virtual ~CommunicationStandard() {}
};


// small helper exception
class ConfigError : public std::runtime_error {
public:
ConfigError(const char* m): std::runtime_error(m) {}
};
