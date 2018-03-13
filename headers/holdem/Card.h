#ifndef TEXAS_HOLDEM_NN_CARD_H
#define TEXAS_HOLDEM_NN_CARD_H

enum Suit {
    CLUBS, SPADES, HEARTS, DIAMONDS
};

/**
 * Contains info that a physical card would. A value between 2-14 (2-Ace) and a suit to which the card belongs.
 */
class Card {
public:
    Suit suit;
    int cardValue;

    Card(Suit, int);

    /**
     * Returns a symbol representing the suit of the card, which is the first letter of the name of the suit.
     * @return First letter of the suit name
     */
    char getSuitSymbol();
};


#endif //TEXAS_HOLDEM_NN_CARD_H
