#ifndef TEXAS_HOLDEM_NN_HAND_H
#define TEXAS_HOLDEM_NN_HAND_H

#include "Card.h"
#include <vector>
#include <array>

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
	std::array<Card *, 2> pocket;

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
	double getHandScore(const std::vector<Card *> &communityCards);

private:
	/**
	 * Checks vector of cards for four of a kind, full house, three of a kind, two pairs or a pair.
	 * If any of the above are found, the appropriate score is returned. Otherwise a score of 0 is returned.
	 *
	 * @param cards Vector of Card pointers to evaluate
	 */
	double valComboCheck(const std::vector<Card *> &cards);

	/**
	 * TODO: REWRITE
	 * Determines if a vector of Card pointers has a straight in it. If it does, it returns an array
	 * containing a 1 as its first element, and the values composing the straight after that.
	 *
	 * @param cards Vector of card pointers to evaluate
	 * @param results Integer vector of size 6, where the results of this check are written to
	 */
	void straightCheck(const std::vector<Card *> &cards);

	/**
	 * TODO: REWRITE
	 * Determines if a vector of Card pointers has a flush. If it does, it writes 8 numbers to the 'results'
	 * vector. If a flush exists, the suit the flush is composed of is written to results[0]. Otherwise results[0] is
	 * 255 (0xff). 8 digits, representing the different suits, are written to results[1-7].
	 *
	 * @param cards Vector of Card pointers to check for flush
	 * @param results Integer vector of size 9, where the results of this check are written to
	 */
	void flushCheck(const std::vector<Card *> &cards);

	std::array<std::uint8_t, 8> straightRes = {};
	std::array<std::uint8_t, 8> flushRes = {};


	std::array<std::uint8_t, 15> vals{};
	std::array<std::uint8_t, 2> coi{};
	std::array<std::uint8_t, 3> kickers{};

	std::array<std::uint8_t, 4> suits{}; // Holds number of appearanc es of each suit
};

#endif //TEXAS_HOLDEM_NN_HAND_H