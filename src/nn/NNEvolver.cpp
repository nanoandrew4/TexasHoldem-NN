#include <iostream>
#include "../../headers/nn/NNEvolver.h"
#include "../../headers/nn/AIPlayer.h"
#include "../../headers/holdem/Table.h"

NNEvolver::NNEvolver() {

    int playersPerTable = 2;
    for (int p = 10; p > 2; p--)
        if (population % p == 0)
            playersPerTable = p;

    Player* players[population];

    for (int p = 0; p < population; p++) {
        players[p] = new AIPlayer();
    }

    std::cout << "Training" << std::endl;
    train(players, playersPerTable, 0);

    for (int t = 0; t < population; t++) {
        delete players[t];
    }
    delete players;
}

NNEvolver::~NNEvolver() {

}

void NNEvolver::train(Player* players[], int playersPerTable, int threadNum) {
    for (int t = 0; t < population / playersPerTable; t++) {
        Player* tablePlayers[playersPerTable]
        for (int p = 0; p < playersPerTable; p++)
            tablePlayers[p] = players[t * playersPerTable + p];
//        std::cout << "Playing table: " << t << std::endl;
//        std::cout << tablePlayers[t][0]->playing << std::endl;
        Table table(tablePlayers, playersPerTable);
        table.play();
    }

    // Sort by money (TODO: CHANGE METRIC)
    quicksort(players, 0, population - 1);

    // Combine top 10? players
}

void NNEvolver::quicksort(Player* players[], int lPiv, int rPiv) {
    int pivot = players[lPiv + (rPiv - lPiv) / 2]->getMoney();
    int a = lPiv, b = rPiv;

    while (a <= b) {
        while (players[a]->getMoney() > pivot)
            a++;
        while (players[b]->getMoney() < pivot)
            b--;
        if (a <= b) {
            Player* p = players[a];
            players[a] = players[b];
            players[b] = p;
            a++;
            b--;
        }
    }

    if (lPiv < b)
        quicksort(players, lPiv, b);
    if (a < rPiv)
        quicksort(players, a, rPiv);
}