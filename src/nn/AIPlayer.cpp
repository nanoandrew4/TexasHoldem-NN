#include <iostream>
#include <algorithm>
#include "../../headers/nn/AIPlayer.h"
#include "../../headers/holdem/Deck.h"
#include "../../headers/holdem/Table.h"

AIPlayer::AIPlayer() {
	nn = new NeuralNetwork(true);
}

AIPlayer::AIPlayer(std::string agentFile) {
	nn = NeuralNetwork::deserialize(agentFile);
}

AIPlayer::AIPlayer(std::string agentFile, std::string agentName) {
	nn = NeuralNetwork::deserialize(agentFile);
	this->name = std::move(agentName);
}

AIPlayer::~AIPlayer() {
	delete nn;
}

int AIPlayer::play(std::vector<double> tableInfo) {
	int action = nn->getAction(tableInfo);
	if ((action == -1 && !allIn) || (money == 0 && !allIn)) {
		playing = false;
		if (Table::output)
			std::cout << "AI FOLDED" << std::endl;
		return -1;
	} else if (action == 0 || action <= tableInfo.at(0) || money < action || allIn) {
		if (Table::output)
			std::cout << "AI CHECKED" << std::endl;
		return 0;
	} else if (action >= 1 && action > tableInfo.at(0) && money >= action) {
		if (Table::output)
			std::cout << "AI RAISED" << std::endl;
		return action;
	}
	exit(1);
}

double AIPlayer::getHandPotential(std::vector<Card *> communityCards) {
	double currScore = hand.getHandScore(communityCards);

	unsigned long numOfCards = communityCards.size() + 2, cardsToCheck = 26;
	if (numOfCards == 7)
		return currScore;

	std::vector<Card *> cards(numOfCards);
	for (size_t c = 0; c < 2; c++)
		cards.at(c) = hand.pocket.at(c);
	for (size_t cc = 0; cc < numOfCards - 2; cc++)
		cards.at(cc + 2) = communityCards.at(cc);

	std::sort(cards.begin(), cards.end(), [](Card *left, Card *right) -> bool {
		return left->getCardValue() < right->getCardValue();
	});

	Deck deck(cards);
	cards.push_back(nullptr);

	double avgPotentialScore = 0;
	for (int c = 0; c < cardsToCheck; c++) { // Don't check all hands, or performance drops exponentially.
		cards.at(numOfCards) = deck.deal();
		avgPotentialScore += (hand.getHandScore(communityCards));
	}

	return ((avgPotentialScore / (deck.getCardsInDeck() * cardsToCheck)) - currScore);
}