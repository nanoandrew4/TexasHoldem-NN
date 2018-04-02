#ifndef TEXAS_HOLDEM_NN_NNEVOLVER_H
#define TEXAS_HOLDEM_NN_NNEVOLVER_H

#include "../holdem/Player.h"
#include "AIPlayer.h"
#include <mutex>

class NNEvolver {
public:
    NNEvolver(int pop, int gensToEvolve, int numOfParents, int itersPerGen, int threads);
    ~NNEvolver();

    void evolve();

    void setOutFileName(std::string fileName);
private:
    int population;
    int gensToEvolve;
    int currGen = 0;
    int numOfParents;
    int itersPerGen;
    int threads;

    std::string outFileName;

    /**
     * Takes care of initializing the worker threads to simulate a tournament, and after all threads are done,
     * the best agents are combined to make the theoretical best agent. All players are reset to be a noisy version
     * of the best agent
     *
     * @param players Array of AIPlayers to be trained
     */
    void train(std::vector<AIPlayer*> players);

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
    void trainThread(std::vector<AIPlayer*> players, int playersPerTable, int threadNum, int startPlayer, int endPlayer);

    /**
     * Sorts array of agents in descending order, based on how much money they have
     */
    void quicksort(std::vector<AIPlayer*>, int, int);

    /**
     * Combines the parent players to generate the next generation agent. All other players copy that agent and apply
     * some random noise to it, in order to keep evolution from stagnating.
     *
     * @param players Array containing all agents
     * @param parents Array containing agents to be combined for the next generation
     */
    void generateNextGen(std::vector<AIPlayer*> players, std::vector<AIPlayer*> parents);
};

#endif //TEXAS_HOLDEM_NN_NNEVOLVER_H