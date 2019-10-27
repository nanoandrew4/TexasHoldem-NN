#ifndef TEXAS_HOLDEM_NN_GENETICALGORITHM_H
#define TEXAS_HOLDEM_NN_GENETICALGORITHM_H

#include <iostream>
#include <sstream>
#include <memory>
#include "../nn/AIPlayer.h"

class GeneticAlgorithm {
public:
	GeneticAlgorithm() = default;

	virtual ~GeneticAlgorithm() = default;

	/**
	 * Evolution algorithm. Each implementation of 'GeneticAlgorithm' should have its own documentation regarding the
	 * specific implementation.
	 *
	 * @param players Vector of players (population)
	 * @param lastGen True if this is the last generation to be evolved, false otherwise
	 */
	virtual void evolve(const std::vector<AIPlayer *> &players) const = 0;

	/**
	 * Returns the name of the evolutionary algorithm an implementation of this class uses. Useful for displaying to
	 * the user when training starts, alongside other variables.
	 *
	 * @return Name of the evolutionary algorithm
	 */
	[[nodiscard]] virtual std::string getAlgorithmType() const = 0;

	/**
	 * Returns a string that is attached to the output file name, representing the algorithm that was used. This is
	 * usually a shortened and lower case version of the 'getAlgorithmType' function. This is used as an identifier
	 * when determining the name of the file that the best agent will be written to.
	 *
	 * @return String identifier of the algorithm that was used for evolution
	 */
	[[nodiscard]] virtual std::string getAlgoDescriptor() const = 0;

	/**
	 * Returns a string that is attached to the output file name, representing the variables that were used during
	 * evolution. It is used as an identifier, that goes after all the global evolution options (such as population)
	 * so that when testing each agent, the conditions under which it evolved are obviously apparent. The names of the
	 * variables are shortened, and the values they represent are too, to minimize the length of the name the file will
	 * take.
	 *
	 * @return String identifier of the variables and values used for evolution
	 */
	[[nodiscard]] virtual std::string getVarsDescriptor() const = 0;

	/**
	 * Called right after initialization of the 'GeneticAlgorithm' implementation, sets up the algorithm specific
	 * variables by requesting input from the user.
	 */
	virtual void setEvolutionVars() = 0;

	/**
	 * Displays the variables, and respective values, that are to be used by the evolution algorithm.
	 */
	virtual void displayEvolutionVars() = 0;
};

#endif //TEXAS_HOLDEM_NN_GENETICALGORITHM_H
