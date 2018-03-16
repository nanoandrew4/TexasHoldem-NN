#include <iostream>
#include "../../headers/nn/AIPlayer.h"

int AIPlayer::neuronsPerLayer[] = {5, 25, 3};

AIPlayer::AIPlayer() {
    nn = new NeuralNetwork(layers, neuronsPerLayer, true);
}

AIPlayer::~AIPlayer() {
    delete nn;
}

int AIPlayer::play(int tableInfo[]) {
//    std::cout << "Playing" << std::endl;
    double input[tableInfo[2] + 3];
    input[0] = tableInfo[0]; // Cost to call
    input[1] = tableInfo[1]; // Chips in pot
    input[2] = tableInfo[2]; // Number or opponents
    input[3] = handPotential();

    for (int i = 0; i < tableInfo[2]; i++)
        input[i + 4] = tableInfo[i + 4];

    return nn->getAction(input);
}

int AIPlayer::handPotential() {

}