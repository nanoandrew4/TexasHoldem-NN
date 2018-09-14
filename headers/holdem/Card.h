#ifndef TEXAS_HOLDEM_NN_CARD_H
#define TEXAS_HOLDEM_NN_CARD_H

enum Suit {
    CLUBS, SPADES, HEARTS, DIAMONDS
};

/**
 * Contains info that a physical card would.
 * A value between [2-14] ([2-Ace]) and a suit to which the card belongs.
 */
class Card {
public:

    /**
     * Creates a new card with the passed values.
     *
     * @param suit Suit the new card belongs to (see Suit enum)
     * @param cardVal Value of card ([2-14])
     */
    Card(Suit suit, unsigned int cardVal);

    /**
     * Returns the first letter of the suit name assigned to this card.
     *
     * @return Character of first letter of the suit name
     */
    char getSuitSymbol() const;

    /**
     * Returns the value assigned to this card.
     *
     * @return Integer representing value assigned to this card.
     */
    unsigned int getCardValue() const { return cardValue; }

    /**
     * Returns the suit assigned to this card.
     *
     * @return Suit (see Suit enum) assigned to this card
     */
    Suit getSuit() const { return suit; }

private:
    Suit suit;
    unsigned int cardValue;
};

#endif //TEXAS_HOLDEM_NN_CARD_H
