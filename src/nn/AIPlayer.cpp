#include <iostream>
#include "../../headers/nn/AIPlayer.h"
#include "../../headers/holdem/Deck.h"

std::vector<int> AIPlayer::neuronsPerLayer = {5, 25, 3};

AIPlayer::AIPlayer() {
    nn = new NeuralNetwork(neuronsPerLayer, true);
}

AIPlayer::AIPlayer(std::string agentFile) {
    nn = NeuralNetwork::deserialize(agentFile);
}

AIPlayer::~AIPlayer() {
    delete nn;
}

int AIPlayer::play(std::vector<double> tableInfo) {
    int ans = nn->getAction(tableInfo);
    if (ans == -1)
        playing = false;
    return (ans > 1 && ans < tableInfo.at(0)) ? 1 : ans; // Deals with all in and improper raise
}

double AIPlayer::getHandPotential(std::vector<Card*> communityCards) { // TODO: NEEDS TESTING
    double score = hand->getHandScore(communityCards);

    int numOfCards = communityCards.size() + 2;
    if (numOfCards == 7)
        return score;

    std::vector<Card*> cards(numOfCards);
    for (int c = 0; c < 2; c++)
        cards.at(c) = hand->pocket.at(c);
    for (int cc = 0; cc < communityCards.size(); cc++)
        cards.at(cc + 2) = communityCards.at(cc);

    hand->quicksortByVal(cards, 0, numOfCards - 1);

    Deck deck(cards);
    cards.push_back(NULL);

    double newAvgScore = 0;
    for (int c = 0; c < 26; c++) {
        cards.at(numOfCards) = deck.deal();
        newAvgScore += (hand->getHandScore(communityCards) / (double) deck.getCardsInDeck());
    }

    return (newAvgScore - score);
}
