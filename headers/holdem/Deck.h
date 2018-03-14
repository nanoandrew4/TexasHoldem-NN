#ifndef TEXAS_HOLDEM_NN_DECK_H
#define TEXAS_HOLDEM_NN_DECK_H

#include <random>
#include <chrono>
#include "Hand.h"

/**
 * Contains 52 cards, like a regular deck, with 13 cards per suit.
 */
class Deck {
private:
    Card* deck[52];

    // Position to read card value from (allows simulating that the deck size decreases when a card is drawn)
    int pos = 51;

    // Shuffles the deck, by randomly swapping cards.
    void shuffle();

public:
    // Fills deck with 52 cards, one of each suit and value
    Deck();

    // Cleans up deck array
    ~Deck();

    /**
     * Returns a pointer to the card on the top of the deck, and decreases 'pos', simulating the card being removed.
     * @return Pointer to card on top of the deck
     */
    Card* deal();
};


#endif //TEXAS_HOLDEM_NN_DECK_H
