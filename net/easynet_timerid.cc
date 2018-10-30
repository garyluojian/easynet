//
// Created by gary on 18-10-4.
//

#include "easynet_timerid.h"

namespace easynet
{
    namespace net
    {
        std::atomic_uint64_t TimerId::TimerIdGenerator::_id_genetator;
    }
}