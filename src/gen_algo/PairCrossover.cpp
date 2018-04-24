#include "../../headers/gen_algo/PairCrossover.h"

void PairCrossover::evolve(std::vector<AIPlayer *> &players, bool lastGen) {
    if (lastGen)
        return;

    int population = players.size();

    std::vector<int> neuronsPerLayer = players.at(0)->getNN()->neuronsPerLayer;
    for (int pop = 0; pop < population - 1; pop += 2) {
        for (int layer = 0; layer < neuronsPerLayer.size() - 1; layer++) {
            int neuronsInLayer = neuronsPerLayer.at(layer);
            for (int neuron = 0; neuron < neuronsInLayer; neuron++) {
                int neuronsInNextLayer = neuronsPerLayer.at(layer + 1);
                for (int nextLayerNeuron = 0; nextLayerNeuron < neuronsInNextLayer; nextLayerNeuron += 2) {
                    double connSwapNetwork1 = players.at(pop)->getNN()->getWeightaAt(layer, neuron, nextLayerNeuron);
                    double connSwapNetwork2 = players.at(pop + 1)->getNN()->getWeightaAt(layer, neuron,
                                                                                         nextLayerNeuron);
                    if (rand() % 1000 < 20) // % chance of mutation
                        connSwapNetwork1 += ((rand() % 10) / (rand() % 999 + 1) - (1 / 500));
                    if (rand() % 1000 < 20) // % chance of mutation
                        connSwapNetwork2 += ((rand() % 10) / (rand() % 999 + 1) - (1 / 500));
                    if (rand() % 1000 < 700) { // % chance of crossover
                        players.at(pop)->getNN()->setWeightAt(layer, neuron, nextLayerNeuron, connSwapNetwork2);
                        players.at(pop + 1)->getNN()->setWeightAt(layer, neuron, nextLayerNeuron, connSwapNetwork1);
                    }
                }
            }
        }
        players.at(pop)->resetMoney();
        players.at(pop + 1)->resetMoney();
    }
}

std::string PairCrossover::getAlgorithmType() {
    return "Pair crossover";
}

std::string PairCrossover::getAlgoDescriptor() {
    return "p-cross";
}

std::string PairCrossover::getVarsDescriptor() {
    std::ostringstream oss;
    oss << "_" << crossoverRate / 10.0 << "-cross_" << mutationRate / 10.0 << "-mut";
    return oss.str();
}

void PairCrossover::setEvolutionVars() {
    std::cout << "Enter crossover rate (0-1000, where 10 is 1% chance): ";
    std::cin >> crossoverRate;
    std::cout << "Enter mutation rate (0-1000, where 10 is 1% chance): ";
    std::cin >> mutationRate;
}

void PairCrossover::displayEvolutionVars() {
    std::cout << "Crossover rate: " << crossoverRate / 10.0 << "%" << std::endl;
    std::cout << "Mutation rate: " << mutationRate / 10.0 << "%" << std::endl;
}