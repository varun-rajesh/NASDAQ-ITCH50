#ifndef ORDER_BOOK_H
#define ORDER_BOOK_H

#include <iostream>
#include <unordered_map>
#include <iomanip>
#include <vector>
#include <cassert>
#include "helper.h"
using namespace std;

struct OrderBookEntry
{
    char side;
    uint16_t stock_locate;
    uint32_t price;
    uint32_t volume;
};

struct OrderBookByStockEntry
{
    char side;
    uint64_t order_reference_number;
    uint32_t price;
    uint32_t volume;
};

struct ExecutedOrder
{
    uint16_t stock_locate;
    uint32_t price;
    uint32_t volume;
    char cross_type;
};

class OrderBook
{
private:
    unordered_map<uint64_t, OrderBookEntry> order_book;
    unordered_map<uint64_t, ExecutedOrder> trade_book;

public:
    void add_order(const AddOrderMessage &message)
    {
        OrderBookEntry entry = {
            .side = message.buy_sell_indicator,
            .stock_locate = message.header.stock_locate,
            .price = message.price,
            .volume = message.shares};
        order_book[message.order_reference_number] = entry;
    };

    void delete_cancel_order(const DeleteCancelMessage &message)
    {
        assert(order_book.find(message.order_reference_number) != order_book.end());
        if (message.delete_cancel_indicator == 'D')
        {
            order_book.erase(message.order_reference_number);
        }
        else if (message.delete_cancel_indicator == 'C')
        {
            order_book[message.order_reference_number].volume -= message.cancelled_shares;
        }

        if (order_book[message.order_reference_number].volume == 0)
        {
            order_book.erase(message.order_reference_number);
        }

    }

    void relpace_order(const ReplaceOrderMessage &message)
    {
        assert(order_book.find(message.original_order_reference_number) != order_book.end());
        assert(order_book.find(message.new_order_reference_number) == order_book.end());
        OrderBookEntry new_entry = {
            .side = order_book[message.original_order_reference_number].side,
            .stock_locate = message.header.stock_locate,
            .price = message.price,
            .volume = message.shares};

        order_book[message.new_order_reference_number] = new_entry;
        order_book.erase(message.original_order_reference_number);
    }

    void execute_order(const OrderExecutedMessage &message)
    {
        assert(order_book.find(message.order_reference_number) != order_book.end());
        order_book[message.order_reference_number].volume -= message.executed_shares;

        if (trade_book.find(message.match_number) == trade_book.end())
        {
            ExecutedOrder order = {
                .stock_locate = message.header.stock_locate,
                .price = order_book[message.order_reference_number].price,
                .volume = message.executed_shares,
                .cross_type = ' '};

            trade_book[message.match_number] = order;
        }
        else
        {
            trade_book[message.match_number].volume += message.executed_shares;
        }

        if (order_book[message.order_reference_number].volume == 0)
        {
            order_book.erase(message.order_reference_number);
        }
    }

    void execute_order_price(const OrderExecutedPriceMessage &message)
    {
        if (message.printable)
        {
            assert(order_book.find(message.order_reference_number) != order_book.end());
            order_book[message.order_reference_number].volume -= message.executed_shares;

            if (trade_book.find(message.match_number) == trade_book.end())
            {
                ExecutedOrder order = {
                    .stock_locate = message.header.stock_locate,
                    .price = message.execution_price,
                    .volume = message.executed_shares,
                    .cross_type = ' '};

                trade_book[message.match_number] = order;
            }
            else
            {
                trade_book[message.match_number].volume += message.executed_shares;
            }

            if (order_book[message.order_reference_number].volume == 0)
            {
                order_book.erase(message.order_reference_number);
            }
        }
    }

    void execute_cross_trade(const TradeCrossMessage &message)
    {
        if (trade_book.find(message.match_number) == trade_book.end())
        {
            ExecutedOrder order = {
                .stock_locate = message.header.stock_locate,
                .price = message.cross_price,
                .volume = message.shares,
                .cross_type = message.cross_type};
            trade_book[message.match_number] = order;
        }
        else
        {
            assert(trade_book[message.match_number].price == message.cross_price);
            assert(trade_book[message.match_number].stock_locate == message.header.stock_locate);
            trade_book[message.match_number].volume += message.shares;
        }
    }

    void execute_non_cross_trade(const TradeNonCrossMessage &message)
    {
        if (trade_book.find(message.match_number) == trade_book.end())
        {
            ExecutedOrder order = {
                .stock_locate = message.header.stock_locate,
                .price = message.price,
                .volume = message.shares,
                .cross_type = ' '};
            trade_book[message.match_number] = order;
        }
        else
        {
            assert(trade_book[message.match_number].price == message.price);
            assert(trade_book[message.match_number].stock_locate == message.header.stock_locate);
            trade_book[message.match_number].volume += message.shares;
        }
    }

    void get_orders_by_stock_locate(uint16_t stock_locate, vector<OrderBookByStockEntry> &entries)
    {
        for (const auto [key, entry] : order_book)
        {
            if (entry.stock_locate == stock_locate)
            {
                OrderBookByStockEntry matching_entry = {
                    .side = entry.side,
                    .order_reference_number = key,
                    .price = entry.price,
                    .volume = entry.volume};

                entries.push_back(matching_entry);
            }
        }
    }

    void print_orders_by_stock_locate(uint16_t stock_locate)
    {
        std::vector<OrderBookByStockEntry> entries;
        get_orders_by_stock_locate(stock_locate, entries);

        std::cout << "Side,Order Ref. Number,Price,Volume" << std::endl;

        for (const auto &entry : entries)
        {
            std::cout << entry.side << ","
                      << entry.order_reference_number << ","
                      << entry.price << ","
                      << entry.volume << std::endl;
        }
    }

    void print_executions_by_stock_locate(uint16_t stock_locate)
    {
        for (const auto [key, entry] : trade_book)
        {
            if (entry.stock_locate == stock_locate) 
            {
                std::cout << std::dec << entry.price << std::endl;
            }
        }
    }

    void print_order_book()
    {
        std::cout << "Order Reference Number,Side,Stock Locate,Price,Volume" << std::endl;

        for (const auto &[order_ref_num, entry] : order_book)
        {
            std::cout << order_ref_num << ","
                      << entry.side << ","
                      << entry.stock_locate << ","
                      << entry.price << ","
                      << entry.volume << std::endl;
        }
    }
};

#endif // ORDER_BOOK_H
