#include <iostream>
#include <thread>
#include "../../headers/nn/NNEvolver.h"
#include "../../headers/holdem/Table.h"

static std::mt19937_64 mt_rand((ulong) std::chrono::high_resolution_clock::now().time_since_epoch().count());

NNEvolver::NNEvolver() {
    std::vector<AIPlayer*> players;

    for (int t = 0; t < threads; t++) {
        threadReady[t] = false;
        threadLocked[t] = true;
    }

    for (int p = 0; p < population; p++)
        players.push_back(new AIPlayer());

    std::cout << "Training" << std::endl;
    for (int g = 0; g < gensToEvolve; g++) {
        train(players);
        std::cout << "Finished training gen: " << g << std::endl;
    }

    for (int t = 0; t < population; t++)
        delete players.at(t);
}

NNEvolver::~NNEvolver() {}

void NNEvolver::train(std::vector<AIPlayer*> players) {
    int playersPerTable = 2;
    for (int p = 10; p > 2; p--)
        if (population % p == 0) {
            playersPerTable = p;
            break;
        }

    int popPerThread = (int)((float) (population) / this->threads + 0.5);

    for (int g = 0; g < itersPerGen; g++) {
        std::vector<std::thread> threads(this->threads);
        int startPopPos = 0;
        int thread = 0;

        long start = std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1000000;
        for (; startPopPos < population; startPopPos += popPerThread) {
            threads.at(thread) = std::thread(&NNEvolver::trainThread, this, players, playersPerTable, thread,
                                             startPopPos,
                                             (startPopPos + popPerThread > population ? population : startPopPos +
                                                                                                     popPerThread));
            thread++;
        }

        for (int t = 0; t < this->threads; t++)
            threads.at(t).join();

        quicksort(players, 0, players.size() - 1);

//        std::cout << "Training took: "
//                  << std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1000000 - start << "ms"
//                  << std::endl;
    }

    long start = std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1000000;
    std::vector<AIPlayer*> parents(numOfParents);
    for (int p = 0; p < numOfParents; p++) {
        parents.at(p) = players.at(p);
        parents.at(p)->resetMoney();
    }

    generateNextGen(players, parents);
//    std::cout << "Evolution took: " << std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1000000 - start << "ms" << std::endl;
}

void NNEvolver::trainThread(std::vector<AIPlayer*> players, int playersPerTable, int threadNum, int startPlayer, int endPlayer) {
    for (int t = startPlayer / playersPerTable; t < endPlayer / playersPerTable; t++) {
        std::vector<Player*> tablePlayers(playersPerTable);
        for (int p = 0; p < playersPerTable; p++)
            tablePlayers.at(p) = players.at(t * playersPerTable + p);
        Table table(tablePlayers, playersPerTable);
        table.play();
    }
}

void NNEvolver::generateNextGen(std::vector<AIPlayer*> players, std::vector<AIPlayer*> parents) {
    NeuralNetwork* evolvedNN = players.at(0)->getNN()->cloneNetworkStructure(false);

    int layers = parents.at(0)->getNN()->neuronsPerLayer.size();
    const std::vector<int> neuronsPerLayer = parents.at(0)->getNN()->neuronsPerLayer;
    for (int l = 0; l < layers - 1; l++)
        for (int n = 0; n < parents.at(0)->getNN()->neuronsPerLayer.at(l); n++)
            for (int nn = 0; nn < parents.at(0)->getNN()->neuronsPerLayer.at(l + 1); nn++) {
                double weight = 0;
                for (int p = 0; p < numOfParents; p++)
                    weight += parents.at(p)->getNN()->getWeightaAt(l, n, nn);
                evolvedNN->setWeightAt(l, n, nn, weight / numOfParents);
            }

    if (currGen == gensToEvolve - 1) {
        std::cout << "Serialized" << std::endl;
        evolvedNN->serialize("nn.dat");
    } else {
        for (int p = 0; p < population; p++) {
            evolvedNN->cloneNetworkInto(players[p]->getNN());
            int mods = mt_rand() % 10 + 5;
            for (int m = 0; m < mods; m++) {
                double noise = ((mt_rand() % 10) / (mt_rand() % 999 + 1) - (1 / 500));
                int l = mt_rand() % (layers - 1);
                int n = mt_rand() % neuronsPerLayer.at(l);
                int nn = mt_rand() % neuronsPerLayer.at(l + 1);
                players.at(p)->getNN()->setWeightAt(l, n, nn, players.at(p)->getNN()->getWeightaAt(l, n, nn) + noise);
            }
        }
    }

    delete evolvedNN;
}

void NNEvolver::quicksort(std::vector<AIPlayer*> players, int lPiv, int rPiv) {
    int pivot = players.at(lPiv + (rPiv - lPiv) / 2)->getMoney();
    int a = lPiv, b = rPiv;

    while (a <= b) {
        while (players.at(a)->getMoney() > pivot)
            a++;
        while (players.at(b)->getMoney() < pivot)
            b--;
        if (a <= b) {
            AIPlayer* p = players.at(a);
            players.at(a) = players.at(b);
            players.at(b) = p;
            a++;
            b--;
        }
    }

    if (lPiv < b)
        quicksort(players, lPiv, b);
    if (a < rPiv)
        quicksort(players, a, rPiv);
}