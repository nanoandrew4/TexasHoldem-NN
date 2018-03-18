#include <iostream>
#include "../../headers/nn/NNEvolver.h"
#include "../../headers/holdem/Table.h"

static std::mt19937_64 mt_rand((ulong) std::chrono::high_resolution_clock::now().time_since_epoch().count());

NNEvolver::NNEvolver() {
    int playersPerTable = 2;
    for (int p = 10; p > 2; p--)
        if (population % p == 0) {
            playersPerTable = p;
            break;
        }

    AIPlayer* players[population];

    for (int p = 0; p < population; p++)
        players[p] = new AIPlayer();

    std::cout << "Training" << std::endl;
    for (int g = 0; g < gensToEvolve; g++) {
        train(players, playersPerTable, 0);
        std::cout << "Finished training gen: " << g << std::endl;
    }

    for (int t = 0; t < population; t++)
        delete players[t];
}

NNEvolver::~NNEvolver() {}

void NNEvolver::train(AIPlayer* players[], int playersPerTable, int threadNum) {
    for (int r = 0; r < 10; r++) {
        for (int t = 0; t < population / playersPerTable; t++) {
            Player *tablePlayers[playersPerTable];
            for (int p = 0; p < playersPerTable; p++)
                tablePlayers[p] = players[t * playersPerTable + p];
//        std::cout << "Playing table: " << t << std::endl;
//        std::cout << tablePlayers[t][0]->playing << std::endl;
            Table table(tablePlayers, playersPerTable);
            table.play();
        }

        // Sort by money
        quicksort(players, 0, population - 1);
    }

    AIPlayer* parents[numOfParents];
    for (int p = 0; p < numOfParents; p++) {
        parents[p] = players[p];
        parents[p]->resetMoney();
    }

    generateNextGen(players, parents);
}

void NNEvolver::generateNextGen(AIPlayer* players[], AIPlayer* parents[]) {
    NeuralNetwork* evolvedNN = players[0]->getNN()->cloneNetworkStructure(false);

    int layers = parents[0]->getNN()->layers;
    const int* neuronsPerLayer = parents[0]->getNN()->neuronsPerLayer;
//    double bias =  1 / (mt_rand() % 999 + 1);
    for (int l = 0; l < parents[0]->getNN()->layers - 1; l++)
        for (int n = 0; n < parents[0]->getNN()->neuronsPerLayer[l]; n++)
            for (int nn = 0; nn < parents[0]->getNN()->neuronsPerLayer[l + 1]; nn++) {
                double weight = 0;
                for (int p = 0; p < numOfParents; p++) {
                    weight += parents[p]->getNN()->getWeightaAt(l, n, nn);
                }
                evolvedNN->setWeightAt(l, n, nn, weight / numOfParents);
            }

    for (int p = 0; p < population; p++) {
        evolvedNN->cloneNetworkInto(players[p]->getNN());
        int mods = mt_rand() % 10 + 5;
        for (int m = 0; m < mods; m++) {
            double noise = ((mt_rand() % 10) / (mt_rand() % 999 + 1) - (1 / 500));
            int l = mt_rand() % (layers - 1);
            int n = mt_rand() % neuronsPerLayer[l];
            int nn = mt_rand() % neuronsPerLayer[l + 1];
            players[p]->getNN()->setWeightAt(l, n, nn, players[p]->getNN()->getWeightaAt(l, n, nn) + noise);
        }
    }

    delete evolvedNN;
}

void NNEvolver::quicksort(AIPlayer* players[], int lPiv, int rPiv) {
    int pivot = players[lPiv + (rPiv - lPiv) / 2]->getMoney();
    int a = lPiv, b = rPiv;

    while (a <= b) {
        while (players[a]->getMoney() > pivot)
            a++;
        while (players[b]->getMoney() < pivot)
            b--;
        if (a <= b) {
            AIPlayer* p = players[a];
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