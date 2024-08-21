#ifndef INSTRUMENT_TABLE_H
#define INSTRUMENT_TABLE_H

#include <iostream>
#include <unordered_map>
#include <cassert>
#include "helper.h"
using namespace std;

struct InstrumentTableEntry
{
    string stock;
    char market_category;
    char financial_status_indicator;
    uint32_t round_lot_size;
    bool round_lots_only;
    char issue_classification;
    char issue_sub_type[2];
    char authenticity;
    char short_sale_threshold;
    char ipo_flag;
    char luld_reference_price_tier;
    char etp_flag;
    uint32_t etp_leverage_factor;
    bool inverse_indicator;
    char trading_state;
    char reason[4];
    char reg_sho_action;
};

class InstrumentTable
{

public:
    void add_to_instrument_table(const StockDirectoryMessage &message)
    {
        if (stock_locate_to_stock_map.find(message.header.stock_locate) != stock_locate_to_stock_map.end())
        {
            std::cout << "Duplicate stock locate: " << std::dec << message.header.stock_locate << "for: " << message.stock << std::endl;
        }
        else
        {
            string stock = string(message.stock, 8);
            stock_locate_to_stock_map[message.header.stock_locate] = stock;
            stock_to_stock_locate_map[stock] = message.header.stock_locate;

            InstrumentTableEntry entry;
            entry.stock = stock;
            entry.market_category = message.market_category;
            entry.financial_status_indicator = message.financial_status_indicator;
            entry.round_lot_size = message.round_lot_size;
            entry.round_lots_only = message.round_lots_only;
            entry.issue_classification = message.issue_classification;
            memcpy(&entry.issue_sub_type, &message.issue_sub_type, sizeof(entry.issue_sub_type));
            entry.authenticity = message.authenticity;
            entry.short_sale_threshold = message.short_sale_threshold;
            entry.ipo_flag = message.ipo_flag;
            entry.luld_reference_price_tier = message.luld_reference_price_tier;
            entry.etp_flag = message.etp_flag;
            entry.etp_leverage_factor = message.etp_leverage_factor;
            entry.inverse_indicator = message.inverse_indicator;
            instrument_table[message.header.stock_locate] = entry;
        }
    }

    void add_stock_trading_action_message(const StockTradingActionMessage &message)
    {
        assert(stock_locate_to_stock_map.find(message.header.stock_locate) != stock_locate_to_stock_map.end());
        assert(stock_locate_to_stock_map[message.header.stock_locate] == string(message.stock, 8));
        instrument_table[message.header.stock_locate].trading_state = message.trading_state;
        memcpy(&instrument_table[message.header.stock_locate].reason, &message.reason, 4);
    }

    void add_reg_sho_restriction(const RegSHORestriction &message)
    {
        assert(stock_locate_to_stock_map.find(message.header.stock_locate) != stock_locate_to_stock_map.end());
        assert(stock_locate_to_stock_map[message.header.stock_locate] == string(message.stock, 8));
        instrument_table[message.header.stock_locate].reg_sho_action = message.reg_sho_action;
    }

    void print_instrument_table()
    {
        for (const auto pair : stock_locate_to_stock_map)
        {
            std::cout << "Stock Locate: " << pair.first
                      << " | Stock: " << pair.second << std::endl;
        }

        for (const auto [key, entry] : instrument_table)
        {
            std::cout << key << ",";
            print_instrument_table_entry(entry);
        }
    }

    uint16_t get_stock_locate_from_stock(const string &stock)
    {
        return stock_to_stock_locate_map[stock];
    }

    string get_stock_from_stock_locate(const uint16_t stock_locate)
    {
        return stock_locate_to_stock_map[stock_locate];
    }

private:
    unordered_map<uint16_t, string> stock_locate_to_stock_map;
    unordered_map<string, uint16_t> stock_to_stock_locate_map;
    unordered_map<uint16_t, InstrumentTableEntry> instrument_table;

    void print_instrument_table_entry(const InstrumentTableEntry &entry)
    {
        std::cout << '"' << entry.stock << "\","
                  << '"' << entry.market_category << "\","
                  << '"' << entry.financial_status_indicator << "\","
                  << entry.round_lot_size << ","
                  << (entry.round_lots_only ? "true" : "false") << ","
                  << '"' << entry.issue_classification << "\","
                  << '"' << entry.issue_sub_type[0] << entry.issue_sub_type[1] << "\","
                  << '"' << entry.authenticity << "\","
                  << '"' << entry.short_sale_threshold << "\","
                  << '"' << entry.ipo_flag << "\","
                  << '"' << entry.luld_reference_price_tier << "\","
                  << '"' << entry.etp_flag << "\","
                  << entry.etp_leverage_factor << ","
                  << (entry.inverse_indicator ? "true" : "false") << "\n";
    }

    void print_instrument_table_header()
    {
        std::cout << "Stock,Market Category,Financial Status Indicator,Round Lot Size,Round Lots Only,"
                  << "Issue Classification,Issue Sub Type,Authenticity,Short Sale Threshold,IPO Flag,"
                  << "LULD Reference Price Tier,ETP Flag,ETP Leverage Factor,Inverse Indicator\n";
    }
};

#endif // INSTRUMENT_TABLE_H