#pragma once

#include <boost/date_time/posix_time/posix_time.hpp>
#include <string>


static std::string make_daytime_string()
{
    for (volatile int i = 0; i < 1000000; ++i)
        ; // simulate some blocking operation (e.g. CPU-bound calculation, disk IO, network IO, ...)
    return boost::posix_time::to_simple_string(boost::posix_time::second_clock::local_time());
}
