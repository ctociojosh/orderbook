//
// Created by Rui Zhou on 2/3/18.
//

#ifndef CS_ORDERBOOK_ORDER_BOOK_H
#define CS_ORDERBOOK_ORDER_BOOK_H

#include "types.h"
#include <list>
#include <vector>
#include <unordered_map>
#include <map>
//#include <memory>

namespace CS {


    enum class MessageType {
        Unknown,
        Add,
        Remove,
        Modify,
        Trade
    };

    enum class OrderStatus {
        Normal,                 // initial status
        TradeDeleted,           // deleted via trade match
        //ExgDeleted              // deleted via receiving exchange deleted message.
    };

    const Side BUY = 0;
    const Side SELL = 1;

    /*typedef struct {
        size_t size;
    }PriceOrderItem;*/
    class PriceOrderItem {

    public:
        PriceOrderItem(size_t sz, OrderId oid):size(sz), orderId(oid){}
        size_t      size;
        OrderId     orderId;
    };

    typedef std::list<PriceOrderItem> PriceOrderList;
    typedef PriceOrderList::iterator    PriceOrderitemPtr;

    typedef struct {
        MessageType     action;     // 4
        Price           price;      // 4
        TradeSize       size;       // 4
        OrderId         orderId;    // 8
        Side            side;       // 4
        PriceOrderitemPtr    ptr;    // 8 (iterator)
        OrderStatus     status;     // 4
        void TradeDelete() {
            status = OrderStatus ::TradeDeleted;
            // ptr will be invalid!!
        }

    }Order;


    typedef struct {
        TradeSize   tradeSize;
        Price       tradePrice;
    }Trade;

    // TODO use plain doubly linked list
    class SamePriceOrderList :public PriceOrderList{
        TradeSize       totalTradeSize_;
    public:
        uint32_t GetTradeSize() {
            return totalTradeSize_;
        }
        void ChangeTradeSize(TradeSize sz) {
            totalTradeSize_+=sz;
        }
    };

    typedef Order   OrderEntryType;

    //typedef std::vector<PriceOrderList> PriceOrderBook;
    //typedef std::map<Price, PriceOrderList> PriceOrderBook;
    typedef std::map<Price, SamePriceOrderList> PriceOrderBook;
    //typedef std::vector<Order>  HisOrderEntrys;
    typedef std::unordered_map<OrderId, Order> HisOrderEntrys;


}


#endif //CS_ORDERBOOK_ORDER_BOOK_H
