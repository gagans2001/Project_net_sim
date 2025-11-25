#pragma once
#ifndef USERDEVICE_H
#define USERDEVICE_H

class UserDevice {
    int id;
public:
    UserDevice(): id(0) {}
    UserDevice(int _id): id(_id) {}
    int getID() const { return id; }
};

#endif

//Kinshuk