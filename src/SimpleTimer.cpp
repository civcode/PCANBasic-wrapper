#include <iostream>

#include "SimpleTimer.h"

using namespace std::chrono;

SimpleTimer::SimpleTimer(const std::chrono::milliseconds interval) : 
    interval_(duration_cast<microseconds>(interval)),
    target_(high_resolution_clock::now() + interval)
{};

SimpleTimer::SimpleTimer(const std::chrono::seconds interval) : 
    interval_(duration_cast<microseconds>(interval)),
    target_(high_resolution_clock::now() + interval)
{};

SimpleTimer::SimpleTimer(const std::chrono::microseconds interval) : 
    interval_(duration_cast<microseconds>(interval)),
    target_(high_resolution_clock::now() + interval)
{};


bool SimpleTimer::is_expired() {
    //auto dt_us = duration_cast<microseconds>(high_resolution_clock::now()-target_);
    auto dt = (high_resolution_clock::now()-target_);
    if (dt.count() >= 0) {
        //std::cout << "\n now (" << dt_us.count()/1000 << ")\n";
        //std::cout << " / " << target_.count()
        target_ += interval_;

        return true;
    } else {
        //std::cout << "(" << dt_us.count()/1000 << ")";
    }
   return false; 
}
