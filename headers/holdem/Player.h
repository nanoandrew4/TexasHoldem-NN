#ifndef TEXAS_HOLDEM_NN_PLAYER_H
#define TEXAS_HOLDEM_NN_PLAYER_H

#include <functional>
#include "Hand.h"
#include "../nn/NeuralNetwork.h"

class Player {
public:
    Player();
    ~Player();

    bool isPlaying() { return playing;};

    Hand* hand;

    void anteUp(int ante);
    void collectWinnings(uint winnings);
    uint getMoney();
    void newRound();

    virtual int play(int[]);

    uint money = 1000;
    bool playing = true;
private:
};


#endif //TEXAS_HOLDEM_NN_PLAYER_H
