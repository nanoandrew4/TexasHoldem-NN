#include <iostream>
#include <thread>
#include <iomanip>
#include <sstream>
#include "../../headers/nn/NNEvolver.h"
#include "../../headers/holdem/Table.h"

NNEvolver::NNEvolver() {
    std::cout << "Enter population size, generations to train, game iterations per "
              << "generation and number of threads to be used" << std::endl;
    std::cin >> population >> gensToEvolve >> itersPerGen >> numOfThreads;
    threadGens.resize(numOfThreads, 0);
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

    for (int p = 10; p > 2; p--)
        if (population % p == 0) {
            playersPerTable = p;
            break;
        }

    popPerThread = (population / numOfThreads) + 1;
}

NNEvolver::~NNEvolver() {
    delete geneticAlgorithm;
}

void NNEvolver::displayEvolverInfo() {
    std::cout << std::endl;
    std::cout << "------------------------------------------------" << std::endl;
    std::cout << "Evolution type: " << geneticAlgorithm->getAlgorithmType() << std::endl;
    std::cout << "Population: " << population << std::endl;
    std::cout << "Players per table: " << playersPerTable << std::endl;
    std::cout << "Target generations to evolve: " << gensToEvolve << std::endl;
    std::cout << "Full table rotations per generation: " << itersPerGen << std::endl;
    std::cout << "Threads used: " << numOfThreads << std::endl << std::endl;

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
    for (int p = 0; p < population; p++)
        players.push_back(new AIPlayer());

    displayEvolverInfo();
    long startWallTime = std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1000000000;
    clock_t startCPUTime = clock();

    std::vector<std::thread> threads(numOfThreads);
    int thread = 0;
    for (int startPopPos = 0; startPopPos < population; startPopPos += popPerThread) {
        int startTable = startPopPos == 0 ? 0 : (startPopPos + playersPerTable) / playersPerTable;
        int endTable =
                (startPopPos + popPerThread > population ? population : startPopPos + popPerThread) / playersPerTable;
        threads.at(thread) = std::thread(&NNEvolver::trainerThread, this, thread, startTable, endTable);
        thread++;
    }

    for (int t = 0; t < numOfThreads; t++)
        threads.at(t).join();

    clock_t endCPUTime = clock();
    long endWallTime = std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1000000000;

    std::cout << std::endl;

    long wallTime = endWallTime - startWallTime;
    long CPUTime = (endCPUTime - startCPUTime) / CLOCKS_PER_SEC;

    outputFormattedTime("Wall clock time", wallTime);
    outputFormattedTime("CPU time", CPUTime);
    std::cout << "Multi-core use: "
              << 100.0 - (100.0 * (wallTime - (CPUTime / (float) numOfThreads)) / (wallTime)) << "%" << std::endl;

    std::cout << "------------------------------------------------" << std::endl << std::endl;

    for (int t = 0; t < population; t++)
        delete players.at(t);
}

void NNEvolver::trainerThread(int threadNum, int startTable, int endTable) {
    for (int currGen = 0; currGen < gensToEvolve; currGen++) {
        threadGens.at(threadNum) = currGen;
        for (int t = 0; t < numOfThreads;) { // Wait for all threads to catch up to this generation
            if (threadGens.at(t) >= currGen)
                t++;
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }

        // Play all tables
        for (int t = startTable; t < endTable; t++) {
            std::vector<Player *> tablePlayers(playersPerTable);
            for (int p = 0; p < playersPerTable; p++)
                tablePlayers.at(p) = players.at(t * playersPerTable + p);
            Table table(tablePlayers);
            try {
                table.play();
                rounds += table.rounds;
                raises += table.raises;
                checks += table.checks;
                folds += table.folds;
            } catch(std::exception& e) {
                std::cout << "Error in table" << std::endl;
                std::cout << "Start table: " << startTable << std::endl;
                std::cout << "End table: " << endTable << std::endl;
            }
        }

        mu.lock();
        threadsDone++;
        mu.unlock();

        // Last thread to finish their portion of the generation sorts and evolves the players
        if (threadsDone == numOfThreads) {
//            std::cout << "\rFinished training gen: " << currGen + 1 << std::flush;
            for (int p = 0; p < players.size(); p++)
                if (players.at(p)->getMoney() == 10000)
                    players.at(p)->anteUp(10000);
            quicksort(0, players.size() - 1);
            richestPlayer += players.at(0)->getMoney();
            if (currGen % 100 == 0 && currGen > 0) {
                std::cout << "Gen " << currGen - 100 << " -> " << currGen << " stats:" << std::endl;
                std::cout << "Rounds played: " << rounds / 100.0 << std::endl;
                std::cout << "Times raised: " << raises / 100.0 << std::endl;
                std::cout << "Times checked: " << checks / 100.0 << std::endl;
                std::cout << "Times folded: " << folds / 100.0 << std::endl;
                std::cout << "Richest player money: " << richestPlayer / 100.0 << std::endl;
                std::cout << std::endl;

                rounds = raises = checks = folds = richestPlayer = 0;
            }

            // Genetic algorithm for evolution of population
            bool lastGen = currGen == gensToEvolve - 1;

            try {
                geneticAlgorithm->evolve(players, lastGen);
            } catch(std::exception& e) {
                std::cout << "Error in evolve" << std::endl;
            }
            if (lastGen)
                writeToFile(players.at(0)->getNN());

            threadsDone = 0;
        }
    }
}

void NNEvolver::quicksort(int lPiv, int rPiv) {
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
        quicksort(lPiv, b);
    if (a < rPiv)
        quicksort(a, rPiv);
}

void NNEvolver::outputFormattedTime(std::string timeType, unsigned long dur) {
    std::cout << timeType << ": ";
    if (dur / 3600 > 0)
        std::cout << std::setprecision(2) << dur / 3600 << "h ";
    if ((dur % 3600) / 60 > 0)
        std::cout << std::setprecision(2) << (dur % 3600) / 60 << "m ";
    std::cout << dur % 60 << "s" << std::endl;
}
