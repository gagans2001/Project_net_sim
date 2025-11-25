// Exception.h
#pragma once
#ifndef CUSTOM_EXCEPTION_H
#define CUSTOM_EXCEPTION_H

class BaseException {
protected:
    const char* msg;
public:
    BaseException(const char* m) : msg(m) {}
    const char* what() const { return msg; }
};

class InvalidInputException : public BaseException {
public:
    InvalidInputException(const char* m) : BaseException(m) {}
};

class CapacityExceededException : public BaseException {
public:
    CapacityExceededException(const char* m) : BaseException(m) {}
};

class ThreadException : public BaseException {
public:
    ThreadException(const char* m) : BaseException(m) {}
};

#endif

//Kinshuk