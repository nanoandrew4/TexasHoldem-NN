#include "../../headers/gen_algo/ElitistCombination.h"
#include "../../headers/nn/NNEvolver.h"

void ElitistCombination::evolve(std::vector<AIPlayer *> &players, bool lastGen) {
    int population = players.size();
    NeuralNetwork *evolvedNN = players.at(0)->getNN()->cloneNetworkStructure(false);

    std::vector<AIPlayer *> parents(numOfParents);
    for (int p = 0; p < numOfParents; p++)
        parents.at(p) = players.at(p);

    int layers = parents.at(0)->getNN()->numOfLayers;
    const std::array<unsigned long, NeuralNetwork::numOfLayers> neuronsPerLayer = parents.at(0)->getNN()->getNeuronsPerLayer();
    for (int l = 0; l < layers - 1; l++)
        for (int n = 0; n < parents.at(0)->getNN()->getNeuronsPerLayer().at(l); n++)
            for (int nn = 0; nn < parents.at(0)->getNN()->getNeuronsPerLayer().at(l + 1); nn++) {
                double weight = 0;
                for (int p = 0; p < numOfParents; p++)
                    weight += parents.at(p)->getNN()->getWeightaAt(l, n, nn);
                evolvedNN->setWeightAt(l, n, nn, weight / numOfParents);
            }

    if (lastGen) {
        players.at(0)->setNN(evolvedNN);
        return;
    } else {
        for (int p = 0; p < population; p++) {
            evolvedNN->cloneNetworkInto(players.at(p)->getNN());
            int mods = rand() % 10 + 5;
            for (int m = 0; m < mods; m++) {
                double noise = ((rand() % 10) / (rand() % 999 + 1) - (1 / 500));
                int l = rand() % (layers - 1);
                int n = rand() % neuronsPerLayer.at(l);
                int nn = rand() % neuronsPerLayer.at(l + 1);
                players.at(p)->getNN()->setWeightAt(l, n, nn, players.at(p)->getNN()->getWeightaAt(l, n, nn) + noise);
            }
            players.at(p)->resetMoney();
        }
    }

    delete evolvedNN;
}

std::string ElitistCombination::getAlgorithmType() {
    return "Elitist Evolution";
}

std::string ElitistCombination::getAlgoDescriptor() {
    return "elitist";
}

std::string ElitistCombination::getVarsDescriptor() {
    return "_" + NNEvolver::shortenInt(numOfParents) + "-parents";
}

void ElitistCombination::setEvolutionVars() {
    std::cout << "Enter number of parents: ";
    std::cin >> numOfParents;
}

void ElitistCombination::displayEvolutionVars() {
    std::cout << "Parents: " << numOfParents << std::endl;
}
