//
// Created by Rui Zhou on 3/3/18.
//

#ifndef CS_ORDERBOOK_ERROR_MONITOR_H
#define CS_ORDERBOOK_ERROR_MONITOR_H

#include <stdint.h>
#include <iostream>
#include <iomanip>

namespace CS {
    class ErrorMonitor {
        uint64_t    duplicateAdd_;
        uint64_t    corruptedMsg_;
        uint64_t    invalidMsg_;
        uint64_t    bidAskCross_;
        uint64_t    tradeOnMissingOrder_;
        uint64_t    invalidTradeSize_;
        uint64_t    cancelMissingOrder_;
        uint64_t    modifyIgnored_;
        uint64_t    removeWrongData_;
    public:
        static ErrorMonitor& GetInstance() {
            static ErrorMonitor instance;
            return instance;
        }

        void DuplicateAdd() {
            ++duplicateAdd_;
        };

        void CorruptMessage() {
            ++corruptedMsg_;
        }

        // negative, invalid trade size
        void InvalidMsg() {
            invalidMsg_++;
        }

        void CrossBidAsk() {
            bidAskCross_++;
        }

        void TradeOnMissingOrder() {
            tradeOnMissingOrder_++;
        }

        void InvalidTradeSize() {
            invalidTradeSize_++;
        }

        void CancelMissingOrder() {
            cancelMissingOrder_++;
        }

        void RemoveWithWrongData() {
            removeWrongData_++;
        }

        void ModifyIgnored() {
            modifyIgnored_++;
        }


        void PrintStats() {
            std::cout << "--- STATS --------------------------------------\n";
            int align = 40;
            std::cout << std::setw(align) << "Duplicated Add " << duplicateAdd_ <<std::endl;
            std::cout << std::setw(align) << "Corrupted Message " << corruptedMsg_ <<std::endl;
            std::cout << std::setw(align) << "Invalid Message(negative/invalid data) "  << invalidMsg_ <<std::endl;
            std::cout << std::setw(align) << "No trade occur when ask/price cross " << std::setw(-30) << bidAskCross_ <<std::endl;
            std::cout << std::setw(align) << "Trade on missing order " << tradeOnMissingOrder_ << std::setw(-30) <<std::endl;
            //std::cout << std::setw(40) << "Invalid trade size " << invalidTradeSize_ <<std::endl;
            std::cout << std::setw(align) << "Remove a missing order " <<  cancelMissingOrder_ <<std::endl;
            std::cout << std::setw(align) << "Modify ignored " <<  modifyIgnored_ <<std::endl;
            std::cout << std::setw(align) << "Remove order with wrong data " << removeWrongData_ <<std::endl;

        }


    };
}
#endif //CS_ORDERBOOK_ERROR_MONITOR_H
