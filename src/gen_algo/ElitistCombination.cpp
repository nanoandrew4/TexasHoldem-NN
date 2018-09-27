#include "../../headers/gen_algo/ElitistCombination.h"
#include "../../headers/nn/NNEvolver.h"

std::mt19937_64 ElitistCombination::mt_rand(std::random_device().operator()());

void ElitistCombination::evolve(std::vector<AIPlayer *> &players, bool lastGen) {
    size_t population = players.size();
    NeuralNetwork *evolvedNN = players.at(0)->getNN()->cloneNetworkStructure(false);

    std::vector<AIPlayer *> parents(numOfParents);
    for (size_t p = 0; p < numOfParents; p++)
        parents.at(p) = players.at(p);

    int layers = parents.at(0)->getNN()->numOfLayers;
    const std::array<std::uint8_t, NeuralNetwork::numOfLayers> neuronsPerLayer = parents.at(0)->getNN()
                                                                                        ->getNeuronsPerLayer();
    for (size_t l = 0; l < layers - 1; l++)
        for (size_t n = 0; n < parents.at(0)->getNN()->getNeuronsPerLayer().at(l); n++)
            for (size_t nn = 0; nn < parents.at(0)->getNN()->getNeuronsPerLayer().at(l + 1); nn++) {
                double weight = 0;
                for (size_t p = 0; p < numOfParents; p++)
                    weight += parents.at(p)->getNN()->getWeightaAt(l, n, nn);
                evolvedNN->setWeightAt(l, n, nn, weight / numOfParents);
            }

    if (lastGen) {
        players.at(0)->setNN(evolvedNN);
        return;
    } else {
        for (size_t p = 0; p < population; p++) {
            evolvedNN->cloneNetworkInto(players.at(p)->getNN());
            size_t mods = mt_rand() % 10 + 5;
            for (int m = 0; m < mods; m++) {
                double noise = ((mt_rand() % 10) / (mt_rand() % 999 + 1) - (1 / 500));
                size_t l = mt_rand() % (layers - 1);
                size_t n = mt_rand() % neuronsPerLayer.at(l);
                size_t nn = mt_rand() % neuronsPerLayer.at(l + 1);
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
