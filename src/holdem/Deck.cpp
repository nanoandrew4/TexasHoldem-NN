#include <iostream>
#include "../../headers/holdem/Deck.h"

static std::mt19937_64 mt_rand((ulong) std::chrono::high_resolution_clock::now().time_since_epoch().count());

Deck::Deck() {
    for (int c = 0; c < 52; c++) deck.emplace_back(Card((Suit) ((c / 13)), (c % 13) + 2));
    pos = deck.size();
    shuffle();
}

Deck::Deck(std::vector<Card *> cards) {
    int cardsEliminated = 0;

    for (int c = 51; c >= 0; c--) {
        if (cardsEliminated >= cards.size() || cards.at(cardsEliminated)->getCardValue() != (c % 13) + 2 ||
            cards.at(cardsEliminated)->getSuit() != (Suit) ((c / 13)))
            deck.emplace_back((Suit) ((c / 13)), (c % 13) + 2);
        else
            cardsEliminated++;
    }

    // Remove any that were not caught in the initialization
    while (cardsEliminated != cards.size()) {
        for (int c = deck.size() - 1; c >= 0 && cardsEliminated != cards.size(); c--)
            if (cards.at(cardsEliminated)->getCardValue() == deck.at(c).getCardValue() &&
                cards.at(cardsEliminated)->getSuit() == deck.at(c).getSuit()) {
                deck.erase(deck.begin() + c);
                cardsEliminated++;
            }
    }

    pos = deck.size();
    shuffle();
}

Deck::~Deck() = default;

void Deck::shuffle() {
    for (int i = 0; i < deck.size(); i++) {
        ulong newPos = mt_rand() % deck.size();
        Card tmp = deck.at(i);
        deck.at(i) = deck.at(newPos);
        deck.at(newPos) = tmp;
    }
}

Card *Deck::deal() {
    // To prevent breaking, decks can be reused. Re-shuffle and reset pos if deck "runs out"
    if (pos == 0) {
        shuffle();
        pos = deck.size();
    }

    return &deck.at(--pos);
}
