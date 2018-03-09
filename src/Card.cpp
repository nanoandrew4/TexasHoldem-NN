#include "../headers/Card.h"

Card::Card(Suit s, int val) {
    if (s == Suit::CLUBS || s == Suit::SPADES)
        color = 'b';
    else
        color = 'r';
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