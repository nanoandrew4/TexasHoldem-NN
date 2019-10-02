#ifndef TEXAS_HOLDEM_NN_AIPLAYER_H
#define TEXAS_HOLDEM_NN_AIPLAYER_H

#include "NeuralNetwork.h"
#include "../holdem/Player.h"

class AIPlayer : public Player {
public:
	/**
	 * Default constructor, for use when training agents. Agents do not carry a name, and are not loaded from any file.
	 * Instead, they are created randomly and trained.
	 */
	AIPlayer();

	/**
	 * Constructs an agent from a previously serialized agent neural network. This constructor employs the
	 * NeuralNetwork::deserialize() method to read in the data and re-create the agent's network.
	 *
	 * @param agentFile File to load agent data from. File extension should not be omitted
	 */
	explicit AIPlayer(const std::string &agentFile);

	/**
	 * Constructs an agent from a previously serialized agent neural network. This constructor employs the
	 * NeuralNetwork::deserialize() method to read in the data and re-create the agent's network.
	 * Also gives agent a name, which is useful for identifying each agent when playing against it in the console.
	 *
	 * @param agentFile File to load agent data from. File extension should not be omitted
	 * @param agentName Name to assign to this agent
	 */
	AIPlayer(const std::string &agentFile, std::string agentName);

	~AIPlayer();

	/**
	 * Implementation of 'play' function in 'Player' class, designed for smart agents that use a neural network to
	 * determine their next action, based on the information they have about the state of the table and their cards.
	 *
	 * @param tableInfo Information to be used as inputs for the neural network
	 * @return Action taken by agent (-1 -> fold, 0 -> call, x > 0 -> raise by 'x')
	 */
	int play(const std::vector<double> &tableInfo);

	/**
	 * Sets this agent's neural network, and deletes the old one.
	 *
	 * @param nn Neural network to assign to this agent
	 */
	void setNN(NeuralNetwork *nn) {
		delete this->nn;
		this->nn = nn;
	}

	/**
	 * Returns a pointer to this agent's neural network.
	 *
	 * @return Neural network pointer belonging to this agent
	 */
	NeuralNetwork *getNN() { return nn; }

private:
	/**
	 * Determines this hands ability to improve on the next card turn. The value is the average of all possible hand
	 * scores next round, minus the current hand score. This metric is used by the neural network to make a decision.
	 *
	 * @param communityCards Community cards to be used in combination with this players hand to determine
	 *        the hand potential
	 * @return Double value representing the hand potential, which is the average hand score on the next turn minus the
	 *         current hand score
	 */
	double getHandPotential(const std::vector<Card *> &communityCards);

	int getRaise(const int action);

	NeuralNetwork *nn;
};

#endif //TEXAS_HOLDEM_NN_AIPLAYER_H
