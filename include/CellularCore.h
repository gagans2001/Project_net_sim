#pragma once
#include "CommunicationStandard.h"


class CellularCore {
CommunicationStandard* standard; // not owned
public:
CellularCore(CommunicationStandard* s): standard(s) { if(!s) throw ConfigError("Null standard to core"); }
int messagesGenerated(int users) const {
return users * standard->messagesPerUser();
}
};
