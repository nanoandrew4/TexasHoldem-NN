#ifndef TEXAS_HOLDEM_NN_CARD_H
#define TEXAS_HOLDEM_NN_CARD_H

enum Suit {
    CLUBS, SPADES, HEARTS, DIAMONDS
};

class Card {
public:
    Suit suit;
    int cardValue;
    char color;

    Card(Suit, int);

    char getSuitSymbol();
};


#endif //TEXAS_HOLDEM_NN_CARD_H
