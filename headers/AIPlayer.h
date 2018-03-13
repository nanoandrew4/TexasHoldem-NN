#ifndef TEXAS_HOLDEM_NN_AIPLAYER_H
#define TEXAS_HOLDEM_NN_AIPLAYER_H

#include "nn/NeuralNetwork.h"
#include "holdem/Player.h"

class AIPlayer : public Player {
public:
    AIPlayer();
    ~AIPlayer();
    int play(int[]);

private:
    NeuralNetwork* nn;
    int handPotential();
};


#endif //TEXAS_HOLDEM_NN_AIPLAYER_H
