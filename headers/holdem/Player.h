#ifndef TEXAS_HOLDEM_NN_PLAYER_H
#define TEXAS_HOLDEM_NN_PLAYER_H

#include <functional>
#include "Hand.h"
#include "../nn/NeuralNetwork.h"

class Player {
public:
    Player();
    ~Player();

    /**
     * Returns whether the player is playing or not, which indicates if he has folded at a previous turn.
     * @return True if player is still playing, false if he folded during the round
     */
    bool isPlaying() { return playing;};

    // Player's hand
    Hand* hand;

    /**
     * Removes an amount 'ante' from the players money.
     * @param ante Amount bet
     */
    void anteUp(uint ante);

    /**
     * Increases an amount 'winnings' from the players money.
     * @param winnings Amount won
     */
    void collectWinnings(uint winnings);

    /**
     * Returns amount of money the player disposes of
     * @return Amount of money the player disposes of
     */
    uint getMoney();

    /**
     * Prepares player for a new round, by setting the 'playing' boolean to true.
     */
    void newRound();

    /**
     * Code to run when a players turn comes around. Virtual allows overwriting by inheriting classes.
     *
     * @params tableInfo Information coming from the table the player is playing at, that might be relevant
     * @return Action taken by player (-1 -> fold, 0 -> call, x > 0 -> raise by 'x')
     */
    virtual int play(int tableInfo[]);

    uint money = 1000;
    bool playing = true;
};


#endif //TEXAS_HOLDEM_NN_PLAYER_H
