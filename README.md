C++ Order Book

Build
    mkdir build && cd build
    cmake ../
    make

Run
    ./matchengine test/sample_msg.txt

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


