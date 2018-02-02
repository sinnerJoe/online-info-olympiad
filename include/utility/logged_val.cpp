#include <utility/logged_val.hpp>
#include <string> 
using namespace std;
LoggedVal::LoggedVal(string mess, int val) : log{mess}, val{val}{}
LoggedVal::LoggedVal(LoggedVal &other){
    log = other.log;
    val = other.val;
}
LoggedVal::LoggedVal() : val{0}, log{""} {}
LoggedVal::LoggedVal(LoggedVal &&other){
    log = other.log;
    val = other.val;
}

LoggedVal &LoggedVal::operator=(const LoggedVal &other){
    log = other.log;
    val = other.val;
}