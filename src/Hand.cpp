#include <iostream>
#include "../headers/Hand.h"

Hand::Hand(Card* c1, Card* c2) {
    this->cards[0] = c1; this->cards[1] = c2;
}

Hand::~Hand() {
    for (auto &card : cards) delete card;
}

void Hand::displayHand() {
    std::cout << cards[0]->cardValue << cards[0]->getSuitSymbol() << " | "
              << cards[1]->cardValue << cards[1]->getSuitSymbol() << std::endl;
}