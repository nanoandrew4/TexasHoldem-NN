#ifndef TEXAS_HOLDEM_NN_TABLE_H
#define TEXAS_HOLDEM_NN_TABLE_H

#include "Hand.h"
#include "Deck.h"
#include "Player.h"

class Table {
public:
    Table(Player** players, uint numOfPlayers);
    ~Table();

    void play();
private:
    void playRound(uint);

    Card* flop[5];

    Player** players;
    Player* dealer;
    Player* smallBlind;
    Player* bigBlind;

    uint numOfPlayers;
    uint activePlayers;

    static const uint BB = 2;
    static const uint SB = 1;

    uint pot = 0;
    uint lastRaise = 0;
    uint lastPlayerRaised = 0;
};


#endif //TEXAS_HOLDEM_NN_TABLE_H
