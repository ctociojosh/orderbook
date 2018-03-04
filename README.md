C++ Order Book

Design

    Data Structure:

    order book is implemented with std::map of std::list, one per side,
        see:
        Class Engine {
            PriceOrderBook  buyOrderBook_;
            PriceOrderBook  sellOrderBook_;
        }


        I have considered a vector as order book data structure.
        PROS:
            cache friendly, vector memory layout is continuous.
            O(1) complexity when access
        CONS:
            need more space. but in realworld it shouldn't be a problem, most order will
            be exist in a very narrow price range.

            the worst case could be O(N) when update Minimum ask price and maximum bid
            price.

            iterate order book is not convenient and could be O(N) complexity when orders
             distribute in a big range.

        I choosed std::map for easy to implementation.
        Add order   :   logN
        Modify      :   logN ~ 2logN (remove and add again)
        Remove      :   logN
        Trade       :   2logN ~ 2 O(N) (if all orders are same price, map will degrade to list)


    std::unordered_map of all orders:
        see:
        Class Engine {
            HisOrderEntrys  hisOrderEntry_;
        }
        map key is order id, value is a value type of struct Order.
        access and remove complexity is O(1)


Constraint


    namespace CS {
        const size_t MaxTradeSize = 1000000;
        const size_t MaxTradePrice = 10000000;
        const size_t MaxMsgLength = 50;
        const size_t MinMsgLength = 5;
    }

Build

    mkdir build && cd build
    cmake ../
    make


Run

    ./matchengine ../test/sample_msg.txt

    tested with Apple LLVM version 9.0.0 (clang-900.0.39.2) and cmake 3.8

    output:

    ---ORDER BOOK --------------------------------------
          1075  S 1
          1050  S 10
          1025  S 2 S 5
          1050  B 3
          1000  B 9 B 1
           975  B 30
    --- STATS --------------------------------------
                             Duplicated Add 0
                          Corrupted Message 0
     Invalid Message(negative/invalid data) 0
        No trade occur when ask/price cross 0
                     Trade on missing order 0
                     Remove a missing order 0
                             Modify ignored 0
               Remove order with wrong data 0
    --- TRADE --------------------------------------
    2@1025
    --- TRADE --------------------------------------
    3@1025
    ---ORDER BOOK --------------------------------------
          1075  S 1
          1050  S 10
          1025  S 4
          1000  B 9 B 1
           975  B 30

