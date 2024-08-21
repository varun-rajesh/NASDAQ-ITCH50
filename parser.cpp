#include <iostream>
#include <fstream>
#include <cstring>
#include <byteswap.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "helper.h"
#include "instrument_table.h"
#include "market_participants.h"
#include "order_book.h"

string ITCH_FEED = "12302019.NASDAQ_ITCH50";

int main(int argc, char *argv[])
{
    std::ifstream file(ITCH_FEED, std::ios::binary);
    InstrumentTable i_table = InstrumentTable();
    MarketParticipantTable mp_table = MarketParticipantTable();
    OrderBook order_book = OrderBook();
    uint64_t i = 0;

    while (true)
    {
        i++;
        uint16_t length = read_length(&file);
        MessageType message_type = read_message_type(&file);

        if (message_type == MessageType::SYSTEM_EVENT_MESSAGE)
        {
            SystemEventMessage message = read_system_event_message(&file, length - 1);
        }
        else if (message_type == MessageType::STOCK_DIRECTORY_MESSAGE)
        {
            StockDirectoryMessage message = read_stock_directory_message(&file, length - 1);
            i_table.add_to_instrument_table(message);
        }
        else if (message_type == MessageType::STOCK_TRADING_ACTION_MESSAGE)
        {
            StockTradingActionMessage message = read_stock_trading_action_message(&file, length - 1);
            i_table.add_stock_trading_action_message(message);
        }
        else if (message_type == MessageType::REG_SHO_RESTRICTION)
        {
            RegSHORestriction message = read_reg_sho_restriction(&file, length - 1);
            i_table.add_reg_sho_restriction(message);
        }
        else if (message_type == MessageType::MARKET_PARTICIPANT_POSITION)
        {
            MarketParticipantPosition message = read_market_participant_position(&file, length - 1);
            mp_table.add_market_participant_position(message);
        }
        else if (message_type == MessageType::ADD_ORDER_MESSAGE)
        {
            AddOrderMessage message = read_add_order_message(&file, length - 1);
            order_book.add_order(message);
        }
        else if (message_type == MessageType::DELETE_CANCEL_MESSAGE)
        {
            DeleteCancelMessage message = read_delete_cancel_message(&file, length - 1);
            order_book.delete_cancel_order(message);
        }
        else if (message_type == MessageType::REPLACE_MESSAGE)
        {
            ReplaceOrderMessage message = read_replace_order_message(&file, length - 1);
            order_book.relpace_order(message);
        }
        else if (message_type == MessageType::ORDER_EXECUTED_MESSAGE)
        {
            OrderExecutedMessage message = read_order_executed_message(&file, length - 1);
            order_book.execute_order(message);
        }
        else if (message_type == MessageType::ORDER_EXECUTED_PRICE_MESSAGE)
        {
            OrderExecutedPriceMessage message = read_order_executed_price_message(&file, length - 1);
            order_book.execute_order_price(message);
        }
        else if (message_type == MessageType::TRADE_CROSS_MESSAGE)
        {
            TradeCrossMessage message = read_trade_cross_message(&file, length - 1);
            order_book.execute_cross_trade(message);
        }
        else if (message_type == MessageType::TRADE_NON_CROSS_MESSAGE)
        {
            TradeNonCrossMessage message = read_trade_non_cross_message(&file, length - 1);
            order_book.execute_non_cross_trade(message);
        }
        else if (message_type == MessageType::MWCB_DECLINE_MESSAGE || message_type == MessageType::IPO_QUOTING_PERIOD || message_type == MessageType::NOII_MESSAGE || message_type == MessageType::LULD_AUCTION_COLLAR)
        {
            dummy_read(&file, length - 1);
        }
        else
        {
            std::cout << "Parsed: " << std::dec << i << " messages" << std::endl;
            return 0;
        }
    }

    return 0;
}