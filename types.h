//
// Created by Rui Zhou on 2/3/18.
//

#ifndef CS_ORDERBOOK_TYPES_H
#define CS_ORDERBOOK_TYPES_H

#include <array>
#include "common.h"
namespace CS {

    typedef std::array<char, FixedStrLength> Str;
    typedef unsigned int Price;
    typedef unsigned int TradeSize;
    typedef uint64_t OrderId;
    typedef unsigned int Side;
}

#endif //CS_ORDERBOOK_TYPES_H
