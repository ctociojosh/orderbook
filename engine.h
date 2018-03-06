//
// Created by Rui Zhou on 2/3/18.
//

#ifndef CS_ORDERBOOK_MATCH_ENGINE_H
#define CS_ORDERBOOK_MATCH_ENGINE_H

#include "types.h"
#include "order_book.h"


namespace CS {
    class Engine {
        HisOrderEntrys  hisOrderEntry_;
        PriceOrderBook  buyOrderBook_;
        PriceOrderBook  sellOrderBook_;

        TradeSize       recentTradeSize_;
        Price           recentTradePrice_;

    private:
        void CheckAskBidPx();

    public:
        Engine();
        void HandleOrder(const Order &order);

        void HandleTrade(const Trade &trade);

        void PrintOrderBook(std::ostream &os) const;

        void PrintTradeMessage();

        void Add(const Order &order);
        void Remove(Order &oldOrder, const Order &newOrder);
        void Modify(Order &oldOrder, Order &newOrder);
    };
}
#endif //CS_ORDERBOOK_MATCH_ENGINE_H
