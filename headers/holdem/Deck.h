#ifndef TEXAS_HOLDEM_NN_DECK_H
#define TEXAS_HOLDEM_NN_DECK_H

#include <random>
#include <chrono>
#include "Hand.h"

class Deck {
private:
    Card* deck[52];
    int pos = 0;
    void shuffle();

public:
    Deck();
    ~Deck();
    Card* deal();
};


#endif //TEXAS_HOLDEM_NN_DECK_H
