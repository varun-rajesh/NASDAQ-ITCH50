#ifndef HELPER_H
#define HELPER_H

#include <iostream>
#include <fstream>
#include <cstring>

enum class MessageType
{
    SYSTEM_EVENT_MESSAGE,
    STOCK_DIRECTORY_MESSAGE,
    STOCK_TRADING_ACTION_MESSAGE,
    REG_SHO_RESTRICTION,
    MARKET_PARTICIPANT_POSITION,
    ADD_ORDER_MESSAGE,
    DELETE_CANCEL_MESSAGE,
    REPLACE_MESSAGE,
    ORDER_EXECUTED_MESSAGE,
    ORDER_EXECUTED_PRICE_MESSAGE,
    TRADE_NON_CROSS_MESSAGE,
    TRADE_CROSS_MESSAGE,
    MWCB_DECLINE_MESSAGE,
    IPO_QUOTING_PERIOD,
    NOII_MESSAGE,
    LULD_AUCTION_COLLAR,
    UNKNOWN_MESSAGE
};

struct Header
{
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint64_t timestamp;
};

struct SystemEventMessage
{
    Header header;
    char system_event;
};

struct StockDirectoryMessage
{
    Header header;
    char stock[8];
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
};

struct StockTradingActionMessage
{
    Header header;
    char stock[8];
    char trading_state;
    char reason[4];
};

struct RegSHORestriction
{
    Header header;
    char stock[8];
    char reg_sho_action;
};

struct MarketParticipantPosition
{
    Header header;
    char mpid[4];
    char stock[8];
    bool primary_market_maker;
    char market_maker_mode;
    char market_participant_state;
};

struct AddOrderMessage
{
    Header header;
    uint64_t order_reference_number;
    char buy_sell_indicator;
    uint32_t shares;
    char stock[8];
    uint32_t price;
    char attribution[4];
};

struct DeleteCancelMessage
{
    Header header;
    uint64_t order_reference_number;
    char delete_cancel_indicator;
    uint32_t cancelled_shares;
};

struct ReplaceOrderMessage
{
    Header header;
    uint64_t original_order_reference_number;
    uint64_t new_order_reference_number;
    uint32_t shares;
    uint32_t price;
};

struct OrderExecutedMessage
{
    Header header;
    uint64_t order_reference_number;
    uint32_t executed_shares;
    uint64_t match_number;
};

struct OrderExecutedPriceMessage
{
    Header header;
    uint64_t order_reference_number;
    uint32_t executed_shares;
    uint64_t match_number;
    bool printable;
    uint32_t execution_price;
};

struct TradeNonCrossMessage
{
    Header header;
    uint32_t shares;
    char stock[8];
    uint32_t price;
    uint64_t match_number;
};

struct TradeCrossMessage
{
    Header header;
    uint32_t shares;
    char stock[8];
    uint32_t cross_price;
    uint64_t match_number;
    char cross_type;
};

std::ostream &operator<<(std::ostream &os, MessageType type);

uint16_t read_length(std::ifstream *fs);

void dummy_read(std::ifstream *fs, uint32_t length);

MessageType read_message_type(std::ifstream *fs);

SystemEventMessage read_system_event_message(std::ifstream *fs, uint16_t length);

StockDirectoryMessage read_stock_directory_message(std::ifstream *fs, uint16_t length);

StockTradingActionMessage read_stock_trading_action_message(std::ifstream *fs, uint16_t length);

RegSHORestriction read_reg_sho_restriction(std::ifstream *fs, uint16_t length);

MarketParticipantPosition read_market_participant_position(std::ifstream *fs, uint16_t length);

AddOrderMessage read_add_order_message(std::ifstream *fs, uint16_t length);

DeleteCancelMessage read_delete_cancel_message(std::ifstream *fs, uint16_t length);

ReplaceOrderMessage read_replace_order_message(std::ifstream *fs, uint16_t length);

OrderExecutedMessage read_order_executed_message(std::ifstream *fs, uint16_t length);

OrderExecutedPriceMessage read_order_executed_price_message(std::ifstream *fs, uint16_t length);

TradeNonCrossMessage read_trade_non_cross_message(std::ifstream *fs, uint16_t length);

TradeCrossMessage read_trade_cross_message(std::ifstream *fs, uint16_t length);

#endif // HELPER_H