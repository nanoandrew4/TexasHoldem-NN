#ifndef TEXAS_HOLDEM_NN_PAIRCROSSOVER_H
#define TEXAS_HOLDEM_NN_PAIRCROSSOVER_H

#include "GeneticAlgorithm.h"

class PairCrossover : public GeneticAlgorithm {
public:
    PairCrossover() = default;

    PairCrossover(int crossoverRate, int mutationRate) {
        this->crossoverRate = crossoverRate;
        this->mutationRate = mutationRate;
    }

    /**
     * Iterates through the whole population, picking two adjacent players and performing crossover and mutation
     * operations on them.
     *
     * Crossover is applied to half the connections, at a rate defined by the user. It essentially swaps a connection
     * between two neurons of one network with the connection between two neurons of another network, at the exact same
     * location in the network (both networks must have the same structure).
     *
     * Mutation is applied at a rate defined by the user. It adds noise to a some connection in the network, to shift it
     * from its original value by a small amount.
     *
     * @param players Vector of players to evolve
     * @param lastGen True if this is the last generation to be evolved, false otherwise
     */
    void evolve(std::vector<AIPlayer *> &players, bool lastGen);

    std::string getAlgorithmType();

    std::string getAlgoDescriptor();

    std::string getVarsDescriptor();

    void setEvolutionVars();

    void displayEvolutionVars();

private:
    int crossoverRate;
    int mutationRate;
};

#endif //TEXAS_HOLDEM_NN_PAIRCROSSOVER_H
