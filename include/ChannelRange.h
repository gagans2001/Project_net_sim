#pragma once
#ifndef CHANNEL_RANGE_H
#define CHANNEL_RANGE_H

#include "basicIO.h"

template <typename T>
class ChannelRange {
    T startUser;
    T endUser;
public:
    ChannelRange(T s = 0, T e = 0) : startUser(s), endUser(e) {}

    void print(int channelNumber) const {
        extern basicIO io;

        io.outputstring("Channel ");
        io.outputint(channelNumber);
        io.outputstring(": ");

        io.outputint(startUser);
        io.outputstring("–");
        io.outputint(endUser);

        io.outputstring("\n");
    }
};

#endif

//Kinshuk 