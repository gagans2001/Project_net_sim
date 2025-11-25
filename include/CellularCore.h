#pragma once
#ifndef CELLCORE_H
#define CELLCORE_H

#include "CommunicationStandard.h"

class CellularCore {
    CommunicationStandard* standard;
public:
    CellularCore(CommunicationStandard* s): standard(s) {}
    int messagesPerUser() const { return standard->messagesPerUser(); }
    int overheadPer100() const { return standard->overheadPer100Messages(); }
};

#endif
//Updated by Gagandeep
