#include <iostream>
#include <algorithm>
#include <random>
#include <thread>
#include <iomanip>
#include <future>
#include "../../headers/nn/NNEvolver.h"
#include "../../headers/holdem/Table.h"

NNEvolver::NNEvolver() : topGenerationalPlayerStream("topGenerationalPlayer.txt"),
                         topWorldCupPlayerStream("topWorldCupPlayer.txt") {
	std::cout << "Enter population size, generations to train, game iterations per "
	          << "generation and number of threads to be used" << std::endl;
	std::cin >> population >> gensToEvolve >> itersPerGen >> numOfThreads;
	threadGens.resize(numOfThreads, 0);
	std::cout << "Enter 0 to use elitist evolution, or 1 to use pair crossover evolution" << std::endl;

	int eStrat;
	std::cin >> eStrat;
	switch (eStrat) {
		default:
		case 0:
			geneticAlgorithm = new ElitistCombination();
			break;
		case 1:
			geneticAlgorithm = new PairCrossover();
			break;
	}
	geneticAlgorithm->setEvolutionVars();
}

// For testing purposes only
NNEvolver::NNEvolver(unsigned long population, unsigned int playersPerTable, unsigned long gensToEvolve,
                     unsigned long itersPerGen, unsigned long numOfThreads, int evolStrat, unsigned long parents,
                     int crossoverRate, int mutationRate) {
	this->population = population;
	this->gensToEvolve = gensToEvolve;
	this->itersPerGen = itersPerGen;
	this->numOfThreads = numOfThreads;
	this->playersPerTable = playersPerTable;

	threadGens.resize(numOfThreads, 0);

	switch (evolStrat) {
		default:
		case 0:
			geneticAlgorithm = new ElitistCombination(parents);
			break;
		case 1:
			geneticAlgorithm = new PairCrossover(crossoverRate, mutationRate);
			break;
	}
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
	if (intToShorten / 1'000'000 > 0)
		oss << intToShorten / 1'000'000 << "M";
	else if (intToShorten / 1'000 > 0)
		oss << intToShorten / 1'000 << "k";
	else
		oss << intToShorten;
	return oss.str();
}

void NNEvolver::train() {
	topGenerationalPlayerStream << numOfThreads << "\n";

	const std::uint8_t inputLayerSize = NeuralNetwork::numOfInputs;
	const std::uint8_t hiddenLayer1Size = inputLayerSize - inputLayerSize / 4;
	const std::uint8_t hiddenLayer2Size = inputLayerSize - inputLayerSize / 3;
	NeuralNetwork::setNeuronsPerLayer({inputLayerSize, hiddenLayer1Size, hiddenLayer2Size, 7});

	std::vector<std::vector<AIPlayer *>> playerGroups(numOfThreads);
	for (unsigned long p = 0; p < population; p++)
		playerGroups.at(p % numOfThreads).emplace_back(new AIPlayer());

	for (const std::vector<AIPlayer *> &playerGroup : playerGroups) {
		if (playerGroup.size() % playersPerTable != 0) {
			std::cout << "Training aborted, population should be adjusted so all tables are fully filled" << std::endl;
			return;
		}
	}

	displayEvolverInfo();
	long startWallTime = std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1'000'000'000;
	clock_t startCPUTime = clock();

	trainPlayerGroups(playerGroups);

	clock_t endCPUTime = clock();
	long endWallTime = std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1'000'000'000;

	std::cout << '\n';

	long wallTime = endWallTime - startWallTime;
	long CPUTime = (endCPUTime - startCPUTime) /
	               CLOCKS_PER_SEC; // TODO: this needs checking... Once other problems have been solved

	outputFormattedTime("Wall clock time", wallTime);
	outputFormattedTime("CPU time", CPUTime);
	std::cout << "Multi-core use: "
	          << 100.0 - (100.0 * (wallTime - (CPUTime / (float) numOfThreads)) / (wallTime)) << "%\n";

	std::cout << "------------------------------------------------\n\n";

	topGenerationalPlayerStream.close();
	topWorldCupPlayerStream.close();

	for (auto &playerGroup : playerGroups)
		for (AIPlayer *player : playerGroup)
			delete player;
}

void NNEvolver::trainPlayerGroups(const std::vector<std::vector<AIPlayer *>> &playerGroups) {
	for (unsigned long g = 0; g < gensToEvolve; g += generationsPerWorldCup) {
		std::vector<std::thread> threads;
		for (auto playerGroup : playerGroups) {
			if (playerGroup != playerGroups.back()) {
				threads.emplace_back(std::thread(&NNEvolver::trainPlayerGroup, this, std::ref(playerGroup),
				                                 std::ref(generationsPerWorldCup)));
			} else {
				trainPlayerGroup(playerGroup, generationsPerWorldCup);
			}
		}
		for (auto &thread: threads)
			thread.join();

		worldCup(playerGroups);
	}
}

void NNEvolver::trainPlayerGroup(std::vector<AIPlayer *> &playerGroup, unsigned long gensToTrain) {
	for (unsigned long currGen = 0; currGen < gensToTrain && currGen < gensToEvolve; currGen++) {
		std::shuffle(playerGroup.begin(), playerGroup.end(), std::mt19937(std::random_device()()));
		for (size_t t = 0; t < playerGroup.size() / playersPerTable; ++t) {
			std::vector<Player *> tablePlayers((unsigned long) playersPerTable /* Will always be less than 11 */);
			for (size_t p = 0; p < playersPerTable; ++p)
				tablePlayers.at(p) = playerGroup.at(t * playersPerTable + p);
			Table table(tablePlayers);
			table.play();
			rounds += table.rounds;
			raises += table.raises;
			checks += table.checks;
			folds += table.folds;
		}

		std::sort(playerGroup.begin(), playerGroup.end(), [](AIPlayer *left, AIPlayer *right) {
			return left->getMoney() > right->getMoney();
		});

		if (gensToTrain == worldCupGenerations) { // World cup round
			playerGroup.at(0)->getNN()->serialize(topWorldCupPlayerStream);
			printTrainingData(playerGroup);
		}

		if (currGen < gensToEvolve - 1)
			geneticAlgorithm->evolve(playerGroup);
		else
			return;
	}
}

void NNEvolver::worldCup(const std::vector<std::vector<AIPlayer *>> &playerGroups) {
	std::vector<AIPlayer *> players;
	for (auto &playerGroup : playerGroups)
		std::move(playerGroup.begin(), playerGroup.end(), std::back_inserter(players));
	std::cout << "Stats for World Cup " << ++playedWorldCups << " of " << gensToEvolve / generationsPerWorldCup << '\n';
	trainPlayerGroup(players, 1);
	// Shuffle player groups?
}

void NNEvolver::printTrainingData(const std::vector<AIPlayer *> &playerGroup) {
	std::cout << "Rounds played: " << rounds << '\n';
	std::cout << "Times raised: " << raises << '\n';
	std::cout << "Times checked: " << checks << '\n';
	std::cout << "Times folded: " << folds << '\n';
	std::cout << "Richest player money: " << playerGroup.at(0)->getMoney() << "\n\n";

	unsigned long totalMoney = 0;
	unsigned long playersHoldingHalfOfAllMoney = 0;
	for (unsigned long p = 0; p < playerGroup.size(); p++) {
		totalMoney += playerGroup.at(p)->getMoney();
		if (totalMoney < playerGroup.size() * 5'000)
			playersHoldingHalfOfAllMoney++;
	}
	std::cout << "TotalMoney: " << totalMoney << "\n"; // TODO: EXTRA MONEY BEING CREATED :(
	std::cout << playersHoldingHalfOfAllMoney << " players hold roughly half of the total money\n\n";

	rounds = raises = checks = folds = 0;
}

void NNEvolver::outputFormattedTime(const std::string &timeType, long dur) {
	std::cout << timeType << ": ";
	if (dur / 3600 > 0)
		std::cout << std::setprecision(2) << dur / 3600 << "h ";
	if ((dur % 3600) / 60 > 0)
		std::cout << std::setprecision(2) << (dur % 3600) / 60 << "m ";
	std::cout << dur % 60 << "s" << std::endl;
}