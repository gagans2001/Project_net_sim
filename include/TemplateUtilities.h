// TemplateUtilities.h
#pragma once
#ifndef TEMPLATE_UTILITIES_H
#define TEMPLATE_UTILITIES_H

template <typename T>
void debugPrint(const char* label, const T& value) {
    // Only prints if label != nullptr
    // This uses your existing io system — no std headers.
    if (label) {
        extern basicIO io;
        io.outputstring(label);
        io.outputint((int)value);
        io.outputstring("\n");
    }
}

#endif

//Kinshuk