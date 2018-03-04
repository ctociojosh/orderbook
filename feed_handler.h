//
// Created by Rui Zhou on 2/3/18.
//

#ifndef CS_ORDERBOOK_FEED_HANDLER_H
#define CS_ORDERBOOK_FEED_HANDLER_H

#include <string>
#include "engine.h"
#include "error_monitor.h"

namespace CS {
    class FeedHandler
    {
    private:
        Engine engine_;

        enum class ParseResult {
            Good,
            CorruptMessage,
            InvalidMsgData
        };

        bool ParseTrade(char *tokenMsg, Trade &trd);

        bool ParseOrder(char *tokenMsg, Order &order);

        void LogParseError(ParseResult err);

        ParseResult ParseTokenAsUInt64(char *&tk_msg, uint64_t &dest) {
            tk_msg = strtok(NULL, ",");
            if (tk_msg == NULL) {
                return ParseResult::CorruptMessage;
            }
            else {
                char * pEnd = NULL;
                dest = std::strtoull(tk_msg, &pEnd, 10);
                if (*pEnd)
                    return ParseResult::InvalidMsgData;
            }
            return ParseResult ::Good;
        }


        ParseResult ParseTokenAsUInt(char *&tk_msg, uint32_t &dest) {
            tk_msg = strtok(NULL, ",");
            if (tk_msg == NULL) {
                return ParseResult::CorruptMessage;
            }
            else {
                char * pEnd = NULL;
                dest = strtol(tk_msg, &pEnd, 10);
                if (*pEnd)
                    return ParseResult::InvalidMsgData;
            }
            return ParseResult ::Good;
        }

        ParseResult ParseSide(char *&tk_msg, uint32_t &dest) {
            tk_msg = strtok(NULL, ",");
            if (tk_msg == NULL) {
                return ParseResult::CorruptMessage;
            }
            else {
                if (tk_msg[0]=='S') {
                    dest = SELL;
                } else if (tk_msg[0]=='B') {
                    dest = BUY;
                } else {
                    return ParseResult::InvalidMsgData;
                }
            }
            return ParseResult ::Good;
        }

        ParseResult ParsePrice(char *&tk_msg, uint32_t &dest) {
            tk_msg = strtok(NULL, " ");
            if (tk_msg == NULL) {
                return ParseResult::CorruptMessage;
            }
            else {
                char * pEnd = NULL;
                dest = strtol(tk_msg, &pEnd, 10);
                if (*pEnd)
                    return ParseResult::InvalidMsgData;
            }
            return ParseResult ::Good;
        }
        MessageType GetMessageType(char *tokenMsg) {
            uint32_t len = strlen(tokenMsg);
            //check length
            if (len == 0 || len > MaxMsgLength) {
                ErrorMonitor::GetInstance().CorruptMessage();
                return MessageType::Unknown;
            }

            tokenMsg = strtok(tokenMsg, ",");
            switch(tokenMsg[0]) {
                case 'A':
                    return MessageType::Add;
                    break;
                case 'M':
                    return MessageType::Modify;
                    break;
                case 'X':
                    return MessageType::Remove;
                    break;
                case 'T':
                    return MessageType::Trade;
                    break;
                default:
                    return MessageType::Unknown;
                    break;
            }
        }
    public:
        FeedHandler();
        void ProcessMessage(const std::string &line);
        void PrintCurrentOrderBook(std::ostream &os) const;

    };
}


#endif //CS_ORDERBOOK_FEED_HANDLER_H
