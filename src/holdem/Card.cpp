#include <cstdlib>
#include <iostream>
#include "../../headers/holdem/Card.h"

Card::Card(Suit suit, int cardVal) {
    // Force quit if erroneous card data is encountered
    if (cardVal > 14 || cardVal < 2) {
        std::cout << "Bad card value: " << cardVal << std::endl;
        exit(1);
    }

    if ( suit < 0 || suit > 3) {
        std::cout << "Bad card suit: " << suit << std::endl;
        exit(1);
    }

    this->suit = suit;
    this->cardValue = cardVal;
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