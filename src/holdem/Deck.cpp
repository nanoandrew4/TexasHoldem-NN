#include <iostream>
#include "../../headers/holdem/Deck.h"

static std::mt19937_64 mt_rand((ulong) std::chrono::high_resolution_clock::now().time_since_epoch().count());

Deck::Deck() {
    for (int c = 0; c < 52; c++) deck[c] = new Card((Suit)((c / 13)), (c % 13) + 2);
    shuffle();
}

Deck::~Deck() {
    for (auto &c : deck) delete c;
}

void Deck::shuffle() {
    for (int i = 0; i < 200; i++) {
        ulong oldPos = mt_rand() % 52;
        ulong newPos = mt_rand() % 52;
        Card* tmp = deck[oldPos];
        deck[oldPos] = deck[newPos];
        deck[newPos] = tmp;
    }
}

Card* Deck::deal() {
    return deck[pos--];
}