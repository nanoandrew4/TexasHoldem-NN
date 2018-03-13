#ifndef TEXAS_HOLDEM_NN_HAND_H
#define TEXAS_HOLDEM_NN_HAND_H

#include "Card.h"

/**
 * Types of hands that can be obtained in Holdem, from best to worst.
 */
enum Hands {
    STR_FLUSH, FOUR_OAK, FH, FLUSH, STRT, THREE_OAK, TWO_PAIR, PAIR, HC
};

/**
 * Contains data for a hand of cards, which is composed of two cards.
 * This class also contains functions that allow to determine the best hand the player has, using the 'flop' in
 * combination with the two cards contained in this class.
 */
class Hand {
public:
    Hand(Card* c1, Card* c2);
    ~Hand();

    // Cards held in this hand
    Card* pocket[2];

    /**
     * Displays an array of cards, showing their value and suit.
     *
     * @param cards Array of cards to be displayed
     * @param numOfCards Number of cards in the 'cards' array
     */
    void displayHand(Card* cards[], int numOfCards = 2);

    /**
     * Using the flop (community cards, the ones on the table) and this hand, returns the best hand that can be played,
     * along with some information that can be used to break ties in the event that two players have the same hand type.
     *
     * @param playerNum Player number at the table
     * @param flop Array containing the cards that are face up on the table
     * @param topHand Array of size 9, where the data regarding the hand will be written to
     */
    void recordBestHand(int playerNum, Card* flop[], int topHand[]);

private:
    /**
     * Checks array of cards (combination of the cards in the hand, plus the community cards) for four of a kind,
     * full house, three of a kind, two pairs or a pair. The results array stores the best option, plus some
     * information required in case two hands have the same hand type, to break a tie.
     *
     * If Four of a kind:
     * results[0] -> 4 (representing four of a kind)
     * results[1] -> Value of the cards forming the 4 of a kind
     * results[2] -> High card that is not a part of the 4 of a kind (also known as first kicker kicker)
     *
     * If Full House:
     * results[0] -> 32 (Three of a kind + pair, simply for representation)
     * results[1] -> Value of three of a kind that forms the full house
     * results[2] -> Value of pair that forms the full house
     *
     * If Three of a kind:
     * results[0] -> 3 (representing three of a kind)
     * results[1] -> Value of the cards forming the 3 of a kind
     * results[2-3] -> 2 Highest kickers (cards that do not form part of the three of a kind)
     *
     * If Two pairs:
     * results[0] -> 22 (representing pair + pair)
     * results[1] -> High pair
     * results[2] -> Second highest pair
     * results[3-4] -> 2 Highest kickers (cards that do not form part of either of the pairs)
     *
     * If Pair:
     * results[0] -> 2 (representing pair)
     * results[1] -> Highest pair
     * results[2-4] -> 3 Highest kickers (cards that do not form part of the pair)
     *
     * @param cards Array of cards to determine best hand for
     * @param results Array of size 5, where the results or this check are written to
     */
    void sameVal(Card* cards[], int results[]);

    /**
     * Determines if a hand (hand + community cards) has a straight in it. If it does, it returns an array
     * containing a 1 as its first element, and the values composing the straight after that.
     *
     * If there is no straight, an array of 0's is written to 'results'.
     *
     * @param cards Cards to be evaluated
     * @param results Array of size 6, where the results of this check are written to
     */
    void straightCheck(Card* cards[], int results[]);

    /**
     * Determines if this hand (hand + community cards) has a flush. If it does, it writes 9 numbers to the 'results'
     * array. 1 is written to results[0], and 8 binary digits, with 1 representing that the card belongs to the flush
     * and 0 representing that the card does not belong to the flush, which are written to results[1-8].
     *
     * If there is no flush, an array of 0's is written to 'results'
     *
     * @param cards Array of cards to check for flush
     * @param results Array of size 9, where the results of this check are written to
     */
    void flushCheck(Card* cards[], int results[]);

    /**
     * Sorts an array of cards (combination of the cards in the hand, plus the 'flop') by their value
     *
     * @param cards Combined array of cards in hand and flop
     * @param lPiv Left-most point to sort array from
     * @oaram rPiv Right-most point to sort array from
     */
    void quicksortByVal(Card* cards[], int lPiv, int rPiv);
};


#endif //TEXAS_HOLDEM_NN_HAND_H
