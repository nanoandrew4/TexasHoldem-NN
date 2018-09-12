#ifndef TEXAS_HOLDEM_NN_TABLE_H
#define TEXAS_HOLDEM_NN_TABLE_H

#include "Hand.h"
#include "Deck.h"
#include "Player.h"

class Table {
public:
    explicit Table(std::vector<Player *> &players);

    ~Table();

    /**
     * With the players currently seated around the table, plays enough rounds so that each player gets to be dealer
     * once.
     */
    void play();

    void playUntilOneLeft();

    // Controls whether game info and updates are printed to the console
    static bool output;

    int raises = 0;
    int rounds = 0;
    int folds = 0;
    int checks = 0;
private:

    /**
     * Plays once around the table, until all players call one another, and either another card is revealed or each
     * player shows his cards at the end of the game.
     *
     * @param startPlayer Player that makes the first betting decision
     * @param lastRaise Optional initial raise, used for setting the pre-flop call price
     */
    void playRound(unsigned long startPlayer, int lastRaise = 0);

    /**
     * Resets the table to allow for a new round to be played.
     *
     * @param cards Vector of Card* type, where the cards to be played are dealt, and assigned to the various players
     * @return True if a new round can be played, false if all but one players are out of money
     */
    bool newRound();

    /**
     * Sets the dealer, small blind and big blind roles to different players, depending on the round number.
     *
     * @param round Round number that is about to be played
     * @return Number of blinds in the game (1 if only small blind, 2 if small and big blind)
     */
    int assignSpecialRoles(unsigned long round);

    /**
     * Makes the initial antes for small blind and big blind, if they exist.
     *
     * @param bigBlindSet True if the big blind exists, false otherwise
     * @return Initial quantity required for players to play their hands in the pre-flop
     */
    void initAntes(bool bigBlindSet);

    /**
     * Splits pot amongst winning players, if applicable. Executed once all betting has finished, and if a tie happened.
     */
    void splitPot();

    /**
     * Writes information about the table the players are seated at to the array passed. This information is used
     * by human players to determine the cost to call a raise made, and by agents in order to determine the best next
     * move.
     *
     * @param tableInfo Array of size (4 + ActivePlayers - 1) where the table information is written to
     * @param currPlayer Player collecting information on the table
     */
    void getTableInfo(std::vector<double> &tableInfo, unsigned long currPlayer);

    // Face up cards on the table
    std::vector<Card *> communityCards;

    Deck deck;

    // Various entities that take special actions during the start of each round
    std::vector<Player *> players;
    Player *dealer;
    Player *smallBlind;
    Player *bigBlind;

    unsigned long numOfPlayers;
    unsigned long activePlayers;

    // Big blind, value that all players must ante up in order to play their hands
    static const int BB = 2;

    // Small blind, value the player assigned this role must ante up in order to play his hand (1/2 BB)
    static const int SB = 1;

    unsigned long pot = 0; // Money on the table
    unsigned long lastRaise = 0; // Last amount raised
    unsigned long lastPlayerRaised = 0; // Last player who raised
};

#endif //TEXAS_HOLDEM_NN_TABLE_H
