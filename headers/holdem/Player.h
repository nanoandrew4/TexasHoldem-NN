#ifndef TEXAS_HOLDEM_NN_PLAYER_H
#define TEXAS_HOLDEM_NN_PLAYER_H

#include "Hand.h"

class Player {
    friend class AIPlayer;

public:
    /**
     * Default constructor, for use by AIPlayer class when training agents. For human players, or agents to be played
     * against by humans, consider using the Player(std::string) constructor.
     */
    Player();

    /**
     * Initializes a player with a name for better identification when a game is being played on the console.
     *
     * @param name Name to assign to this player
     */
    explicit Player(std::string name);

    virtual ~Player();

    /**
     * Returns whether the player is playing or not, which indicates if he has folded at a previous turn in the current
     * round.
     *
     * @return True if player is still playing, false if he folded during the round
     */
    bool isPlaying() { return playing; };

    // Player's hand
    Hand *hand;

    /**
     * Removes an amount, when a bet is made.
     *
     * @param ante Amount bet
     */
    void anteUp(int ante);

    /**
     * Increases the players money, when a round is won.
     *
     * @param winnings Amount won
     */
    void collectWinnings(int winnings);

    int getMoney();

    std::string getName() { return name; }

    /**
     * Prepares player for a new round, by setting the 'playing' boolean to true.
     */
    void newRound();

    /**
     * For resetting an agents money after a generation of training only. Sets this players money to the
     * initial hardcoded amount.
     */
    void resetMoney() { money = initMoney; }

    /**
     * Code to run when a players turn comes around. Virtual allows overwriting by inheriting classes.
     *
     * @params tableInfo Information coming from the table the player is playing at, that might be relevant
     * @return Action taken by player (-1 -> fold, 0 -> call, x > 0 -> raise by 'x')
     */
    virtual int play(std::vector<double> tableInfo);

    /**
     * For use in AIPlayer. Does nothing in Player implementation.
     */
    virtual double getHandPotential(std::vector<Card *> communityCards) {};

private:
    static const int initMoney = 10000;
    int money = 10000;
    bool playing = true;

    std::string name;
};

#endif //TEXAS_HOLDEM_NN_PLAYER_H
