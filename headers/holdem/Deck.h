#ifndef TEXAS_HOLDEM_NN_DECK_H
#define TEXAS_HOLDEM_NN_DECK_H

#include <random>
#include <chrono>
#include "Hand.h"

/**
 * Contains 52 cards, like a regular deck, with 13 cards per suit.
 */
class Deck {
public:
    /**
     * Fills deck with the cards found in a standard deck (52 cards).
     */
    explicit Deck(bool autoReShuffle = true);

    /**
     * Fills deck with cards found in a standard deck, but excludes those contained
     * in the 'cards' vector passed.
     *
     * @param cards Cards to be excluded from deck
     */
    explicit Deck(std::vector<Card *> cards);

    ~Deck();

    /**
     * Returns a pointer to the card on the top of the deck, and decreases 'pos',
     * simulating the card being removed.
     *
     * If the deck runs out of cards, it will shuffle itself and reset 'pos', allowing
     * for one deck instance to be used indefinitely.
     *
     * @return Pointer to card dealt
     */
    Card *deal();

    /**
     * Returns the number of cards in the deck (initially).
     *
     * @return Integer representation of size of deck
     */
    int getCardsInDeck() const { return (int) deck.size(); }

private:
    std::vector<Card> deck;

    // Position to read card value from (allows simulating cards being drawn)
    int pos;

    bool autoReShuffle;

    // Shuffles the deck, by randomly swapping cards.
    void shuffle();
};

#endif //TEXAS_HOLDEM_NN_DECK_H