#pragma once
#include "CommunicationStandard.h"


class CellularCore {
CommunicationStandard* standard; // not owned
public:
CellularCore(CommunicationStandard* s): standard(s) {}
int messagesGenerated(int users) const {
return users * standard->messagesPerUser();
}
};
//garima