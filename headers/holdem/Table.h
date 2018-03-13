#ifndef TEXAS_HOLDEM_NN_TABLE_H
#define TEXAS_HOLDEM_NN_TABLE_H

#include "Hand.h"
#include "Deck.h"
#include "Player.h"

class Table {
public:
    Table(Player* players[], uint numOfPlayers);
    ~Table();

    /**
     * With the players currently seated around the table, plays enough rounds so that each player gets to be dealer
     * once.
     */
    void play();
private:

    /**
     * Plays once around the table, until all players call one another, and either another card is revealed or each
     * player shows his cards at the end of the game.
     *
     * @param startPlayer Player that makes the first betting decision
     */
    void playRound(uint startPlayer);

    /**
     * Writes information about the table the players are seated at to the array passed. This information is used
     * by human players to determine the cost to call a raise made, and by agents in order to determine the best next
     * move.
     *
     * @param tableInfo Array of size (4 + ActivePlayers - 1) where the table information is written to
     * @param currPlayer Player collecting information on the table
     */
    void getTableInfo(int tableInfo[], Player* currPlayer);

    // Face up cards on the table
    Card* communityCards[5];

    // Various entities that take special actions during the start of each round
    Player** players;
    Player* dealer;
    Player* smallBlind;
    Player* bigBlind;

    uint numOfPlayers;
    uint activePlayers;

    // Big blind, value that all players must ante up in order to play their hands
    static const uint BB = 2;

    // Small blind, value the player assigned this role must ante up in order to play his hand (1/2 BB)
    static const uint SB = 1;

    uint pot = 0; // Money on the table
    uint lastRaise = 0; // Last amount raised
    uint lastPlayerRaised = 0; // Last player who raised
};


#endif //TEXAS_HOLDEM_NN_TABLE_H
