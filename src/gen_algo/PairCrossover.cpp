#include "../../headers/gen_algo/PairCrossover.h"

std::mt19937_64 PairCrossover::mt_rand(std::random_device().operator()());

std::uniform_real_distribution<> PairCrossover::mutationEffectDistribution(-0.05, 0.05);

void PairCrossover::evolve(const std::vector<AIPlayer *> &players) const {
	const size_t population = players.size();

	std::array<std::uint8_t, NeuralNetwork::numOfLayers> neuronsPerLayer = NeuralNetwork::getNeuronsPerLayer();
	for (size_t pop = 0; pop < population - 1; pop += 2) {
		for (size_t layer = 0; layer < neuronsPerLayer.size() - 1; layer++) {
			const size_t neuronsInLayer = neuronsPerLayer.at(layer);
			for (size_t neuron = 0; neuron < neuronsInLayer; neuron++) {
				const size_t neuronsInNextLayer = neuronsPerLayer.at(layer + 1);
				for (size_t nextLayerNeuron = 0; nextLayerNeuron < neuronsInNextLayer; nextLayerNeuron += 2) {
					double connSwapNetwork1 = players.at(pop)->getNN()->getWeightaAt(layer, neuron, nextLayerNeuron);
					double connSwapNetwork2 = players.at(pop + 1)->getNN()->getWeightaAt(layer, neuron,
					                                                                     nextLayerNeuron);
					if (mt_rand() % 1000 < 20) // % chance of mutation
						connSwapNetwork1 += mutationEffectDistribution(mt_rand);
					if (mt_rand() % 1000 < 20) // % chance of mutation
						connSwapNetwork2 += mutationEffectDistribution(mt_rand);
					if (mt_rand() % 1000 < 700) { // % chance of crossover
						players.at(pop)->getNN()->setWeightAt(layer, neuron, nextLayerNeuron, connSwapNetwork2);
						players.at(pop + 1)->getNN()->setWeightAt(layer, neuron, nextLayerNeuron, connSwapNetwork1);
					}
				}
			}
		}
		players.at(pop)->resetMoney();
		players.at(pop + 1)->resetMoney();
	}
	players.back()->resetMoney();
}

std::string PairCrossover::getAlgorithmType() const {
	return "Pair crossover";
}

std::string PairCrossover::getAlgoDescriptor() const {
	return "p-cross";
}

std::string PairCrossover::getVarsDescriptor() const {
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
