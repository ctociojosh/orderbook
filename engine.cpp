//
// Created by Rui Zhou on 2/3/18.
//

#include <climits>
#include <algorithm>
#include <iostream>
#include <iomanip>

#include "engine.h"
#include "error_monitor.h"
#include "order_book.h"


using namespace std;

namespace CS {
    Engine::Engine()
    :recentTradeSize_(0),recentTradePrice_(0)
    {
    }

    void Engine::HandleOrder(const Order &order) {
        auto &&oldOrderIt = hisOrderEntry_.find(order.orderId);

        if (oldOrderIt == hisOrderEntry_.end()) {
            if (order.action == MessageType ::Add) {
                Add(order);
            } else {
                // missing order
                ErrorMonitor::GetInstance().CancelMissingOrder();
            }
        } else {
            // order exist
            Order &oldOrder = oldOrderIt->second;

            if (order.action == MessageType::Add) {
                // duplicate add
                ErrorMonitor::GetInstance().DuplicateAdd();

            } else if (order.action == MessageType ::Remove) {

                Remove(oldOrder, order);

            } else if (order.action == MessageType ::Modify) {
                //skip same price and same size
                if (oldOrder.size == order.size && oldOrder.price == order.price) {
                    //it is just a notification after trade match or
                    //it is intend to modify but without change, ignore it too.
                    return;
                }

                Modify(oldOrder, const_cast<Order&>(order));
                // update oldOrder info
                //oldOrder.size = order.size;
                //oldOrder.price = order.price;

            } else {
                //TODO not possible here

            }
        }
    }

    void Engine::CheckAskBidPx() {
        if (buyOrderBook_.empty() || sellOrderBook_.empty()) {
            return;
        }
        auto &&sellIter = sellOrderBook_.begin();
        auto &&buyIter = buyOrderBook_.rbegin();
        if (sellIter->first <= buyIter->first) {
            ErrorMonitor::GetInstance().CrossBidAsk();
        }
    }

    //
    void Engine::Modify(Order &oldOrder, Order &newOrder) {
        CheckAskBidPx();

        if (oldOrder.status == OrderStatus::TradeDeleted) {
            ErrorMonitor::GetInstance().modifyOrderDeleted();
            return;
        }

        if (oldOrder.size == newOrder.size &&
                oldOrder.side == newOrder.side &&
                oldOrder.price == newOrder.price) {
            // this is either modify order but without any change or
            // after trade happened, an exchange message to reflect the order book/hisentry
            // change(which was done in HandleTrade)
            ErrorMonitor::GetInstance().ModifyIgnored();
            return;
        }

        // not same side or not same price
        if (newOrder.side != oldOrder.side || newOrder.price != oldOrder.price) {
            // remove old order
            Remove(oldOrder, oldOrder);
            Add(newOrder);
        } else {
            // same side, same price, diff size, keep time priority
            // TODO if size = 0
            if (newOrder.side == BUY) {
                auto &&buyOrderList = buyOrderBook_.find(oldOrder.price);
                if (buyOrderList != buyOrderBook_.end()) {
                    oldOrder.ptr->size = newOrder.size;
                } else {
                    ErrorMonitor::GetInstance().modifyOrderDeleted();
                    return;
                }
            } else if (newOrder.side == SELL) {
                auto &&sellOrderList = sellOrderBook_.find(oldOrder.price);
                if (sellOrderList != sellOrderBook_.end()) {
                    oldOrder.ptr->size = newOrder.size;
                } else {
                    ErrorMonitor::GetInstance().modifyOrderDeleted();
                    return;
                }
            }
        }
    }

    void Engine::Remove(Order &oldOrder, const Order &newOrder) {
        CheckAskBidPx();

        // to remove order, their price, size, side must be equal.
        if (oldOrder.status != OrderStatus::TradeDeleted) {
            if (oldOrder.price != newOrder.price ||
                oldOrder.size != newOrder.size ||
                oldOrder.side != newOrder.side) {
                ErrorMonitor::GetInstance().RemoveWithWrongData();
                return;
            }
        }


        // if the order status is 'trade delete' means we don't need to modify order book
        if (oldOrder.status != OrderStatus::TradeDeleted) {
            PriceOrderBook &map = oldOrder.side == BUY ? buyOrderBook_ : sellOrderBook_;
            auto &&iter = map.find(oldOrder.price);

            if (iter == map.end()) {
                ErrorMonitor::GetInstance().RemoveWithWrongData();
            } else {
                auto &&ordList = iter->second;
                ordList.erase(oldOrder.ptr);
                // change order list total size
                ordList.ChangeTradeSize(-oldOrder.size);

                if (ordList.empty()) {
                    map.erase(iter);
                }
            }
        }

        // remove from hisorderentry.
        auto &&oldOrderIt = hisOrderEntry_.find(oldOrder.orderId);
        if (oldOrderIt != hisOrderEntry_.end()) {
            hisOrderEntry_.erase(oldOrderIt);
        }
    }

    void Engine::Add(const Order &tmpOrder) {
        CheckAskBidPx();

        auto &&ret = hisOrderEntry_.insert(std::make_pair(tmpOrder.orderId, tmpOrder));
        Order &order = ret.first->second;

        // put in the book
        PriceOrderBook &map = order.side == BUY? buyOrderBook_ : sellOrderBook_;
        auto &&iter = map.find(order.price);
        if (iter!=map.end()) {
            iter->second.emplace_back(order.size, order.orderId);
            iter->second.ChangeTradeSize(order.size);
            order.ptr = iter->second.end();
            order.ptr--;
        } else {
            auto &&ret = map.insert(std::make_pair(order.price, SamePriceOrderList()));
            ret.first->second.emplace_back(order.size, order.orderId);
            ret.first->second.ChangeTradeSize(order.size);
            order.ptr = ret.first->second.end();
            order.ptr--;
        }
    }

    void Engine::HandleTrade(const Trade &trade) {
        // Not check bid and ask price
        // ensure there is cross book?
        if (buyOrderBook_.empty() || sellOrderBook_.empty()) {
            ErrorMonitor::GetInstance().TradeOnMissingOrder();
            return;
        }

        auto &&buyIter = buyOrderBook_.end();
        buyIter--;
        if (buyIter->first < trade.tradePrice) {
            ErrorMonitor::GetInstance().TradeOnMissingOrder();
            return;
        }


        auto &&sellIter = sellOrderBook_.find(trade.tradePrice);
        if (sellIter == sellOrderBook_.end()) {
            ErrorMonitor::GetInstance().TradeOnMissingOrder();
            return;
        }

        //ensure enough orderlist quantity
        TradeSize  ts = trade.tradeSize;
        auto &&sellOrderList = sellIter->second;
        auto &&buyOrderList = buyIter->second;
        if (buyOrderList.GetTradeSize() < trade.tradeSize ||
                sellOrderList.GetTradeSize() < trade.tradeSize) {
            ErrorMonitor::GetInstance().TradeOnMissingOrder();
            return;
        } else {
            sellOrderList.ChangeTradeSize(-ts);
            buyOrderList.ChangeTradeSize(-ts);
        }



        // sell order book
        auto &&curSellItem = sellOrderList.begin();
        auto &&endSellItem = sellOrderList.end();

        while (curSellItem != endSellItem) {
            auto &&orignOrd = this->hisOrderEntry_.find(curSellItem->orderId);
            if (orignOrd!=hisOrderEntry_.end()) {
                if (ts >= curSellItem->size) {
                    // mark order in hisorderentry as trade delete
                    orignOrd->second.TradeDelete();
                } else {
                    // change the hisorderentry order size
                    orignOrd->second.size -= ts;
                }
            } else {
                ErrorMonitor::GetInstance().TradeOnMissingOrder();
            }

            if (ts >= curSellItem->size) {
                ts -= curSellItem->size;
                ++curSellItem;
            } else {
                curSellItem->size -= ts;
                ts = 0;
                break;
            }
        }
        sellOrderList.erase(sellOrderList.begin(), curSellItem);
        if (sellOrderList.empty()) {
            sellOrderBook_.erase(sellIter);
        }

        // buy order book
        ts = trade.tradeSize;

        auto &&curBuyItem = buyOrderList.begin();
        auto &&endBuyItem = buyOrderList.end();

        while (curBuyItem != endBuyItem) {
            auto &&orignOrd = this->hisOrderEntry_.find(curBuyItem->orderId);
            if (orignOrd!=hisOrderEntry_.end()) {
                if (ts >= curBuyItem->size) {
                    // mark order in hisorderentry as trade delete
                    orignOrd->second.TradeDelete();
                } else {
                    // change the hisorderentry order size
                    orignOrd->second.size -= ts;
                }
            } else {
                ErrorMonitor::GetInstance().TradeOnMissingOrder();
            }

            if (ts >= curBuyItem->size) {
                ts -= curBuyItem->size;
                ++curBuyItem;
            } else {
                curBuyItem->size -= ts;
                ts = 0;
                break;
            }
        }
        buyOrderList.erase(buyOrderList.begin(),curBuyItem);


        if (buyOrderList.empty()) {
            buyOrderBook_.erase(buyIter);
        }

        // print recent trade message =================================
        if (recentTradePrice_ != trade.tradePrice) {
            recentTradePrice_ = trade.tradePrice;
            recentTradeSize_ = 0;
        }
        recentTradeSize_ += trade.tradeSize;
#ifndef PROFILE
        std::cout << "--- TRADE --------------------------------------\n";
        std::cout << recentTradeSize_ << "@" << (double)(recentTradePrice_/ 100.) << endl;
#endif
    }


    void Engine::PrintOrderBook(std::ostream &os) const {
        os << "---ORDER BOOK --------------------------------------\n";
        for (auto it = sellOrderBook_.rbegin(); it != sellOrderBook_.rend(); ++it) {
            auto &&lst = it->second;
            os << std::setw(10) << it->first /100.<< " ";
            for (auto &&sz : lst) {
                os << " S " << sz.size;
            }
            os << endl;
        }

        for (auto it = buyOrderBook_.rbegin(); it != buyOrderBook_.rend(); ++it) {
            auto &&lst = it->second;
            os << std::setw(10) << it->first /100.<< " ";
            for (auto &&sz : lst) {
                os << " B " << sz.size;
            }
            os << endl;
        }
    }

}