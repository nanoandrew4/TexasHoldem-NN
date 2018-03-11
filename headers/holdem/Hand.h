#ifndef TEXAS_HOLDEM_NN_HAND_H
#define TEXAS_HOLDEM_NN_HAND_H

#include "Card.h"

enum Hands {
    STR_FLUSH, FOUR_OAK, FH, FLUSH, STRT, THREE_OAK, TWO_PAIR, PAIR, HC
};

class Hand {
private:
    void quicksortByVal(Card*[], uint, uint, uint);

    int* sameVal(Card*[]);
    int* straight(Card*[]);
    int* flush(Card*[]);

public:
    Card* cards[2];
    Hand(Card* c1, Card* c2);
    ~Hand();

    void displayHand();

    int* score(uint, Card*[]);
};


#endif //TEXAS_HOLDEM_NN_HAND_H
