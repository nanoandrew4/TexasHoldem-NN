#include <iostream>
#include "../../headers/holdem/Hand.h"

Hand::Hand(Card* c1, Card* c2) {
    this->pocket[0] = c1; this->pocket[1] = c2;
}

Hand::~Hand() {
    for (auto &card : pocket) delete card;
}

/**
 * Displays an array of card pointers, so the user can see what is going on.
 * Note that all values represented in code are shifted one to the left, so a value of 2 actually represents a 3. This
 * is done to enable wrapping of the Ace around.
 *
 * @param cards Array of pointers to cards to display
 * @param numOfCards Number of cards to be displayed
 */
void Hand::displayHand(Card* cards[], int numOfCards) {
    for (int c = 0; c < numOfCards; c++) {
        if (cards[c]->cardValue > 10) {
            if (cards[c]->cardValue == 11)
                std::cout << "J";
            else if (cards[c]->cardValue == 12)
                std::cout << "Q";
            else if (cards[c]-> cardValue == 13)
                std::cout << "K";
            else if (cards[c]->cardValue == 14)
                std::cout << "A";
            std::cout << cards[c]->getSuitSymbol() << (numOfCards - c != 1 ? " | " : "\n");
        } else
            std::cout << cards[c]->cardValue << cards[c]->getSuitSymbol() << (numOfCards - c != 1 ? " | " : "\n");
    }
}

void Hand::score(int playerNum, Card* flop[], int topHand[]) {
    // Check for flush, straight and # of cards. All else can be determined from this
    topHand[0] = playerNum;

    Card* cards[7];
    cards[0] = this->pocket[0];
    cards[1] = this->pocket[1];
    for (int f = 0; f < 5; f++)
        cards[f + 2] = flop[f];

    quicksortByVal(cards, 0, 6);

    std::cout << "Evaluating: ";
    displayHand(cards, 7);

    int straightRes[6] = {0};
    straight(cards, straightRes);
    int flushRes[8] = {0};
    flush(cards, straightRes, flushRes);

//    for (int i = 0; i < 8; i++)
//        std::cout << flushRes[i] << " ";
//    std::cout << std::endl;
//    for (int i = 0; i < 6; i++)
//        std::cout << straightRes[i] << " ";
//    std::cout << std::endl;

    int sameValRes[6] = {0};
    sameVal(cards, sameValRes);

    if (straightRes[0] == 1 && flushRes[0] == 1) { // Verify straight flush (and royal flush)
        for (int c = 0; c < 4; c++) {
            if (cards[c]->cardValue == straightRes[1]) {
                int matches = 0;
                for (int cc = 0; cc < 7; cc++)
                    if (cards[cc]->cardValue == straightRes[matches++ + 1] && cards[cc]->suit == flushRes[cc + 1])
                        matches++;
                if (matches >= 5) {
                    topHand[1] = STR_FLUSH;
                    topHand[2] = cards[c]->cardValue;
                    std::cout << "Straight flush | High: " << cards[c]->cardValue << std::endl;
                    return;
                }
            }
        }
    }

    if (sameValRes[0] == 4) { // 4 of a kind
        topHand[1] = FOUR_OAK;
        topHand[2] = sameValRes[1]; // Value of four of a kind
        topHand[3] = sameValRes[2]; // Kicker
        std::cout << "Four of a kind | High " << sameValRes[1] << " | Kicker: " << sameValRes[2] << std::endl;
    } else if (sameValRes[0] == 32) { // Full house
        topHand[1] = FH;
        topHand[2] = sameValRes[1]; // Value of three of a kind
        topHand[3] = sameValRes[2]; // Value of pair
        std::cout << "Full house | High TOAK " << sameValRes[1] << " | High pair: " << sameValRes[2] << std::endl;
    } else if (flushRes[0] == 1) { // Flush
        topHand[1] = FLUSH;
        for (int c = 0; c < 7; c++)
            topHand[c + 2] = cards[c]->cardValue; // Copy all card values in case multiple players have a flush
        std::cout << "Flush" << std::endl;
    } else if (straightRes[0] == 1) { // Straight
        topHand[1] = STRT;
        topHand[2] = straightRes[1]; // Highest card in the straight
        std::cout << "Straight | High in straight: " << straightRes[1] << std::endl;
    } else if (sameValRes[0] == 3) { // Three of a kind
        topHand[1] = THREE_OAK;
        for (int svr = 1; svr < 4; svr++) // Copy over what the value of the match is, along with two kickers
            topHand[svr + 1] = sameValRes[svr];
        std::cout << "Three of a kind | High: " << sameValRes[1] << " | Kicker: " << sameValRes[2] << " | Second kicker: " << sameValRes[3] << std::endl;
    } else if (sameValRes[0] == 22) { // Two pairs
        topHand[1] = TWO_PAIR;
        for (int svr = 1; svr < 5; svr++) // Copy over values of two pairs, and two kickers
            topHand[svr + 1] = sameValRes[svr];
        std::cout << "Two pairs | High pair: " << sameValRes[1] << " | Low pair: " << sameValRes[2] << " | Kicker: " << sameValRes[3] << " | Second kicker: " << sameValRes[4] << std::endl;
    } else if (sameValRes[0] == 2) { // Pair
        topHand[1] = PAIR;
        for (int svr = 1; svr < 5; svr++) // Copy over value of pair and three kickers
            topHand[svr + 1] = sameValRes[svr];
        std::cout << "Pair | High: " << sameValRes[1] << " | Kicker: " << sameValRes[2] << " | Second kicker: " << sameValRes[3] << " | Third kicker: " << sameValRes[4] << std::endl;
    } else { // High card
        topHand[1] = HC;
        for (int c = 0; c < 7; c++) // Copy over all cards, for comparison
            topHand[c + 2] = cards[c]->cardValue;
        std::cout << "High card" << std::endl;
    }

    std::cout << std::endl;
}

int* Hand::sameVal(Card* cards[], int results[]) {
    // Pairs check
    int vals[13] = {0};

    for (int c = 0; c < 7; c++)
        vals[cards[c]->cardValue]++;

    int coi[2] {0};

    for (int c = 1; c < 13; c++) { // Needs revising
        for (int cois = 0; cois < 2; cois++) {
            if (vals[c] >= vals[coi[cois]]) {
                if (cois == 0) {
                    coi[1] = coi[0];
                    coi[0] = c;
                } else if (c < coi[1])
                    coi[1] = c;
            }
        }
    }

    int kickers[3] = {0};

    if (coi[0] == 4)
        coi[1] = coi[0];

    if (coi[0] != 3 || coi[1] != 2) {
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
int* Hand::straight(Card* cards[], int results[]) {
    // Straight check

    int seq = 0;
    for (int v = 1; v < 7; v++) {
        if (cards[v - 1]->cardValue - cards[v]->cardValue == 1)
            seq++;
        else if (cards[v - 1]->cardValue - cards[v]->cardValue != 0)
            seq = 0;
        if (v == 6 && cards[0]->cardValue == 14 && cards[6]->cardValue - 1 == 1) {
            seq++;
            v++;
        }
        if (seq > 3) {
            results[0] = 1;
            int pos = 1;
            for (int c = v - 5; c <= v; c++)
                if (results[pos - 1] != cards[c % 7]->cardValue)
                    results[pos++] = cards[c % 7]->cardValue;

            if (results[5] == 14)
                results[5] = 1;
            break;
        }
    }

    return results;
}

int* Hand::flush(Card* cards[], int straightRes[], int results[]) {
    // Flush check

    int suits[4] = {0};

    for (int c = 0; c < 7; c++) {
        suits[cards[c]->suit]++;
        results[c + 1] = cards[c]->suit;
    }

    for (int s = 0; s < 4; s++) {
        if (suits[s] > 4) {
            results[0] = 1;
            if (straightRes[0] == 0 || straightRes[1] == 14) {
                for (int c = 0; c < 7; c++) {
                    if (cards[c]->suit == s)
                        results[c + 1] = 1;
                    else
                        results[c + 1] = 0;
                }
            } else if (straightRes[0] == 1 && straightRes[1] != 14) {
                int aces = /*cards[0]->suit == s ? 1 : cards[1]->suit == s ? 2 : */0; // TODO: NEEDS TESTING
                for (int c = 0; c < 7; c++) {
                    if (cards[(c + aces) % 7]->suit == s)
                        results[c + 1] = 1;
                    else
                        results[c + 1] = 0;
                }
            }
        }
    }

    return results;
}

void Hand::quicksortByVal(Card* cards[], int lPiv, int rPiv) {
    int pivot = cards[lPiv + (rPiv - lPiv) / 2]->cardValue;
    int a = lPiv, b = rPiv;

    while (a <= b) {
        while (cards[a]->cardValue > pivot)
            a++;
        while (cards[b]->cardValue < pivot)
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
        quicksortByVal(cards, lPiv, b);
    if (a < rPiv)
        quicksortByVal(cards, a, rPiv);
}