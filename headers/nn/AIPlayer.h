#ifndef TEXAS_HOLDEM_NN_AIPLAYER_H
#define TEXAS_HOLDEM_NN_AIPLAYER_H

#include "NeuralNetwork.h"
#include "../holdem/Player.h"

class AIPlayer : public Player {
public:
    AIPlayer();
    ~AIPlayer();

    /**
     * Implementation of 'play' function in 'Player' class, designed for smart agents that use a neural network to
     * determine their next action, based on the information they have about the state of the table and their cards.
     *
     * @param tableInfo Information to be used as inputs for the neural network
     * @return Action taken by agent (-1 -> fold, 0 -> call, x > 0 -> raise by 'x')
     */
    int play(std::vector<double> tableInfo);

    void setNN(NeuralNetwork* nn) {delete this->nn; this->nn = nn;}
    NeuralNetwork* getNN() { return nn;}
private:
    double getHandPotential(std::vector<Card*> communityCards);
    NeuralNetwork* nn;

    static std::vector<int> neuronsPerLayer;
};


#endif //TEXAS_HOLDEM_NN_AIPLAYER_H
