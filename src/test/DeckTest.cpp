#include <vector>
#include <iostream>
#include <algorithm>
#include "../../headers/test/DeckTest.h"
#include "../../headers/holdem/Deck.h"

void DeckTest::test() {
    srand(time(NULL));
    std::vector<int> deckCheck;

    // Create decks and check that all cards are present after shuffling (done in Deck constructor).

    for (int i = 0; i < 1000; i++) {
        deckCheck.resize(52, 0);

        Deck d1;
        for (int c = 0; c < d1.getCardsInDeck(); c++) {
            Card *card = d1.deal();
            deckCheck.at(card->getCardValue() - 2 + card->getSuit() * 13)++;
        }
        for (int c : deckCheck)
            if (c != 1) {
                std::cout << "Deck -> Shuffle integrity test failed on pass " << i << std::endl;
                return;
            }
        deckCheck.clear();
    }

    // Draw a random number of cards from a deck, and exclude them when creating another deck.

    for (int i = 0; i < 1000; i++) {
        deckCheck.resize(52, 0);

        Deck d1;
        std::vector<Card *> excluded;

        int numExcluded = rand() % 5 + 5;
        for (int c = 0; c < numExcluded; c++)
            excluded.push_back(d1.deal());

        std::sort(excluded.begin(), excluded.end(), [](Card *left, Card *right) -> bool {
            return left->getCardValue() < right->getCardValue();
        });
        Deck d2(excluded);
        for (int c = 0; c < d2.getCardsInDeck(); c++) {
            Card *card = d2.deal();
            deckCheck.at(card->getCardValue() - 2 + card->getSuit() * 13)++;
        }

        for (auto &c : excluded)
            if (deckCheck.at(c->getCardValue() - 2 + c->getSuit() * 13) == 1) {
                std::cout << "Deck -> Exclusion integrity test failed on pass " << i << std::endl;
                return;
            }

        deckCheck.clear();
    }

    deckCheck.resize(52, 0);

    // Check that a deck can deal more than 52 cards by re-shuffling itself. Simulate dealing 104 cards using one deck.

    Deck d1;
    for (int c = 0; c < d1.getCardsInDeck() * 2; c++) {
        Card *card = d1.deal();
        deckCheck.at(card->getCardValue() - 2 + card->getSuit() * 13)++;
    }

    for (int c = 0; c < deckCheck.size(); c++)
        if (deckCheck.at(2) != 2) {
            std::cout << "Deck -> Deck reuse integrity test failed" << std::endl;
            return;
        }
}
