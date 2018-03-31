#include <iostream>
#include <vector>
#include "../../headers/holdem/Hand.h"

Hand::Hand(Card* c1, Card* c2) {
    pocket.push_back(c1); pocket.push_back(c2);
}

Hand::~Hand() {}

// TODO: CHECK FOR BIAS TOWARDS HIGHER CARDS

/**
 * Displays an vector of cards, so the user can see what is going on.
 *
 * @param cards Vector of pointers to cards to display
 * @param numOfCards Number of cards to be displayed
 */
void Hand::displayHand(std::vector<Card*> cards, int numOfCards) {
    for (int c = 0; c < numOfCards && c < cards.size(); c++) {
        if (cards.at(c)->getCardValue() > 10) {
            if (cards.at(c)->getCardValue() == 11)
                std::cout << "J";
            else if (cards.at(c)->getCardValue() == 12)
                std::cout << "Q";
            else if (cards.at(c)-> getCardValue() == 13)
                std::cout << "K";
            else if (cards.at(c)->getCardValue() == 14)
                std::cout << "A";
            std::cout << cards.at(c)->getSuitSymbol() << (numOfCards - c != 1 ? " | " : "\n");
        } else
            std::cout << cards.at(c)->getCardValue() << cards.at(c)->getSuitSymbol() << (numOfCards - c != 1 ? " | " : "\n");
    }
}

void Hand::displayHand() {
    displayHand(pocket, 2);
}

void Hand::recordBestHand(int playerNum, std::vector<Card*> communityCards, std::vector<int> topHand) {
    // Check for flushCheck, straightCheck and # of cards. All else can be determined from this
    topHand.push_back(playerNum);

    int numOfCards = communityCards.size() + 2;

    std::vector<Card*> cards;
    for (int c = 0; c < 2; c++)
        cards.push_back(pocket.at(c));
    for (int cc = 0; cc < numOfCards - 2; cc++)
        cards.push_back(communityCards.at(cc));

    quicksortByVal(cards, 0, numOfCards - 1);

//    std::cout << "Evaluating for player " << playerNum << ": ";
//    displayHand(cards, 7);

    std::vector<int> straightRes(6, 0);
    straightCheck(cards, straightRes);

    std::vector<int> flushRes(8, 0);
    flushCheck(cards, flushRes);

//    for (int i = 0; i < 8; i++)
//        std::cout << flushRes.at(i) << " ";
//    std::cout << std::endl;
//    for (int i = 0; i < 6; i++)
//        std::cout << straightRes.at(i) << " ";
//    std::cout << std::endl;

    std::vector<int> sameValRes(5, 0);
    sameVal(cards, sameValRes);

    if (straightRes.at(0) == 1 && flushRes.at(0) == 1) { // Verify straight flush (and royal flush)
        for (int c = 0; c < 4; c++) {
            if (cards.at(c)->getCardValue() == straightRes.at(1)) {
                int suit = flushRes.at(c);
                int matches = flushRes.at(c + 1) == suit ? 1 : 0;
                for (int cc = c + 1; cc < 9 && matches < 5; cc++)
                    if (cards.at(cc % numOfCards)->getCardValue() == straightRes.at(matches + 1) && flushRes.at((cc % numOfCards) + 1) == suit)
                        matches++;
                if (matches == 5) {
                    topHand.push_back(STR_FLUSH);
                    topHand.push_back(cards.at(c)->getCardValue());
//                    std::cout << "Straight flush | High: " << cards.at(c)->getCardValue() << std::endl << std::endl;
                    return;
                }
            }
        }
    }

    if (sameValRes.at(0) == 4) { // 4 of a kind
        topHand.push_back(FOUR_OAK);
        topHand.push_back(sameValRes.at(1)); // Value of four of a kind
        topHand.push_back(sameValRes.at(2)); // Kicker
//        std::cout << "Four of a kind | High " << sameValRes.at(1) << " | Kicker: " << sameValRes.at(2) << std::endl;
    } else if (sameValRes.at(0) == 32) { // Full house
        topHand.push_back(FH);
        topHand.push_back(sameValRes.at(1)); // Value of three of a kind
        topHand.push_back(sameValRes.at(2)); // Value of pair
//        std::cout << "Full house | High TOAK " << sameValRes.at(1) << " | High pair: " << sameValRes.at(2) << std::endl;
    } else if (flushRes.at(0) == 1) { // Flush
        topHand.push_back(FLUSH);
        for (int c = 0; c < numOfCards; c++)
            topHand.push_back(cards.at(c)->getCardValue()); // Copy all card values in case multiple players have a flushCheck
//        std::cout << "Flush" << std::endl;
    } else if (straightRes.at(0) == 1) { // Straight
        topHand.push_back(STRT);
        topHand.push_back(straightRes.at(1)); // Highest card in the straightCheck
//        std::cout << "Straight | High in straight: " << straightRes.at(1) << std::endl;
    } else if (sameValRes.at(0) == 3) { // Three of a kind
        topHand.push_back(THREE_OAK);
        for (int svr = 1; svr < 4; svr++) // Copy over what the value of the match is, along with two kickers
            topHand.push_back(sameValRes.at(svr));
//        std::cout << "Three of a kind | High: " << sameValRes.at(1) << " | Kicker: " << sameValRes.at(2) << " | Second kicker: " << sameValRes.at(3) << std::endl;
    } else if (sameValRes.at(0) == 22) { // Two pairs
        topHand.push_back(TWO_PAIR);
        for (int svr = 1; svr < 5; svr++) // Copy over values of two pairs, and two kickers
            topHand.push_back(sameValRes.at(svr));
//        std::cout << "Two pairs | High pair: " << sameValRes.at(1) << " | Low pair: " << sameValRes.at(2) << " | Kicker: " << sameValRes.at(3) << " | Second kicker: " << sameValRes.at(4) << std::endl;
    } else if (sameValRes.at(0) == 2) { // Pair
        topHand.push_back(PAIR);
        for (int svr = 1; svr < 5; svr++) // Copy over value of pair and three kickers
            topHand.push_back(sameValRes.at(svr));
//        std::cout << "Pair | High: " << sameValRes.at(1) << " | Kicker: " << sameValRes.at(2) << " | Second kicker: " << sameValRes.at(3) << " | Third kicker: " << sameValRes.at(4) << std::endl;
    } else { // High card
        topHand.push_back(HC);
        for (int c = 0; c < numOfCards; c++) // Copy over all cards, for comparison
            topHand.push_back(cards.at(c)->getCardValue());
//        std::cout << "High card" << std::endl;
    }

//    std::cout << std::endl;
}

void Hand::sameVal(std::vector<Card*> cards, std::vector<int> results) {
    std::vector<int> vals(15, 0);

    // Count how many cards have any given value
    for (int c = 0; c < cards.size(); c++)
        vals.at(cards.at(c)->getCardValue())++;

    // Two card values of most interest (read as 'Card Of Interest'). First element is more interesting than the second
    std::vector<int> coi(2, 0);

    for (int c = 2; c < 15; c++) { // Runs through all values and determines which card values will give the best hand
        for (int cois = 0; cois < 2; cois++) {
            if (vals.at(c) >= vals.at(coi.at(cois))) {
                if (cois == 0) {
                    coi.at(1) = coi.at(0);
                    coi.at(0) = c;
                } else if (c < coi.at(1))
                    coi.at(1) = c;
            }
        }
    }

    std::vector<int> kickers(3, 0);

    if (coi.at(0) != 3 || coi.at(1) != 2) { // Gets kickers unless Full House, which does not have any
        int k = 0;
        for (int c = 0; c < cards.size() && k < 3; c++)
            if (cards.at(c)->getCardValue() != coi.at(0) && cards.at(c)->getCardValue() != coi.at(1))
                kickers.at(k++) = cards.at(c)->getCardValue();
    }

    if (vals.at(coi.at(0)) == 2 && vals.at(coi.at(1)) != 2) { // Pair
        results.at(0) = 2;
        results.at(1) = coi.at(0);
        for (int k = 0; k < 3; k++)
            results.at(2 + k) = kickers.at(k);
    } else if (vals.at(coi.at(0)) == 2 && vals.at(coi.at(1)) == 2) { // Two pair
        results.at(0) = 22;
        results.at(1) = coi.at(0);
        results.at(2) = coi.at(1);
        for (int k = 0; k < 2; k++)
            results.at(3 + k) = kickers.at(k);
    } else if (vals.at(coi.at(0)) == 3 && vals.at(coi.at(1)) != 2) { // Three of a kind
        results.at(0) = 3;
        results.at(1) = coi.at(0);
        for (int k = 0; k < 2; k++)
            results.at(2 + k) = kickers.at(k);
    } else if (vals.at(coi.at(0)) == 3 && vals.at(coi.at(1)) == 2) { // Full house
        results.at(0) = 32;
        results.at(1) = coi.at(0);
        results.at(2) = coi.at(1);
    } else if (vals.at(coi.at(0)) == 4) { // Four of a kind
        results.at(0) = 4;
        results.at(1) = coi.at(0);
        results.at(2) = kickers.at(0);
    }
}

void Hand::straightCheck(std::vector<Card*> cards, std::vector<int> results) {
    if (cards.size() < 5)
        return;

    int seq = 0; // Length of potential sequence
    for (int v = 1; v < cards.size(); v++) {
        if (cards.at(v - 1)->getCardValue() - cards.at(v)->getCardValue() == 1)
            seq++;
        else if (cards.at(v - 1)->getCardValue() - cards.at(v)->getCardValue() != 0)
            seq = 0;
        if (v == 6 && cards.at(0)->getCardValue() == 14 && cards.at(6)->getCardValue() - 1 == 1) { // Special ace check as 1
            seq++;
            v++;
        }
        if (seq > 3) { // 1-2-3-4-5 (only 4 '-', so if 4 exist then a sequence must exist)
            results.at(0) = 1; // Set straight found to true
            int pos = 1;
            // Record straight (and wrap around in case ace exists, which has val of 14)
            for (int c = (v + 2) % cards.size(); c <= v && pos < 6; c++)
                if (results.at(pos - 1) != cards.at(c % cards.size())->getCardValue())
                    results.at(pos++) = cards.at(c % cards.size())->getCardValue();

//             If last card in sequence is ace, change calue from 14 to 1
//            if (results.at(5) == 14)
//                results.at(5) = 1;
            break;
        }
    }
}

void Hand::flushCheck(std::vector<Card*> cards, std::vector<int> results) {
    if (cards.size() < 5)
        return;

    std::vector<int> suits(4, 0); // Holds number of appearances of each suit

    // Count each suit and add it to the results array
    for (int c = 0; c < cards.size(); c++) {
        suits.at(cards.at(c)->getSuit())++;
        results.at(c + 1) = cards.at(c)->getSuit();
    }

    for (int s = 0; s < 4; s++) {
        if (suits.at(s) > 4) {
            results.at(0) = 1;
            return;
        }
    }
}

void Hand::quicksortByVal(std::vector<Card*> cards, int lPiv, int rPiv) {
    int pivot = cards.at(lPiv + (rPiv - lPiv) / 2)->getCardValue();
    int a = lPiv, b = rPiv;

    while (a <= b) {
        while (cards.at(a)->getCardValue() > pivot)
            a++;
        while (cards.at(b)->getCardValue() < pivot)
            b--;
        if (a <= b) {
            Card* c = cards.at(a);
            cards.at(a) = cards.at(b);
            cards.at(b) = c;
            a++;
            b--;
        }
    }

    if (lPiv < b)
        quicksortByVal(cards, lPiv, b);
    if (a < rPiv)
        quicksortByVal(cards, a, rPiv);
}