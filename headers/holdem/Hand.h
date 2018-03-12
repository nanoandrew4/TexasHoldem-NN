#ifndef TEXAS_HOLDEM_NN_HAND_H
#define TEXAS_HOLDEM_NN_HAND_H

#include "Card.h"

enum Hands {
    STR_FLUSH, FOUR_OAK, FH, FLUSH, STRT, THREE_OAK, TWO_PAIR, PAIR, HC
};

class Hand {
private:
    void quicksortByVal(Card*[], int, int);

    int* sameVal(Card*[], int[]);
    int* straight(Card*[], int[]);
    int* flush(Card*[], int[], int[]);

public:
    Card* pocket[2];
    Hand(Card* c1, Card* c2);
    ~Hand();

    void displayHand(Card*[], int numOfCards = 2);

    void score(int, Card*[], int[]);
};


#endif //TEXAS_HOLDEM_NN_HAND_H
