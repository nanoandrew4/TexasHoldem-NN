#include <iostream>
#include <algorithm>
#include <thread>
#include <iomanip>
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

	for (int p = 10; p > 2; --p)
		if (population % p == 0) {
			playersPerTable = p;
			break;
		}

	popPerThread = (population / numOfThreads) + 1;
}

// For testing purposes only
NNEvolver::NNEvolver(unsigned long population, unsigned long gensToeEvolve, unsigned long itersPerGen,
                     unsigned long numOfThreads, int evolStrat, unsigned long parents, int crossoverRate,
                     int mutationRate) {
	this->population = population;
	this->gensToEvolve = gensToeEvolve;
	this->itersPerGen = itersPerGen;
	this->numOfThreads = numOfThreads;

	threadGens.resize(numOfThreads, 0);

	switch (evolStrat) {
		case 0:
			geneticAlgorithm = new ElitistCombination(parents);
			break;
		case 1:
			geneticAlgorithm = new PairCrossover(crossoverRate, mutationRate);
			break;
	}

	for (int p = 10; p > 2; --p)
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

std::string NNEvolver::shortenInt(unsigned long intToShorten) {
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
	for (unsigned long p = 0; p < population; ++p)
		players.push_back(new AIPlayer());

	displayEvolverInfo();
	long startWallTime = std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1000000000;
	clock_t startCPUTime = clock();

	std::vector<std::thread> threads(numOfThreads);
	size_t thread = 0;
	for (unsigned long startPopPos = 0; startPopPos < population; startPopPos += popPerThread) {
		unsigned long startTable = startPopPos == 0 ? 0 : (startPopPos + playersPerTable) / playersPerTable;
		unsigned long endTable =
				(startPopPos + popPerThread > population ? population : startPopPos + popPerThread) / playersPerTable;
		threads.at(thread) = std::thread(&NNEvolver::trainerThread, this, thread, startTable, endTable);
		++thread;
	}

	for (size_t t = 0; t < numOfThreads; ++t)
		threads.at(t).join();

	clock_t endCPUTime = clock();
	long endWallTime = std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1000000000;

	std::cout << '\n';

	long wallTime = endWallTime - startWallTime;
	long CPUTime = (endCPUTime - startCPUTime) /
	               CLOCKS_PER_SEC; // TODO: this needs checking... Once other problems have been solved

	outputFormattedTime("Wall clock time", wallTime);
	outputFormattedTime("CPU time", CPUTime);
	std::cout << "Multi-core use: "
	          << 100.0 - (100.0 * (wallTime - (CPUTime / (float) numOfThreads)) / (wallTime)) << "%\n";

	std::cout << "------------------------------------------------\n\n";

	for (unsigned long t = 0; t < population; t++)
		delete players.at(t);
}

void NNEvolver::trainerThread(size_t threadNum, size_t startTable, size_t endTable) {
	for (unsigned long currGen = 0; currGen < gensToEvolve; ++currGen) {
		threadGens.at(threadNum) = currGen;
		for (size_t t = 0; t < numOfThreads;) { // Wait for all threads to catch up to this generation
			if (threadGens.at(t) >= currGen)
				++t;
			std::this_thread::sleep_for(std::chrono::microseconds(100));
		}

		// Play all tables
		for (size_t t = startTable; t < endTable; ++t) {
			std::vector<Player *> tablePlayers((unsigned long) playersPerTable /* Will always be less than 11 */);
			for (size_t p = 0; p < playersPerTable; ++p)
				tablePlayers.at(p) = players.at(t * playersPerTable + p);
			Table table(tablePlayers);
			try {
				table.play();
				rounds += table.rounds;
				raises += table.raises;
				checks += table.checks;
				folds += table.folds;
			} catch (std::exception &e) {
				mu.lock();
				std::cout << "Error in table" << std::endl;
//				std::cout << "Start table: " << startTable << std::endl;
//				std::cout << "End table: " << endTable << std::endl;
				mu.unlock();
			}
		}

		mu.lock();
		threadsDone++;
		mu.unlock();

		// Last thread to finish their portion of the generation sorts and evolves the players
		if (threadsDone == numOfThreads) {
//            std::cout << "\rFinished training gen: " << currGen + 1 << std::flush;
//            for (size_t p = 0; p < players.size(); ++p)
//                if (players.at(p)->getMoney() == 10000)
//                    players.at(p)->anteUp(10000);
			std::sort(players.begin(), players.end(), [](AIPlayer *left, AIPlayer *right) -> bool {
				return left->getMoney() > right->getMoney();
			});

			if (currGen % 100 == 0 && currGen > 0) {
				std::cout << "Gen " << currGen - 100 << " -> " << currGen << " stats:\n";
				std::cout << "Rounds played: " << rounds / (double) 100 << '\n';
				std::cout << "Times raised: " << raises / (double) 100 << '\n';
				std::cout << "Times checked: " << checks / (double) 100 << '\n';
				std::cout << "Times folded: " << folds / (double) 100 << '\n';
				std::cout << "Richest player money: " << players.at(0)->getMoney() << "\n\n";

				rounds = raises = checks = folds = 0;
			}

			// Genetic algorithm for evolution of population
			bool lastGen = currGen == gensToEvolve - 1;

			try {
				geneticAlgorithm->evolve(players, lastGen);
			} catch (std::exception &e) {
				std::cout << "Error in evolve" << std::endl;
			}
			if (lastGen)
				writeToFile(players.at(0)->getNN());

			threadsDone = 0;
		}
	}
}

void NNEvolver::outputFormattedTime(std::string timeType, long dur) {
	std::cout << timeType << ": ";
	if (dur / 3600 > 0)
		std::cout << std::setprecision(2) << dur / 3600 << "h ";
	if ((dur % 3600) / 60 > 0)
		std::cout << std::setprecision(2) << (dur % 3600) / 60 << "m ";
	std::cout << dur % 60 << "s" << std::endl;
}
