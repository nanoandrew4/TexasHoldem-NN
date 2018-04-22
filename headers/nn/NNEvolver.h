#ifndef TEXAS_HOLDEM_NN_NNEVOLVER_H
#define TEXAS_HOLDEM_NN_NNEVOLVER_H

#include "../holdem/Player.h"
#include "AIPlayer.h"
#include <mutex>

enum EvolutionStrat {
    PAIR_CROSS, ELISIST
};

class NNEvolver {
public:
    NNEvolver();

    ~NNEvolver();

    void train();

private:
    int population;
    int gensToEvolve;
    int currGen = 0;
    int numOfParents = 0;
    int itersPerGen;
    int threads;

    std::string outFileName;
    EvolutionStrat evolutionStrat;

    /**
     * Takes care of initializing the worker threads to simulate a tournament, and after all threads are done,
     * the best agents are combined to make the theoretical best agent. All players are reset to be a noisy version
     * of the best agent
     *
     * @param players Array of AIPlayers to be trained
     */
    void trainGen(std::vector<AIPlayer *> players);

    /**
     * Training thread, simulates 'itersPerGen' tables, using players in array 'players' between 'startPlayer' index and
     * 'endPlayer' index.
     *
     * @param players Array of AIPlayers to be trained
     * @param playersPerTable Number of players that can sit at a table
     * @param threadNum Number of the thread, in order to coordinate between threads using threadReady array
     * @param startPlayer Index of players array this thread should start at
     * @param endPlayer Index of players array this thread should end at
     */
    void
    trainGenThread(std::vector<AIPlayer *> players, int playersPerTable, int threadNum, int startPlayer, int endPlayer);

    /**
     * Combines the parent players to generate the next generation agent. All other players copy that agent and apply
     * some random noise to it, in order to keep evolution from stagnating. The number of parents is specified when
     * initializing a NNEvolver instance, and they represent the best scoring players of the population.
     *
     * @param players Array containing all agents
     * @param parents Array containing agents to be combined for the next generation
     */
    void elitistCombination(std::vector<AIPlayer *> players, std::vector<AIPlayer *> parents);

    void pairCrossover(std::vector<AIPlayer *> players);

    /**
     * Sorts array of agents in descending order, based on how much money they have
     */
    void quicksort(std::vector<AIPlayer *>, int, int);

    /**
     * Outputs a given number in hours, minutes and seconds
     * @param dur Duration to be displayed in hours, minutes and seconds
     */
    void outputFormattedTime(unsigned long dur);
};

#endif //TEXAS_HOLDEM_NN_NNEVOLVER_H