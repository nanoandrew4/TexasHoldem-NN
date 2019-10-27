#ifndef TEXAS_HOLDEM_NN_ELITISTCOMBINATION_H
#define TEXAS_HOLDEM_NN_ELITISTCOMBINATION_H

#include "GeneticAlgorithm.h"

class ElitistCombination : public GeneticAlgorithm {
public:
	ElitistCombination() = default;

	ElitistCombination(unsigned long parents) { this->numOfParents = parents; };

	/**
	 * Combines the parent players to generate the next generation agent. All other players copy that agent and apply
	 * some random noise to it, in order to keep evolution from stagnating. The number of parents is specified when
	 * initializing an 'ElitistCombination' instance, and they represent the best scoring players of the population.
	 * Each of the weights in their networks are averaged, and a new network with these averaged values is created.
	 * All players copy the new network's values, and apply some noise to prevent stagnation.
	 *
	 * @param players Vector containing all agents
	 * @param lastGen True if this is the last generation to be evolved, false otherwise
	 */
	void evolve(const std::vector<AIPlayer *> &players) const;

	std::string getAlgorithmType() const;

	std::string getAlgoDescriptor() const;

	std::string getVarsDescriptor() const;

	void setEvolutionVars();

	void displayEvolutionVars();

private:
	unsigned long numOfParents;

	static std::mt19937_64 mt_rand;

	static std::uniform_real_distribution<> noiseDistribution;
};

#endif //TEXAS_HOLDEM_NN_ELITISTCOMBINATION_H
