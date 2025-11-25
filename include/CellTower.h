#pragma once
#include <vector>
#include "UserDevice.h"
#include "CommunicationStandard.h"


class CellTower {
std::vector<UserDevice> users;
CommunicationStandard* standard; // not owned
public:
CellTower(CommunicationStandard* s): standard(s) {}

void addUser(const UserDevice& u) { users.push_back(u); }


int numChannels() const {
return standard->totalBandwidthKHz() / standard->channelBandwidthKHz();
}


int totalSupportedUsers() const {
return numChannels() * standard->usersPerChannel() * standard->antennas();
}


std::vector<UserDevice> usersInFirstChannel() const {
int capacity = standard->usersPerChannel() * standard->antennas();
std::vector<UserDevice> out;
for (int i = 0; i < capacity && i < (int)users.size(); ++i) out.push_back(users[i]);
return out;
}


int currentUserCount() const { return (int)users.size(); }
};
//garima