#include <iostream>
#include <fstream>
#include <cstring>
#include <byteswap.h>
#include <cassert>
#include <algorithm>
#include <iomanip>
#include <chrono>
#include "helper.h"

std::ostream &operator<<(std::ostream &os, MessageType type)
{
    switch (type)
    {
    case MessageType::SYSTEM_EVENT_MESSAGE:
        os << "SYSTEM_EVENT_MESSAGE";
        break;
    case MessageType::STOCK_DIRECTORY_MESSAGE:
        os << "STOCK_DIRECTORY_MESSAGE";
        break;
    case MessageType::STOCK_TRADING_ACTION_MESSAGE:
        os << "STOCK_TRADING_ACTION_MESSAGE";
        break;
    case MessageType::REG_SHO_RESTRICTION:
        os << "REG_SHO_RESTRICTION";
        break;
    case MessageType::MARKET_PARTICIPANT_POSITION:
        os << "MARKET_PARTICIPANT_POSITION";
        break;
    case MessageType::ADD_ORDER_MESSAGE:
        os << "ADD_ORDER_MESSAGE";
        break;
    case MessageType::DELETE_CANCEL_MESSAGE:
        os << "DELETE_CANCEL_MESSAGE";
        break;
    case MessageType::REPLACE_MESSAGE:
        os << "REPLACE_MESSAGE";
        break;
    case MessageType::ORDER_EXECUTED_MESSAGE:
        os << "ORDER_EXECUTED_MESSAGE";
        break;
    case MessageType::ORDER_EXECUTED_PRICE_MESSAGE:
        os << "ORDER_EXECUTED_PRICE_MESSAGE";
        break;
    case MessageType::TRADE_NON_CROSS_MESSAGE:
        os << "TRADE_NON_CROSS_MESSAGE";
        break;
    case MessageType::TRADE_CROSS_MESSAGE:
        os << "TRADE_CROSS_MESSAGE";
        break;
    case MessageType::MWCB_DECLINE_MESSAGE:
        os << "MWCB_DECLINE_MESSAGE";
        break;
    case MessageType::IPO_QUOTING_PERIOD:
        os << "IPO_QUOTING_PERIOD";
        break;
    case MessageType::NOII_MESSAGE:
        os << "NOII_MESSAGE";
        break;
    case MessageType::LULD_AUCTION_COLLAR:
        os << "LULD_AUCTION_COLLAR";
        break;
    case MessageType::UNKNOWN_MESSAGE:
    default:
        os << "UNKNOWN_MESSAGE";
        break;
    }
    return os;
}

MessageType read_message_type(std::ifstream *fs)
{
    char message_type;
    if (fs->read(&message_type, 1))
    {
        switch (message_type)
        {
        case 'S':
            return MessageType::SYSTEM_EVENT_MESSAGE;
        case 'R':
            return MessageType::STOCK_DIRECTORY_MESSAGE;
        case 'H':
            return MessageType::STOCK_TRADING_ACTION_MESSAGE;
        case 'Y':
            return MessageType::REG_SHO_RESTRICTION;
        case 'L':
            return MessageType::MARKET_PARTICIPANT_POSITION;
        case 'A':
        case 'F':
            return MessageType::ADD_ORDER_MESSAGE;
        case 'X':
        case 'D':
            return MessageType::DELETE_CANCEL_MESSAGE;
        case 'U':
            return MessageType::REPLACE_MESSAGE;
        case 'E':
            return MessageType::ORDER_EXECUTED_MESSAGE;
        case 'C':
            return MessageType::ORDER_EXECUTED_PRICE_MESSAGE;
        case 'P':
            return MessageType::TRADE_NON_CROSS_MESSAGE;
        case 'Q':
            return MessageType::TRADE_CROSS_MESSAGE;
        case 'V':
            return MessageType::MWCB_DECLINE_MESSAGE;
        case 'K':
            return MessageType::IPO_QUOTING_PERIOD;
        case 'I':
            return MessageType::NOII_MESSAGE;
        case 'J':
            return MessageType::LULD_AUCTION_COLLAR;
        default:
            std::cout << "Received: " << message_type << std::endl;
            return MessageType::UNKNOWN_MESSAGE;
        }
    }
    return MessageType::UNKNOWN_MESSAGE;
}

uint16_t parse_uint16_t(const char *x_char)
{
    uint16_t x;
    memcpy(&x, x_char, 2);
    return __bswap_16(x);
}

uint32_t parse_uint32_t(const char *x_char)
{
    uint32_t x;
    memcpy(&x, x_char, 4);
    return __bswap_32(x);
}

uint64_t parse_uint64_t(const char *x_char)
{
    uint64_t x;
    memcpy(&x, x_char, 8);
    return __bswap_64(x);
}

uint64_t parse_timestamp(const char *timestamp_char)
{
    uint64_t timestamp;
    memcpy(&timestamp, timestamp_char, 6);
    timestamp = __bswap_64(timestamp);
    return timestamp >> 16;
}

void dummy_read(std::ifstream *fs, uint32_t length)
{
    char *message = (char *)calloc(length, sizeof(char));
    fs->read(message, length);
    free(message);
}

uint16_t read_length(std::ifstream *fs)
{
    char length_char[2];
    uint16_t length;

    if (fs->read(length_char, 2))
    {
        memcpy(&length, length_char, sizeof(length_char));
        return __bswap_16(length);
    }
    else
    {
        return 0;
    }
}

std::string format_timestamp(const uint64_t timestamp_ns)
{
    using namespace std::chrono;

    // Convert nanoseconds to seconds and nanoseconds
    nanoseconds ns(timestamp_ns);
    auto seconds_since_midnight = duration_cast<seconds>(ns);
    auto nanoseconds_part = ns - seconds_since_midnight;

    // Calculate hours, minutes, and seconds
    auto hours_duration = duration_cast<hours>(seconds_since_midnight);
    auto minutes_duration = duration_cast<minutes>(seconds_since_midnight % hours(1));
    auto seconds_duration = duration_cast<seconds>(seconds_since_midnight % minutes(1));

    // Calculate milliseconds, microseconds, and nanoseconds
    auto milliseconds_part = duration_cast<milliseconds>(nanoseconds_part);
    auto microseconds_part = duration_cast<microseconds>(nanoseconds_part);
    auto nanoseconds_part_final = nanoseconds_part - milliseconds_part;

    // Format the timestamp into a string
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << hours_duration.count() << ':'
        << std::setw(2) << std::setfill('0') << minutes_duration.count() << ':'
        << std::setw(2) << std::setfill('0') << seconds_duration.count() << '.'
        << std::setw(3) << std::setfill('0') << milliseconds_part.count() << '.'
        << std::setw(3) << std::setfill('0') << (microseconds_part.count() % 1000) << '.'
        << std::setw(3) << std::setfill('0') << (nanoseconds_part_final.count() % 1000);
    return oss.str();
}

Header parse_header(const char *message)
{
    Header header = {
        .stock_locate = parse_uint16_t(&message[0]),
        .tracking_number = parse_uint16_t(&message[2]),
        .timestamp = parse_timestamp(&message[4])};

    return header;
}

void print_header(const Header &header)
{
    std::cout << "  Stock Locate   : " << std::setw(4) << std::setfill(' ') << header.stock_locate << std::endl;
    std::cout << "  Tracking Number: " << std::setw(4) << std::setfill(' ') << header.tracking_number << std::endl;
    std::cout << "  Timestamp      : " << format_timestamp(header.timestamp) << std::endl;
}

void print_system_event_message(SystemEventMessage &msg)
{
    std::cout << "SystemEventMessage:" << std::endl;
    print_header(msg.header);
    std::cout << "  System Event   : '" << msg.system_event << "'" << std::endl;
}

SystemEventMessage read_system_event_message(std::ifstream *fs, uint16_t length)
{
    assert(length == 11);

    char *message = (char *)calloc(length, sizeof(char));
    SystemEventMessage parsed_message;

    if (fs->read(message, length))
    {
        parsed_message.header = parse_header(&message[0]);
        parsed_message.system_event = message[10];
    }

    free(message);
    return parsed_message;
}

void print_stock_directory_message(const StockDirectoryMessage &msg)
{
    std::cout << "Stock Directory Message:" << std::endl;
    print_header(msg.header);
    std::cout << "  Stock ID: " << std::string(msg.stock, 8) << std::endl;
    std::cout << "  Market Category: " << msg.market_category << std::endl;
    std::cout << "  Financial Status Indicator: " << msg.financial_status_indicator << std::endl;
    std::cout << "  Round Lot Size: " << std::dec << msg.round_lot_size << std::endl;
    std::cout << "  Round Lots Only: " << (msg.round_lots_only ? "Yes" : "No") << std::endl;
    std::cout << "  Issue Classification: " << msg.issue_classification << std::endl;
    std::cout << "  Issue Sub-Type: " << msg.issue_sub_type[0] << msg.issue_sub_type[1] << std::endl;
    std::cout << "  Authenticity: " << msg.authenticity << std::endl;
    std::cout << "  Short Sale Threshold: " << msg.short_sale_threshold << std::endl;
    std::cout << "  IPO Flag: " << msg.ipo_flag << std::endl;
    std::cout << "  LULD Reference Price Tier: " << msg.luld_reference_price_tier << std::endl;
    std::cout << "  ETP Flag: " << msg.etp_flag << std::endl;
    std::cout << "  ETP Leverage Factor: " << std::dec << msg.etp_leverage_factor << std::endl;
    std::cout << "  Inverse Indicator: " << (msg.inverse_indicator ? "Yes" : "No") << std::endl;
}

StockDirectoryMessage read_stock_directory_message(std::ifstream *fs, uint16_t length)
{
    assert(length == 38);

    char *message = (char *)calloc(length, sizeof(char));
    StockDirectoryMessage parsed_message;

    if (fs->read(message, length))
    {
        parsed_message.header = parse_header(&message[0]);
        memcpy(&parsed_message.stock, &message[10], 8);
        parsed_message.market_category = message[18];
        parsed_message.financial_status_indicator = message[19];
        parsed_message.round_lot_size = parse_uint32_t(&message[20]);
        parsed_message.round_lots_only = (message[24] == 'Y');
        parsed_message.issue_classification = message[25];
        memcpy(&parsed_message.issue_sub_type, &message[26], 2);
        parsed_message.authenticity = message[28];
        parsed_message.short_sale_threshold = message[29];
        parsed_message.ipo_flag = message[30];
        parsed_message.luld_reference_price_tier = message[31];
        parsed_message.etp_flag = message[32];
        parsed_message.etp_leverage_factor = parse_uint32_t(&message[33]);
        parsed_message.inverse_indicator = (message[37] == 'Y');
    }

    free(message);
    return parsed_message;
}

void print_stock_trading_action_message(const StockTradingActionMessage &msg)
{
    std::cout << "Stock Trading Action Message:" << std::endl;
    print_header(msg.header);
    std::cout << "  Stock: " << std::string(msg.stock, 8) << std::endl;
    std::cout << "  Trading State: " << msg.trading_state << std::endl;
    std::cout << "  Reason: " << std::string(msg.reason, 4) << std::endl;
}

StockTradingActionMessage read_stock_trading_action_message(std::ifstream *fs, uint16_t length)
{
    assert(length == 24);

    char *message = (char *)calloc(length, sizeof(char));
    StockTradingActionMessage parsed_message;

    if (fs->read(message, length))
    {
        parsed_message.header = parse_header(&message[0]);
        memcpy(&parsed_message.stock, &message[10], 8);
        parsed_message.trading_state = message[18];
        memcpy(&parsed_message.reason, &message[20], 4);
    }

    free(message);
    return parsed_message;
}

void print_reg_sho_restriction(const RegSHORestriction &msg)
{
    std::cout << "Reg SHO Restriction Message:" << std::endl;
    print_header(msg.header);
    std::cout << "  Stock: " << std::string(msg.stock, 8) << std::endl;
    std::cout << "  Reg SHO Action: " << msg.reg_sho_action << std::endl;
}

RegSHORestriction read_reg_sho_restriction(std::ifstream *fs, uint16_t length)
{
    assert(length == 19);

    char *message = (char *)calloc(length, sizeof(char));
    RegSHORestriction parsed_message;

    if (fs->read(message, length))
    {
        parsed_message.header = parse_header(&message[0]);
        memcpy(&parsed_message.stock, &message[10], 8);
        parsed_message.reg_sho_action = message[18];
    }

    free(message);
    return parsed_message;
}

void print_market_participant_position(const MarketParticipantPosition &msg)
{
    std::cout << "Reg SHO Restriction Message:" << std::endl;
    print_header(msg.header);
    std::cout << "  MPID: " << std::dec << std::string(msg.mpid, 4) << std::endl;
    std::cout << "  Stock: " << std::string(msg.stock, 8) << std::endl;
    std::cout << "  Primary Market Maker: " << (msg.primary_market_maker ? "Yes" : "No") << std::endl;
    std::cout << "  Market Maker Mode: " << msg.market_maker_mode << std::endl;
    std::cout << "  Market Participant State: " << msg.market_participant_state << std::endl;
}

MarketParticipantPosition read_market_participant_position(std::ifstream *fs, uint16_t length)
{
    assert(length == 25);
    char *message = (char *)calloc(length, sizeof(char));
    MarketParticipantPosition parsed_message;

    if (fs->read(message, length))
    {
        parsed_message.header = parse_header(&message[0]);
        memcpy(&parsed_message.mpid, &message[10], 4);
        memcpy(&parsed_message.stock, &message[14], 8);
        parsed_message.primary_market_maker = (message[22] == 'Y');
        parsed_message.market_maker_mode = message[23];
        parsed_message.market_participant_state = message[24];
    }

    free(message);
    return parsed_message;
}

void print_add_order_message(const AddOrderMessage &msg)
{
    std::cout << "Add Order Message:" << std::endl;
    print_header(msg.header);
    std::cout << "  Order Reference Number: " << std::dec << msg.order_reference_number << std::endl;
    std::cout << "  Buy/Sell Indicator: " << msg.buy_sell_indicator << std::endl;
    std::cout << "  Shares: " << std::dec << msg.shares << std::endl;
    std::cout << "  Stock: " << std::string(msg.stock, 8) << std::endl;
    std::cout << "  Price: " << std::dec << msg.price << std::endl;
    std::cout << "  Attribution: " << std::string(msg.attribution, 4) << std::endl;
}

AddOrderMessage read_add_order_message(std::ifstream *fs, uint16_t length)
{
    assert(length == 35 || length == 39);
    char *message = (char *)calloc(length, sizeof(char));
    AddOrderMessage parsed_message;

    if (fs->read(message, length))
    {
        parsed_message.header = parse_header(&message[0]);
        parsed_message.order_reference_number = parse_uint64_t(&message[10]);
        parsed_message.buy_sell_indicator = message[18];
        parsed_message.shares = parse_uint32_t(&message[19]);
        memcpy(&parsed_message.stock, &message[23], 8);
        parsed_message.price = parse_uint32_t(&message[31]);
        if (length == 39)
        {
            memcpy(&parsed_message.attribution, &message[35], 4);
        }
        else
        {
            memcpy(&parsed_message.attribution, &"NSDQ", 4);
        }
    }

    free(message);
    return parsed_message;
}

void print_delete_cancel_message(const DeleteCancelMessage &msg)
{
    std::cout << "Delete/Cancel Message:" << std::endl;
    print_header(msg.header);
    std::cout << "  Order Reference Number: " << std::dec << msg.order_reference_number << std::endl;
    std::cout << "  Delete/Cancel Indicator: " << msg.delete_cancel_indicator << std::endl;
    std::cout << "  Cancelled Shares: " << std::dec << msg.cancelled_shares << std::endl;
}

DeleteCancelMessage read_delete_cancel_message(std::ifstream *fs, uint16_t length)
{
    assert(length == 22 || length == 18);
    char *message = (char *)calloc(length, sizeof(char));
    DeleteCancelMessage parsed_message;

    if (fs->read(message, length))
    {
        parsed_message.header = parse_header(&message[0]);
        parsed_message.order_reference_number = parse_uint64_t(&message[10]);
        parsed_message.delete_cancel_indicator = (length == 22) ? 'C' : 'D';
        parsed_message.cancelled_shares = (length == 22) ? parse_uint32_t(&message[18]) : 0;
    }

    free(message);
    return parsed_message;
}

void print_replace_order_message(const ReplaceOrderMessage &msg)
{
    std::cout << "Replace Order Message:" << std::endl;
    print_header(msg.header);
    std::cout << "  Original Order Reference Number: " << std::dec << msg.original_order_reference_number << std::endl;
    std::cout << "  New Order Reference Number: " << std::dec << msg.new_order_reference_number << std::endl;
    std::cout << "  Shares: " << std::dec << msg.shares << std::endl;
    std::cout << "  Price: " << std::dec << msg.price << std::endl;
}

ReplaceOrderMessage read_replace_order_message(std::ifstream *fs, uint16_t length)
{
    assert(length == 34);
    char *message = (char *)calloc(length, sizeof(char));
    ReplaceOrderMessage parsed_message;

    if (fs->read(message, length))
    {
        parsed_message.header = parse_header(&message[0]);
        parsed_message.original_order_reference_number = parse_uint64_t(&message[10]);
        parsed_message.new_order_reference_number = parse_uint64_t(&message[18]);
        parsed_message.shares = parse_uint32_t(&message[26]);
        parsed_message.price = parse_uint32_t(&message[30]);
    }

    free(message);
    return parsed_message;
}

void print_order_executed_message(const OrderExecutedMessage &msg)
{
    std::cout << "Order Executed Message:" << std::endl;
    print_header(msg.header);
    std::cout << "  Order Reference Number: " << std::dec << msg.order_reference_number << std::endl;
    std::cout << "  Executed Shares: " << std::dec << msg.executed_shares << std::endl;
    std::cout << "  Match Number: " << std::dec << msg.match_number << std::endl;
}

OrderExecutedMessage read_order_executed_message(std::ifstream *fs, uint16_t length)
{
    assert(length == 30);
    char *message = (char *)calloc(length, sizeof(char));
    OrderExecutedMessage parsed_message;

    if (fs->read(message, length))
    {
        parsed_message.header = parse_header(&message[0]);
        parsed_message.order_reference_number = parse_uint64_t(&message[10]);
        parsed_message.executed_shares = parse_uint32_t(&message[18]);
        parsed_message.match_number = parse_uint64_t(&message[22]);
    }

    free(message);
    return parsed_message;
}

void print_order_executed_price_message(const OrderExecutedPriceMessage &msg)
{
    std::cout << "Order Executed Price Message:" << std::endl;
    print_header(msg.header);
    std::cout << "  Order Reference Number: " << std::dec << msg.order_reference_number << std::endl;
    std::cout << "  Executed Shares: " << std::dec << msg.executed_shares << std::endl;
    std::cout << "  Match Number: " << std::dec << msg.match_number << std::endl;
    std::cout << "  Printable: " << msg.printable << std::endl;
    std::cout << "  Execution Price: " << std::dec << msg.execution_price << std::endl;
}

OrderExecutedPriceMessage read_order_executed_price_message(std::ifstream *fs, uint16_t length)
{
    assert(length == 35);
    char *message = (char *)calloc(length, sizeof(char));
    OrderExecutedPriceMessage parsed_message;

    if (fs->read(message, length))
    {
        parsed_message.header = parse_header(&message[0]);
        parsed_message.order_reference_number = parse_uint64_t(&message[10]);
        parsed_message.executed_shares = parse_uint32_t(&message[18]);
        parsed_message.match_number = parse_uint64_t(&message[22]);
        parsed_message.printable = (message[30] == 'Y');
        parsed_message.execution_price = parse_uint32_t(&message[31]);
    }

    free(message);
    return parsed_message;
}

void print_trade_non_cross_message(const TradeNonCrossMessage &msg)
{
    std::cout << "Trade Non-Cross Message:" << std::endl;
    print_header(msg.header);
    std::cout << "  Shares: " << std::dec << msg.shares << std::endl;
    std::cout << "  Stock: " << std::string(msg.stock, sizeof(msg.stock)) << std::endl;
    std::cout << "  Price: " << std::dec << msg.price << std::endl;
    std::cout << "  Match Number: " << std::dec << msg.match_number << std::endl;
}

TradeNonCrossMessage read_trade_non_cross_message(std::ifstream *fs, uint16_t length)
{
    assert(length == 43);
    char *message = (char *)calloc(length, sizeof(char));
    TradeNonCrossMessage parsed_message;

    if (fs->read(message, length))
    {
        parsed_message.header = parse_header(&message[0]);
        parsed_message.shares = parse_uint32_t(&message[19]);
        memcpy(&parsed_message.stock, &message[23], 8);
        parsed_message.price = parse_uint32_t(&message[31]);
        parsed_message.match_number = parse_uint64_t(&message[35]);
    }

    free(message);
    return parsed_message;
}

void print_trade_non_cross_message(const TradeCrossMessage &msg)
{
    std::cout << "Trade Non-Cross Message:" << std::endl;
    print_header(msg.header);
    std::cout << "  Shares: " << std::dec << msg.shares << std::endl;
    std::cout << "  Stock: " << std::string(msg.stock, sizeof(msg.stock)) << std::endl;
    std::cout << "  Cross Price: " << std::dec << msg.cross_price << std::endl;
    std::cout << "  Match Number: " << std::dec << msg.match_number << std::endl;
    std::cout << "  Cross Type: " << std::dec << msg.cross_type << std::endl;
}

TradeCrossMessage read_trade_cross_message(std::ifstream *fs, uint16_t length)
{
    assert(length == 39);
    char *message = (char *)calloc(length, sizeof(char));
    TradeCrossMessage parsed_message;

    if (fs->read(message, length))
    {
        parsed_message.header = parse_header(&message[0]);
        parsed_message.shares = parse_uint32_t(&message[10]);
        memcpy(&parsed_message.stock, &message[18], 8);
        parsed_message.cross_price = parse_uint32_t(&message[26]);
        parsed_message.match_number = parse_uint64_t(&message[30]);
        parsed_message.cross_type = message[38];
    }

    free(message);
    return parsed_message;
}