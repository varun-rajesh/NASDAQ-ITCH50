#ifndef MARKET_PARTICIPANTS_H
#define MARKET_PARTICIPANTS_H

#include <iostream>
#include <unordered_map>
#include <cassert>
#include "helper.h"
using namespace std;

struct MarketParticipantFlags
{
    bool primary_market_maker;
    char market_maker_mode;
    char market_participant_state;
};

class MarketParticipantTable
{
private:
    unordered_map<string, unordered_map<string, MarketParticipantFlags>> market_participants;

public:
    void print_market_participants() const
    {
        std::cout << "MPID,Stock,Primary Market Maker,Market Maker Mode,Market Participant State\n";

        for (const auto &mpid_pair : market_participants)
        {
            const std::string &mpid = mpid_pair.first;

            for (const auto &stock_pair : mpid_pair.second)
            {
                const std::string &stock = stock_pair.first;
                const MarketParticipantFlags &flags = stock_pair.second;

                std::cout << mpid << ','
                          << stock << ','
                          << (flags.primary_market_maker ? "Yes" : "No") << ','
                          << flags.market_maker_mode << ','
                          << flags.market_participant_state << std::endl;
            }
        }
    }
    void add_market_participant_position(const MarketParticipantPosition &message)
    {
        MarketParticipantFlags flags = {
            .primary_market_maker = message.primary_market_maker,
            .market_maker_mode = message.market_maker_mode,
            .market_participant_state = message.market_participant_state};

        market_participants[string(message.mpid, 4)][string(message.stock, 8)] = flags;
    }
};

#endif // MARKET_PARTICIPANTS_H