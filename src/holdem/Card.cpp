#include <cstdlib>
#include <iostream>
#include "../../headers/holdem/Card.h"

Card::Card(Suit s, int val) {
    if (val > 14 || val < 2 || s < 0 || s > 3) {
        std::cout << "Bad card data" << std::endl;
        exit(1);
    }
    this->suit = s;
    this->cardValue = val;
}

char Card::getSuitSymbol() const {
    switch (suit) {
        case Suit::SPADES:
            return 'S';
        case Suit::CLUBS:
            return 'C';
        case Suit::DIAMONDS:
            return 'D';
        case Suit::HEARTS:
            return 'H';
        default:
            return '?';
    }
}