#include <iostream>
#include "../../headers/holdem/Hand.h"

Hand::Hand(Card* c1, Card* c2) {
    this->cards[0] = c1; this->cards[1] = c2;
}

Hand::~Hand() {
    for (auto &card : cards) delete card;
}

void Hand::displayHand() {
    std::cout << cards[0]->cardValue << cards[0]->getSuitSymbol() << " | "
              << cards[1]->cardValue << cards[1]->getSuitSymbol() << std::endl;
}

int* Hand::score(uint playerNum, Card* flop[]) {
    // Check for flush, straight and # of cards. All else can be determined from this

    int topHand[9] = {0};
    topHand[0] = playerNum;

    Card* cards[7];
    cards[0] = this->cards[0];
    cards[1] = this->cards[1];
    for (int f = 0; f < 5; f++)
        cards[f + 2] = flop[f];

    quicksortByVal(cards, 7, 0, 7);

    int* straightRes = straight(cards);
    int* flushRes = flush(cards);
    int* sameValRes = sameVal(cards);

    if (straightRes[0] == 1 && flushRes[0] == 1) { // Verify straight flush (and royal flush)
        for (int c = 0; c <= 2; c++) {
            if (cards[c]->cardValue == straightRes[1] && ((c == 0 && flushRes[6] == 0 && flushRes[7] == 0) ||
               (c == 1 && flushRes[0] == 0 && flushRes[7] == 0) || (c == 2 && flushRes[0] == 0 && flushRes[1] == 0))) {
                topHand[1] = STR_FLUSH;
                topHand[2] = cards[c]->cardValue;
            }
        }
    } else if (sameValRes[0] == 4) { // 4 of a kind
        topHand[1] = FOUR_OAK;
        topHand[2] = sameValRes[1]; // Value of four of a kind
        topHand[3] = sameValRes[2]; // Kicker
    } else if (sameValRes[0] == 32) { // Full house
        topHand[1] = FH;
        topHand[2] = sameValRes[1]; // Value of three of a kind
        topHand[3] = sameValRes[2]; // Value of pair
    } else if (flushRes[0] == 1) { // Flush
        topHand[1] = FLUSH;
        for (int c = 0; c < 7; c++)
            topHand[c + 2] = cards[c]->cardValue; // Copy all card values in case multiple players have a flush
    } else if (straightRes[0] == 1) { // Straight
        topHand[1] = STRT;
        topHand[2] = straightRes[1]; // Highest card in the straight
    } else if (sameValRes[0] == 3) { // Three of a kind
        topHand[1] = THREE_OAK;
        for (int svr = 1; svr < 4; svr++) // Copy over what the value of the match is, along with two kickers
            topHand[svr + 1] = sameValRes[svr];
    } else if (sameValRes[0] == 22) { // Two pairs
        topHand[1] = TWO_PAIR;
        for (int svr = 1; svr < 5; svr++) // Copy over values of two pairs, and two kickers
            topHand[svr + 1] = sameValRes[svr];
    } else if (sameValRes[0] == 2) { // Pair
        topHand[1] = PAIR;
        for (int svr = 1; svr < 5; svr++) // Copy over value of pair and three kickers
            topHand[svr + 1] = sameValRes[svr];
    } else { // High card
        topHand[1] = HC;
        for (int c = 0; c < 7; c++) // Copy over all cards, for comparison
            topHand[c + 2] = cards[c]->cardValue;
    }

    return topHand;
}

int* Hand::sameVal(Card* cards[]) {
    // Pairs check
    int results[6] = {0};
    int vals[13] = {0};

    for (int c = 0; c < 7; c++)
        vals[cards[c]->cardValue]++;

    int coi[2] {0};

    for (int c = 1; c < 13; c++) {
        for (int cois = 0; cois < 2; cois++) {
            if (vals[c] >= vals[coi[cois]]) {
                if (cois == 0) {
                    coi[1] = coi[0];
                    coi[0] = c;
                } else
                    coi[1] = c;
            }
        }
    }

    int kickers[3] = {0};

    if (coi[0] == 4)
        coi[1] = coi[0];

    if (coi[0] != 3 && coi[1] != 2) {
        int k = 0;
        for (int c = 0; c < 7; c++)
            if (cards[c]->cardValue != coi[0] && cards[c]->cardValue != coi[1])
                kickers[k++] = cards[c]->cardValue;
    }

    if (vals[coi[0]] == 2 && vals[coi[1]] != 2) { // Pair
        results[0] = 2;
        results[1] = coi[0];
        for (int k = 0; k < 3; k++)
            results[2 + k] = kickers[k];
    } else if (vals[coi[0]] == 2 && vals[coi[1]] == 2) { // Two pair
        results[0] = 22;
        results[1] = coi[0];
        results[2] = coi[1];
        for (int k = 0; k < 2; k++)
            results[3 + k] = kickers[k];
    } else if (vals[coi[0]] == 3 && vals[coi[1]] != 2) { // Three of a kind
        results[0] = 3;
        results[1] = coi[0];
        for (int k = 0; k < 2; k++)
            results[2 + k] = kickers[k];
    } else if (vals[coi[0]] == 3 && vals[coi[1]] == 2) { // Full house
        results[0] = 32;
        results[1] = coi[0];
        results[2] = coi[1];
    } else if (vals[coi[0]] == 4) { // Four of a kind
        results[0] = 4;
        results[1] = coi[0];
        results[2] = kickers[0];
    }

    return results;
}

/**
 * Determines if a hand (pocket + community cards) has a straight in it. If it does, it returns an array containing a 1
 * as it's first element, and the straight after that. If there is no straight, an array of 0's is returned.
 *
 * @param cards Cards to be evaluated
 * @return Array representing straight and indicating that one exists
 */
int* Hand::straight(Card* cards[]) {
    // Straight check

    int results[6] = {0};

    int seq = 0;
    for (int v = 1; v < 7; v++) { // TODO: ACE AS 1
        if (cards[v - 1]->cardValue - cards[v]->cardValue == 1)
            seq++;
        else
            seq = 0;
        if (seq > 4) {
            results[0] = 1;
            for (int c = v - 5; c < v; c++)
                results[c + 1] = cards[c]->cardValue;
            break;
        }
    }

    return results;
}

int* Hand::flush(Card* cards[]) {
    // Flush check

    int results[8] = {0};
    int suits[4] = {0};

    for (int c = 0; c < 7; c++) {
        suits[cards[c]->suit]++;
        results[c + 1] = cards[c]->suit;
    }

    for (int s = 0; s < 4; s++) {
        if (suits[s] > 4) {
            results[0] = 1;
            for (int c = 0; c < 7; c++) {
                if (cards[c]->suit == s)
                    results[c + 1] = 1;
                else
                    results[c + 1] = 0;
            }
        }
    }

    return results;
}

void Hand::quicksortByVal(Card* cards[], uint size, uint lPiv, uint rPiv) {
    int pivot = size / 2;
    uint a = lPiv, b = rPiv;

    while (a <= b) {
        while (cards[a] < cards[pivot])
            a++;
        while (cards[b] > cards[pivot])
            b--;
        if (a <= b) {
            Card* c = cards[a];
            cards[a] = cards[b];
            cards[b] = c;
            a++;
            b--;
        }
    }

    if (lPiv < b)
        quicksortByVal(cards, size, lPiv, b);
    if (a < rPiv)
        quicksortByVal(cards, size, lPiv, b);

}