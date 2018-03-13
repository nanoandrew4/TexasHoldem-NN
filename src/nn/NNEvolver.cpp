#include <iostream>
#include "../../headers/nn/NNEvolver.h"
#include "../../headers/nn/AIPlayer.h"
#include "../../headers/holdem/Table.h"

NNEvolver::NNEvolver() {
    std::vector<Player*> players;

    for (int p = 0; p < population; p++)
            players.push_back(new AIPlayer());

    int playersPerTable = 2;
    for (int p = 10; p > 2; p--)
        if (population % p == 0)
            playersPerTable = p;

    Player*** tablePlayers;
    tablePlayers = new Player**[population / playersPerTable];

    for (int t = 0; t < population / playersPerTable; t++) {
        tablePlayers[t] = new Player*[playersPerTable];
        for (int p = 0; p < playersPerTable; p++)
            tablePlayers[t][p] = players.at((t * playersPerTable) + p);
    }

    std::cout << "Training" << std::endl;

    train(tablePlayers, playersPerTable, 0);

    for (int t = 0; t < population / playersPerTable; t++) {
        for (int p = 0; p < playersPerTable; p++)
            delete tablePlayers[t][p];
        delete tablePlayers[t];
    }
    delete tablePlayers;

}

NNEvolver::~NNEvolver() {

}

void NNEvolver::train(Player*** tablePlayers, int playersPerTable, int threadNum) {
    for (int t = 0; t < population / playersPerTable; t++) {
        std::cout << "Playing table: " << t << std::endl;
        std::cout << tablePlayers[t][0]->playing << std::endl;
        Table table(tablePlayers[t], playersPerTable);
        table.play();
    }

    // Actual learning...
}