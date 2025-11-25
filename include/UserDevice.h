#pragma once


class UserDevice {
int id;
public:
explicit UserDevice(int id): id(id) {}
int getID() const { return id; }
};
//garima