#ifndef TEXAS_HOLDEM_NN_NNEVOLVER_H
#define TEXAS_HOLDEM_NN_NNEVOLVER_H

#include "../holdem/Player.h"
#include "AIPlayer.h"
#include "../gen_algo/GeneticAlgorithm.h"
#include "../gen_algo/ElitistCombination.h"
#include "../gen_algo/PairCrossover.h"
#include <mutex>
#include <fstream>
#include <atomic>
#include <memory>

class NNEvolver {
public:
	/**
	 * Initializes global evolution variables based on user input, gives user choice of genetic algorithm to use,
	 * and initializes the algorithm specific variables.
	 */
	NNEvolver();

	// For testing purposes
	NNEvolver(unsigned long population, unsigned int playersPerTable, unsigned long gensToEvolve,
	          unsigned long itersPerGen, unsigned long numOfThreads, int evolStrat, unsigned long parents,
	          int crossoverRate, int mutationRate);

	~NNEvolver();

	/**
	 * Entry point for training. Initializes all agents, keeps track of the time the training takes and oversees the
	 * evolution of each generation.
	 */
	void train();

	/**
	 * Takes an integer and abbreviates it using 'k' and 'M' to denote large integers with shorter strings. Used in
	 * function 'writeToFile(NeuralNetwork *)' to shorten the variables used in the evolver, therefore shortening the
	 * file name.
	 *
	 * @param intToShorten Integer to be shortened
	 * @return String representing shortened integer
	 */
	static std::string shortenInt(unsigned long intToShorten);

private:
	/// Global evolution variables
	unsigned long population;
	unsigned long gensToEvolve;
	unsigned long itersPerGen;
	unsigned long numOfThreads;

	unsigned int playersPerTable = 2;
	std::vector<long> threadGens;

	unsigned long generationsPerWorldCup = 100;
	unsigned long playedWorldCups = 0;
	unsigned long worldCupGenerations = 1;

	/// Evolution algorithm to be used to evolve each generation
	GeneticAlgorithm *geneticAlgorithm;

	/// Stream for debugging the neural network, best player NN data is written to a file so it can be inspected and
	/// simulated
	std::ofstream topGenerationalPlayerStream;

	/// Stream for debugging neural network, with bigger generation steps and less volume of data compared to
	/// topGenerationalPlayerStream
	std::ofstream topWorldCupPlayerStream;

	/**
	 * Displays the variables (global and algorithm specific) that will be used in training agents.
	 */
	void displayEvolverInfo();

	/**
	 * Writes the data of a neural network to a file. The file name for the output data will be fashioned after the
	 * variables input. The data is written using the function 'NeuralNetwork::serialize(std::string)'.
	 *
	 * @param nn Neural network to write to a file
	 */
	void writeToFile(NeuralNetwork *nn);

	void trainPlayerGroups(const std::vector<std::vector<AIPlayer *>> &playerGroups);

	void trainPlayerGroup(std::vector<AIPlayer *> &playerGroup, unsigned long gensToTrain);

	void worldCup(const std::vector<std::vector<AIPlayer *>> &playerGroups);

	void printTrainingData(const std::vector<AIPlayer *> &playerGroup);

	/**
	 * Outputs a given number in hours, minutes and seconds.
	 *
	 * @param timeType String to be displayed before the formatted time, can be used as an identifier
	 * @param dur Duration to be displayed in hours, minutes and seconds
	 */
	void outputFormattedTime(const std::string &timeType, long dur);

	// Debugging vals
	std::atomic<unsigned long> raises = 0;
	std::atomic<unsigned long> rounds = 0;
	std::atomic<unsigned long> folds = 0;
	std::atomic<unsigned long> checks = 0;
};

#endif //TEXAS_HOLDEM_NN_NNEVOLVER_H