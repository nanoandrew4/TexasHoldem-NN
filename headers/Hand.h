#ifndef TEXAS_HOLDEM_NN_HAND_H
#define TEXAS_HOLDEM_NN_HAND_H

#include "Card.h"

class Hand {
public:
    Card* cards[2];
    Hand(Card* c1, Card* c2);
    ~Hand();

    void displayHand();
};


#endif //TEXAS_HOLDEM_NN_HAND_H
