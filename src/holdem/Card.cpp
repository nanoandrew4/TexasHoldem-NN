#include "../../headers/holdem/Card.h"

Card::Card(Suit s, int val) {
    this->suit = s;
    this->cardValue = val;
}

char Card::getSuitSymbol() {
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