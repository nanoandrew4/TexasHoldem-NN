#include <iostream>
#include <thread>
#include <iomanip>
#include "../../headers/nn/NNEvolver.h"
#include "../../headers/holdem/Table.h"

static std::mt19937_64 mt_rand((ulong) std::chrono::high_resolution_clock::now().time_since_epoch().count());

NNEvolver::NNEvolver() {
    std::cout << "Enter population size, generations to train, game iterations per "
              << "generation and number of threads to be used" << std::endl;
    std::cin >> population >> gensToEvolve >> itersPerGen >> threads;
    std::cout << "Enter 0 to use elitist evolution, or 1 to use pair crossover evolution" << std::endl;

    int eStrat;
    std::cin >> eStrat;
    evolutionStrat = eStrat == 0 ? EvolutionStrat::ELISIST : EvolutionStrat::PAIR_CROSS;

    if (evolutionStrat == ELISIST) {
        std::cout << "Enter number of parents to be used in evolving each generation: ";
        std::cin >> numOfParents;
    }

    std::cout << "Enter filename to save agent data to: ";
    std::cin >> outFileName;
}

NNEvolver::~NNEvolver() = default;

void NNEvolver::train() {
    std::vector<AIPlayer *> players;

    for (int p = 0; p < population; p++)
        players.push_back(new AIPlayer());

    std::cout << "Training" << std::endl;
    long start = std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1000000000;
    for (; currGen < gensToEvolve; currGen++) {
        trainGen(players);
        std::cout << "\rFinished training gen: " << (currGen + 1) << std::flush;
    }
    std::cout << std::endl;

    outputFormattedTime(std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1000000000 - start);

    for (int t = 0; t < population; t++)
        delete players.at(t);
}

void NNEvolver::trainGen(std::vector<AIPlayer *> players) {
    int playersPerTable = 2;
    for (int p = 10; p > 2; p--)
        if (population % p == 0) {
            playersPerTable = p;
            break;
        }

    int popPerThread = population / this->threads + 1;

    for (int g = 0; g < itersPerGen; g++) {
        std::vector<std::thread> threads(this->threads);
        int thread = 0;
        for (int startPopPos = 0; startPopPos < population; startPopPos += popPerThread) {
            threads.at(thread) = std::thread(&NNEvolver::trainGenThread, this, players, playersPerTable, thread,
                                             startPopPos == 0 ? 0 : startPopPos + playersPerTable,
                                             startPopPos + popPerThread > population ? population : startPopPos +
                                                                                                    popPerThread);
            thread++;
        }

        for (int t = 0; t < this->threads; t++)
            threads.at(t).join();
        threads.clear();

        quicksort(players, 0, players.size() - 1);
    }

    // Genetic algorithm for evolution of population

    if (evolutionStrat == ELISIST) {
        std::vector<AIPlayer *> parents(numOfParents);
        for (int p = 0; p < numOfParents; p++) {
            parents.at(p) = players.at(p);
            parents.at(p)->resetMoney();
        }

        elitistCombination(players, parents);
    } else if (evolutionStrat == PAIR_CROSS)
        pairCrossover(players);
}

void NNEvolver::trainGenThread(std::vector<AIPlayer *> players, int playersPerTable, int threadNum, int startPlayer,
                               int endPlayer) {
    for (int t = startPlayer / playersPerTable; t < endPlayer / playersPerTable; t++) {
        std::vector<Player *> tablePlayers(playersPerTable);
        for (int p = 0; p < playersPerTable; p++)
            tablePlayers.at(p) = players.at(t * playersPerTable + p);
        Table table(tablePlayers);
        table.play();
    }
}

void NNEvolver::elitistCombination(std::vector<AIPlayer *> players, std::vector<AIPlayer *> parents) {
    NeuralNetwork *evolvedNN = players.at(0)->getNN()->cloneNetworkStructure(false);

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
        evolvedNN->serialize(outFileName + ".dat");
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

void NNEvolver::pairCrossover(std::vector<AIPlayer *> players) {
    std::vector<int> neuronsPerLayer = players.at(0)->getNN()->neuronsPerLayer;
    for (int pop = 0; pop < population - 1; pop += 2) {
        for (int layer = 0; layer < neuronsPerLayer.size() - 1; layer++) {
            int neuronsInLayer = neuronsPerLayer.at(layer);
            for (int neuron = 0; neuron < neuronsInLayer; neuron++) {
                int neuronsInNextLayer = neuronsPerLayer.at(layer + 1);
                for (int nextLayerNeuron = 0; nextLayerNeuron < neuronsInNextLayer; nextLayerNeuron+=2) {
                    double connSwapNetwork1 = players.at(pop)->getNN()->getWeightaAt(layer, neuron, nextLayerNeuron);
                    double connSwapNetwork2 = players.at(pop + 1)-> getNN()->getWeightaAt(layer, neuron, nextLayerNeuron);
                    if (mt_rand() % 100 < 2) // 2% chance of mutation
                        connSwapNetwork1 += ((mt_rand() % 10) / (mt_rand() % 999 + 1) - (1 / 500));
                    if (mt_rand() % 100 < 2) // 2% chance of mutation
                        connSwapNetwork2 += ((mt_rand() % 10) / (mt_rand() % 999 + 1) - (1 / 500));
                    if (mt_rand() % 10 < 6) { // 60% chance of crossover
                        players.at(pop)->getNN()->setWeightAt(layer, neuron, nextLayerNeuron, connSwapNetwork2);
                        players.at(pop + 1)->getNN()->setWeightAt(layer, neuron ,nextLayerNeuron, connSwapNetwork1);
                    }
                }
            }
        }
    }
}

void NNEvolver::quicksort(std::vector<AIPlayer *> players, int lPiv, int rPiv) {
    int pivot = players.at(lPiv + (rPiv - lPiv) / 2)->getMoney();
    int a = lPiv, b = rPiv;

    while (a <= b) {
        while (players.at(a)->getMoney() > pivot)
            a++;
        while (players.at(b)->getMoney() < pivot)
            b--;
        if (a <= b) {
            AIPlayer *p = players.at(a);
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

void NNEvolver::outputFormattedTime(unsigned long dur) {
    std::cout << "Duration: ";
    if (dur / 3600 > 0)
        std::cout << std::setprecision(2) << dur / 3600 << "h ";
    if ((dur % 3600) / 60 > 0)
        std::cout << std::setprecision(2) << (dur % 3600) / 60 << "m ";
    std::cout << dur % 60 << "s" << std::endl;
}