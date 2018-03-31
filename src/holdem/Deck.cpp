#include <iostream>
#include "../../headers/holdem/Deck.h"

static std::mt19937_64 mt_rand((ulong) std::chrono::high_resolution_clock::now().time_since_epoch().count());

Deck::Deck() {
    for (int c = 0; c < 52; c++) deck.emplace_back(Card((Suit)((c / 13)), (c % 13) + 2));
    shuffle();
}

Deck::Deck(std::vector<Card*> cards) {
    int cardsEliminated = 0;
    for (int c = 0; c < 52; c++) {
        if (cardsEliminated >= cards.size()|| cards.at(cardsEliminated)->getCardValue() != (c % 13) + 2 || cards.at(cardsEliminated)->getSuit() != (Suit)((c / 13)))
            deck.emplace_back((Suit) ((c / 13)), (c % 13) + 2);
        else
            cardsEliminated++;
    }

//    for (int i = cardsEliminated; i < cards.size(); i++)
//        std::cout << cards.at(i)->getCardValue() << cards.at(i)->getSuitSymbol() << std::endl;
    while (cardsEliminated != cards.size()) { // TODO: OPTIMIZE
        for (int c = deck.size() - 1; c >= 0 && cardsEliminated != cards.size(); c--) {
//            std::cout << cards.at(cardsEliminated)->getCardValue() << cards.at(cardsEliminated)->getSuitSymbol() << std::endl;
//            std::cout << deck.at(c).getCardValue() << deck.at(c).getSuitSymbol() << std::endl << std::endl;
            if (cards.at(cardsEliminated)->getCardValue() == deck.at(c).getCardValue() && cards.at(cardsEliminated)->getSuit() == deck.at(c).getSuit()) {
                deck.erase(deck.begin() + c);
                cardsEliminated++;
            }
        }
    }

    pos -= deck.size() - 1;
    shuffle();
}

Deck::~Deck() {}

void Deck::shuffle() {
    for (int i = 0; i < 200; i++) {
        ulong oldPos = mt_rand() % deck.size();
        ulong newPos = mt_rand() % deck.size();
        Card tmp = deck.at(oldPos);
        deck.at(oldPos) = deck.at(newPos);
        deck.at(newPos) = tmp;
    }
}

Card* Deck::deal() {
    if (pos == 0) {
        shuffle();
        pos = deck.size() - 1;
    }
    return &deck.at(pos--);
}