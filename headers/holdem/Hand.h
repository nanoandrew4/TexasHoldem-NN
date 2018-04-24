#ifndef TEXAS_HOLDEM_NN_HAND_H
#define TEXAS_HOLDEM_NN_HAND_H

#include "Card.h"
#include <vector>

/**
 * Types of hands that can be obtained in Texas Hold'em, from worst to best.
 */
enum Hands {
    HC, PAIR, TWO_PAIR, THREE_OAK, STRT, FLUSH, FH, FOUR_OAK, STR_FLUSH
};

/**
 * Contains data for a hand of cards, which is composed of two cards.
 * Also contains functions that allow to determine the score of a hand.
 */
class Hand {
public:
    Hand(Card *c1, Card *c2);

    ~Hand();

    // Cards held in this hand (2)
    std::vector<Card *> pocket;

    /**
     * Displays a vector of cards, showing their value and suit.
     *
     * @param cards Vector of cards to be displayed
     */
    static void displayHand(std::vector<Card *> cards);

    void displayHand();

    /**
     * Using the community cards and this hand, returns the score of this hand. Higher scores indicate better hands.
     * Calls valComboCheck(), straightCheck() and flushCheck().
     *
     * @param communityCards Vector containing Card pointers that are face up on the table
     */
    double getHandScore(std::vector<Card *> communityCards);

    /**
     * Sorts a vector of card pointers by their value, in descending order.
     *
     * @param cards Vector of Card pointers
     * @param lPiv Left-most point to sort array from
     * @param rPiv Right-most point to sort array from
     */
    static void quicksortByVal(std::vector<Card *> &cards, int lPiv, int rPiv);

private:
    /**
     * Checks vector of cards for four of a kind, full house, three of a kind, two pairs or a pair.
     * If any of the above are found, the appropriate score is returned. Otherwise a score of 0 is returned.
     *
     * @param cards Vector of Card pointers to evaluate
     */
    double valComboCheck(std::vector<Card *> cards);

    /**
     * Determines if a vector of Card pointers has a straight in it. If it does, it returns an array
     * containing a 1 as its first element, and the values composing the straight after that.
     *
     * @param cards Vector of card pointers to evaluate
     * @param results Integer vector of size 6, where the results of this check are written to
     */
    void straightCheck(std::vector<Card *> cards, std::vector<int> &results);

    /**
     * Determines if a vector of Card pointers has a flush. If it does, it writes 9 numbers to the 'results'
     * vector. If a flush exists, the suit the flush is composed of is written to results[0]. Otherwise results[0] is -1.
     * 8 digits, representing the different suits, are written to results[1-8].
     *
     * @param cards Vector of Card pointers to check for flush
     * @param results Integer vector of size 9, where the results of this check are written to
     */
    void flushCheck(std::vector<Card *> cards, std::vector<int> &results);
};

#endif //TEXAS_HOLDEM_NN_HAND_H