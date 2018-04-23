#ifndef TEXAS_HOLDEM_NN_NNEVOLVER_H
#define TEXAS_HOLDEM_NN_NNEVOLVER_H

#include "../holdem/Player.h"
#include "AIPlayer.h"
#include "../gen_algo/GeneticAlgorithm.h"
#include "../gen_algo/ElitistCombination.h"
#include "../gen_algo/PairCrossover.h"
#include <mutex>

class NNEvolver {
public:
    /**
     * Initializes global evolution variables based on user input, gives user choice of genetic algorithm to use,
     * and initializes the algorithm specific variables.
     */
    NNEvolver();

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
    static std::string shortenInt(int intToShorten);

private:
    /// Global evolution variables
    int population;
    int gensToEvolve;
    int currGen = 0;
    int itersPerGen;
    int threads;

    /// Evolution algorithm to be used to evolve each generation
    GeneticAlgorithm *geneticAlgorithm;

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
     * Takes care of initializing the worker threads to simulate a tournament, and after all threads are done,
     * the agents are evolved using the desired evolution algorithm.
     *
     * @param players Vector of AIPlayers to be trained
     */
    void trainGen(std::vector<AIPlayer *> players);

    /**
     * Training thread, simulates 'itersPerGen' tables, using players in vector 'players' between 'startPlayer' index
     * and 'endPlayer' index. Breaks up work load amongst threads to improve runtime. Threads are joined before anything
     * is done with the players vector.
     *
     * @param players Vector of AIPlayers to be trained
     * @param playersPerTable Number of players that sit at a table
     * @param startPlayer Index of players vector this thread should start at, in 'players' vector
     * @param endPlayer Index of players vector this thread should end at, in 'players' vector
     */
    void trainGenThread(std::vector<AIPlayer *> players, int playersPerTable, int startPlayer, int endPlayer);

    /**
     * Sorts vector of agents in descending order, based on how much money they have.
     */
    void quicksort(std::vector<AIPlayer *>, int, int);

    /**
     * Outputs a given number in hours, minutes and seconds.
     *
     * @param timeType String to be displayed before the formatted time, can be used as an identifier
     * @param dur Duration to be displayed in hours, minutes and seconds
     */
    void outputFormattedTime(std::string timeType, unsigned long dur);
};

#endif //TEXAS_HOLDEM_NN_NNEVOLVER_H