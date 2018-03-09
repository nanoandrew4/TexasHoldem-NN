#ifndef TEXAS_HOLDEM_NN_PLAYER_H
#define TEXAS_HOLDEM_NN_PLAYER_H

#include <functional>
#include "Hand.h"

class Player {
public:
    Player();
    ~Player();

    bool isPlaying() { return playing;};

    Hand* hand;

    virtual int playTurn(int costToCall) { return play(costToCall);};
    std::function<int(int)> play;

    void anteUp(int ante);
    uint getMoney();
    void newRound();

    void humanPlayer();
private:
    uint money = 1000;
    bool playing = true;
};


#endif //TEXAS_HOLDEM_NN_PLAYER_H
