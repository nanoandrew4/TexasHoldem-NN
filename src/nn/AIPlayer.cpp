#include <iostream>
#include "../../headers/nn/AIPlayer.h"
#include "../../headers/holdem/Deck.h"

std::vector<int> AIPlayer::neuronsPerLayer = {5, 25, 3};

AIPlayer::AIPlayer() {
    nn = new NeuralNetwork(neuronsPerLayer, true);
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
    std::vector<int> topHand(9);
    hand->recordBestHand(0, communityCards, topHand);

    double score = topHand[1];
    for (int i = 2; i < 9; i++)
        score += (topHand[i] / 15) / pow(10, i);

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
    for (int c = 0; c < deck.getCardsInDeck(); c++) {
        cards.at(numOfCards) = deck.deal();
        hand->recordBestHand(0, communityCards, topHand);
        double tmpScore = topHand[1];
        for (int i = 2; i < 9; i++)
            tmpScore += (topHand[i] / 15) / pow(10, i);
        newAvgScore += (tmpScore / deck.getCardsInDeck());
    }

    return (newAvgScore - score);
}