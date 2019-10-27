#include "../../headers/gen_algo/ElitistCombination.h"
#include "../../headers/nn/NNEvolver.h"

std::mt19937_64 ElitistCombination::mt_rand(std::random_device().operator()());

std::uniform_real_distribution<> ElitistCombination::noiseDistribution(-0.5, 0.5);

void ElitistCombination::evolve(const std::vector<AIPlayer *> &players) const {
	size_t population = players.size();
	NeuralNetwork *evolvedNN = players.at(0)->getNN()->cloneNetworkStructure(false);

	std::vector<AIPlayer *> parents(numOfParents);
	for (size_t p = 0; p < numOfParents; p++)
		parents.at(p) = players.at(p);

	int layers = NeuralNetwork::numOfLayers;
	const std::array<std::uint8_t, NeuralNetwork::numOfLayers> neuronsPerLayer = NeuralNetwork::getNeuronsPerLayer();
	for (size_t l = 0; l < layers - 1; l++) {
		for (size_t n = 0; n < NeuralNetwork::getNeuronsPerLayer().at(l); n++) {
			for (size_t nn = 0; nn < NeuralNetwork::getNeuronsPerLayer().at(l + 1); nn++) {
				double weight = 0;
				for (size_t p = 0; p < numOfParents; p++)
					weight += parents.at(p)->getNN()->getWeightaAt(l, n, nn);
				evolvedNN->setWeightAt(l, n, nn, weight / numOfParents);
			}
		}
	}

	for (size_t p = 0; p < population; p++) {
		evolvedNN->cloneNetworkInto(players.at(p)->getNN());
		size_t mods = mt_rand() % 10 + 5;
		for (unsigned long m = 0; m < mods; m++) {
			double noise = noiseDistribution(mt_rand);
			size_t l = mt_rand() % (layers - 1);
			size_t n = mt_rand() % neuronsPerLayer.at(l);
			size_t nn = mt_rand() % neuronsPerLayer.at(l + 1);
			players.at(p)->getNN()->setWeightAt(l, n, nn, players.at(p)->getNN()->getWeightaAt(l, n, nn) + noise);
		}
		players.at(p)->resetMoney();
	}

	delete evolvedNN;
}

std::string ElitistCombination::getAlgorithmType() const {
	return "Elitist Evolution";
}

std::string ElitistCombination::getAlgoDescriptor() const {
	return "elitist";
}

std::string ElitistCombination::getVarsDescriptor() const {
	return "_" + NNEvolver::shortenInt(numOfParents) + "-parents";
}

void ElitistCombination::setEvolutionVars() {
	std::cout << "Enter number of parents: ";
	std::cin >> numOfParents;
}

void ElitistCombination::displayEvolutionVars() {
	std::cout << "Parents: " << numOfParents << std::endl;
}
