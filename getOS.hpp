// Operating system compile-time determiner
// Jordan "Jorb" Dehmel, 2023
// jdehmel@outlook.com, github.com/jorbDehmel

#ifndef GETOS_H
#define GETOS_H

enum SystemType
{
    Unknown,

    Windows,
    MacOS,
    Linux,
};

#if (defined(_WIN32) || defined(_WIN64))
SystemType OS = Windows;
#elif (defined(LINUX) || defined(__linux__))
SystemType OS = Linux;
#elif (defined(__APPLE__))
SystemType OS = MacOS;
#else
SystemType OS = Unknown;
#endif

#endif
