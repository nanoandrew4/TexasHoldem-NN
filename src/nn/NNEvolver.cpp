#include <iostream>
#include <thread>
#include <iomanip>
#include <sstream>
#include "../../headers/nn/NNEvolver.h"
#include "../../headers/holdem/Table.h"

NNEvolver::NNEvolver() {
    std::cout << "Enter population size, generations to train, game iterations per "
              << "generation and number of threads to be used" << std::endl;
    std::cin >> population >> gensToEvolve >> itersPerGen >> threads;
    std::cout << "Enter 0 to use elitist evolution, or 1 to use pair crossover evolution" << std::endl;

    int eStrat;
    std::cin >> eStrat;
    switch (eStrat) {
        case 0:
            geneticAlgorithm = new ElitistCombination();
            break;
        case 1:
            geneticAlgorithm = new PairCrossover();
            break;
    }
    geneticAlgorithm->setEvolutionVars();
}

NNEvolver::~NNEvolver() {
    delete geneticAlgorithm;
}

void NNEvolver::displayEvolverInfo() {
    std::cout << std::endl;
    std::cout << "------------------------------------------------" << std::endl;
    std::cout << "Evolution type: " << geneticAlgorithm->getAlgorithmType() << std::endl;
    std::cout << "Population: " << population << std::endl;
    std::cout << "Target generations to evolve: " << gensToEvolve << std::endl;
    std::cout << "Full table rotations per generation: " << itersPerGen << std::endl;
    std::cout << "Threads used: " << threads << std::endl << std::endl;

    std::cout << "~Evolution type specific options~" << std::endl;
    geneticAlgorithm->displayEvolutionVars();

    std::cout << std::endl;
}

void NNEvolver::writeToFile(NeuralNetwork *nn) {
    std::ostringstream oss;
    oss << "nn_" << geneticAlgorithm->getAlgoDescriptor() + "_" << shortenInt(population) << "-pop_"
        << shortenInt(gensToEvolve) << "-gens_" << shortenInt(itersPerGen) << "-iters"
        << geneticAlgorithm->getVarsDescriptor() << ".dat";

    nn->serialize(oss.str());
    std::cout << std::endl << "Best agent saved under file: " << oss.str() << std::endl;
}

std::string NNEvolver::shortenInt(int intToShorten) {
    std::ostringstream oss;
    if (intToShorten / 1000000 > 0)
        oss << intToShorten / 1000000 << "M";
    else if (intToShorten / 1000 > 0)
        oss << intToShorten / 1000 << "k";
    else
        oss << intToShorten;
    return oss.str();
}

void NNEvolver::train() {
    std::vector<AIPlayer *> players;
    for (int p = 0; p < population; p++)
        players.push_back(new AIPlayer());

    displayEvolverInfo();
    long startWallTime = std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1000000000;
    clock_t startCPUTime = clock();

    for (; currGen < gensToEvolve; currGen++) {
        std::cout << "\rTraining gen: " << (currGen + 1) << std::flush;
        trainGen(players);
    }

    clock_t endCPUTime = clock();
    long endWallTime = std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1000000000;

    std::cout << std::endl;

    long wallTime = endWallTime - startWallTime;
    long CPUTime = (endCPUTime - startCPUTime) / CLOCKS_PER_SEC;

    outputFormattedTime("Wall clock time", wallTime);
    outputFormattedTime("CPU time", CPUTime);
    std::cout << "Multi-core use: "
              << 100.0 - (100.0 * (wallTime - (CPUTime / (float) threads)) / (wallTime)) << "%" << std::endl;

    std::cout << "------------------------------------------------" << std::endl << std::endl;

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

    int popPerThread = (population / this->threads) + 1;

    for (int g = 0; g < itersPerGen; g++) {
        std::vector<std::thread> threads(this->threads);
        int thread = 0;
        for (int startPopPos = 0; startPopPos < population; startPopPos += popPerThread) {
            threads.at(thread) = std::thread(&NNEvolver::trainGenThread, this, players, playersPerTable,
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

    bool lastGen = currGen == gensToEvolve - 1;
    geneticAlgorithm->evolve(players, lastGen);
    if (lastGen)
        writeToFile(players.at(0)->getNN());
}

void NNEvolver::trainGenThread(std::vector<AIPlayer *> players, int playersPerTable, int startPlayer,
                               int endPlayer) {
    for (int t = startPlayer / playersPerTable; t < endPlayer / playersPerTable; t++) {
        std::vector<Player *> tablePlayers(playersPerTable);
        for (int p = 0; p < playersPerTable; p++)
            tablePlayers.at(p) = players.at(t * playersPerTable + p);
        Table table(tablePlayers);
        table.play();
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

void NNEvolver::outputFormattedTime(std::string timeType, unsigned long dur) {
    std::cout << timeType << ": ";
    if (dur / 3600 > 0)
        std::cout << std::setprecision(2) << dur / 3600 << "h ";
    if ((dur % 3600) / 60 > 0)
        std::cout << std::setprecision(2) << (dur % 3600) / 60 << "m ";
    std::cout << dur % 60 << "s" << std::endl;
}
