#ifndef TEXAS_HOLDEM_NN_NNEVOLVER_H
#define TEXAS_HOLDEM_NN_NNEVOLVER_H

#include "../holdem/Player.h"
#include "AIPlayer.h"
#include "../gen_algo/GeneticAlgorithm.h"
#include "../gen_algo/ElitistCombination.h"
#include "../gen_algo/PairCrossover.h"
#include <mutex>
#include <fstream>

class NNEvolver {
public:
	/**
	 * Initializes global evolution variables based on user input, gives user choice of genetic algorithm to use,
	 * and initializes the algorithm specific variables.
	 */
	NNEvolver();

	// For testing purposes
	NNEvolver(unsigned long population, unsigned long gensToeEvolve, unsigned long itersPerGen,
	          unsigned long numOfThreads, int evolStrat, unsigned long parents, int crossoverRate, int mutationRate);

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
	/// Vector of players to be evolved
	std::vector<AIPlayer *> players;

	/// Global evolution variables
	unsigned long population;
	unsigned long gensToEvolve;
	unsigned long itersPerGen;
	unsigned long numOfThreads;

	int playersPerTable = 2;
	unsigned long popPerThread;
	std::vector<long> threadGens;
	unsigned long threadsDone = 0;

	std::mutex mu;

	/// Evolution algorithm to be used to evolve each generation
	GeneticAlgorithm *geneticAlgorithm;

	/// Stream for debugging the neural network, best player NN data is written to a file so it can be inspected and
	/// simulated
	std::ofstream topGenerationalPlayerStream;

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

	/**
	 * Training thread, simulates 'itersPerGen' tables, using players in vector 'players' between 'startPlayer' index
	 * and 'endPlayer' index. Breaks up work load amongst threads to improve runtime. Threads are joined before anything
	 * is done with the players vector.
	 *
	 * @param threadNum Thread identifier
	 * @param startTable TODO
	 * @param endTable TODO
	 */
	void trainerThread(size_t threadNum, size_t startTable, size_t endTable);

	/**
	 * Outputs a given number in hours, minutes and seconds.
	 *
	 * @param timeType String to be displayed before the formatted time, can be used as an identifier
	 * @param dur Duration to be displayed in hours, minutes and seconds
	 */
	void outputFormattedTime(const std::string &timeType, long dur);

	// Debugging vals
	int raises = 0;
	int rounds = 0;
	int folds = 0;
	int checks = 0;
};

#endif //TEXAS_HOLDEM_NN_NNEVOLVER_H