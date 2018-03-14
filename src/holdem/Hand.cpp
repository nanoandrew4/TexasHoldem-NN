#include <iostream>
#include "../../headers/holdem/Hand.h"

Hand::Hand(Card* c1, Card* c2) {
    this->pocket[0] = c1; this->pocket[1] = c2;
}

Hand::~Hand() {}

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

void Hand::displayHand() {
    displayHand(pocket, 2);
}

// TODO: VERIFY THAT SCORING WORKS WITH 5 AND 6 COMBINED SIZES
void Hand::recordBestHand(int playerNum, Card* communityCards[], int topHand[]) {
    // Check for flushCheck, straightCheck and # of cards. All else can be determined from this
    topHand[0] = playerNum;

    Card* cards[7];
    cards[0] = this->pocket[0];
    cards[1] = this->pocket[1];
    for (int f = 0; f < 5; f++)
        cards[f + 2] = communityCards[f];

    quicksortByVal(cards, 0, 6);

    std::cout << "Evaluating for player " << playerNum << ": ";
    displayHand(cards, 7);

    int straightRes[6] = {0};
    straightCheck(cards, straightRes);
    int flushRes[8] = {0};
    flushCheck(cards, flushRes);

//    for (int i = 0; i < 8; i++)
//        std::cout << flushRes[i] << " ";
//    std::cout << std::endl;
//    for (int i = 0; i < 6; i++)
//        std::cout << straightRes[i] << " ";
//    std::cout << std::endl;

    int sameValRes[5] = {0};
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
                    std::cout << "Straight flush | High: " << cards[c]->cardValue << std::endl << std::endl;
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
            topHand[c + 2] = cards[c]->cardValue; // Copy all card values in case multiple players have a flushCheck
        std::cout << "Flush" << std::endl;
    } else if (straightRes[0] == 1) { // Straight
        topHand[1] = STRT;
        topHand[2] = straightRes[1]; // Highest card in the straightCheck
        std::cout << "Straight | High in straightCheck: " << straightRes[1] << std::endl;
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

void Hand::sameVal(Card* cards[], int results[]) {
    int vals[15] = {0}; // Holds number of appearances of each card value

    // Count how many cards have any given value
    for (int c = 0; c < 7; c++)
        vals[cards[c]->cardValue]++;

    // Two card values of most interest (read as 'Card Of Interest'). First element is more interesting than the second
    int coi[2] = {0};

    for (int c = 1; c < 13; c++) { // Runs through all values and determines which card values will give the best hand
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

    if (coi[0] != 3 || coi[1] != 2) { // Gets kickers unless Full House, which does not have any
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
}

void Hand::straightCheck(Card* cards[], int results[]) {
    int seq = 0; // Length of potential sequence
    for (int v = 1; v < 7; v++) {
        if (cards[v - 1]->cardValue - cards[v]->cardValue == 1)
            seq++;
        else if (cards[v - 1]->cardValue - cards[v]->cardValue != 0)
            seq = 0;
        if (v == 6 && cards[0]->cardValue == 14 && cards[6]->cardValue - 1 == 1) { // Special ace check as 1
            seq++;
            v++;
        }
        if (seq > 3) { // 1-2-3-4-5 (only 4 '-', so if 4 exist then a sequence must exist)
            results[0] = 1; // Set straight found to true
            int pos = 1;
            // Record straight (and wrap around in case ace exists, which has val of 14)
            for (int c = (v + 2) % 7; c <= v; c++)
                if (results[pos - 1] != cards[c % 7]->cardValue)
                    results[pos++] = cards[c % 7]->cardValue;

            // If last card in sequence is ace, change calue from 14 to 1
            if (results[5] == 14)
                results[5] = 1;
            break;
        }
    }
}

void Hand::flushCheck(Card* cards[], int results[]) {
    int suits[4] = {0}; // Holds number of appearances of each suit

    // Count each suit and add it to the results array
    for (int c = 0; c < 7; c++) {
        suits[cards[c]->suit]++;
        results[c + 1] = cards[c]->suit;
    }

    // Any suit not belonging to the flush has its index in results set to 0, otherwise 1
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