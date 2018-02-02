#ifndef LOGGED_VAL_HPP
#define LOGGED_VAL_HPP

#include <string>
using namespace std;

struct LoggedVal{
    string log;
    int val;
    LoggedVal(string, int);
    LoggedVal();
    LoggedVal(LoggedVal&);
    LoggedVal(LoggedVal&&);
    LoggedVal& operator=(const LoggedVal &);
};

#endif