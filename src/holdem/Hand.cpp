#include <iostream>
#include <algorithm>
#include "../../headers/holdem/Hand.h"

Hand::Hand(Card *c1, Card *c2) {
	pocket.at(0) = c1;
	pocket.at(1) = c2;
}

Hand::~Hand() = default;

void Hand::displayHand(std::vector<Card *> cards) {
	for (unsigned long c = 0; c < cards.size() && c < cards.size(); c++) {
		if (cards.at(c)->getCardValue() > 10) {
			if (cards.at(c)->getCardValue() == 11)
				std::cout << "J";
			else if (cards.at(c)->getCardValue() == 12)
				std::cout << "Q";
			else if (cards.at(c)->getCardValue() == 13)
				std::cout << "K";
			else if (cards.at(c)->getCardValue() == 14)
				std::cout << "A";
			std::cout << cards.at(c)->getSuitSymbol() << (cards.size() - c != 1 ? " | " : "\n");
		} else
			std::cout << cards.at(c)->getCardValue() << cards.at(c)->getSuitSymbol()
			          << (cards.size() - c != 1 ? " | " : "\n");
	}
}

void Hand::displayHand() {
	displayHand({pocket.at(0), pocket.at(1)});
}

double Hand::getHandScore(const std::vector<Card *> &communityCards) {
	// Check for flushCheck, straightCheck and # of cards. All else can be determined from this.
	double score = 0, valScore = 0;

	unsigned long numOfCards = communityCards.size() + 2; // Community cards plus pocket (always 2)

	// Combine hand and community cards into one vector, and sort in descending order by value
	std::vector<Card *> cards(pocket.size() + communityCards.size());
	for (size_t c = 0; c < pocket.size(); c++)
		cards.at(c) = pocket.at(c);
	for (size_t c = 0; c < communityCards.size(); c++)
		cards.at(c + 2) = communityCards.at(c);

	std::sort(cards.begin(), cards.end(), [](Card *left, Card *right) -> bool {
		return left->getCardValue() > right->getCardValue();
	});

	straightCheck(cards);
	flushCheck(cards);

	// Verify straight flush (and royal flush)
	if (straightRes.at(0) == 1 && flushRes.at(0) != 0xff)
		for (unsigned long seqStart = 0; seqStart < 4; seqStart++) {
			unsigned long suit = flushRes.at(0), matches = flushRes.at(seqStart + 1) == suit ? 1 : 0;
			for (size_t seqCard = seqStart + 1; seqCard < 9 && matches < 5; seqCard++)
				if (cards.at(seqCard % numOfCards)->getCardValue() ==
				    straightRes.at((seqStart + matches) % numOfCards + 1) &&
				    flushRes.at((seqCard % numOfCards) + 1) == suit)
					++matches;
			if (matches == 5) {
				//      Base         Highest card in straight flush
				score = STR_FLUSH + (cards.at(seqStart)->getCardValue() / 20.0);
				return score;
			}
		}

	/*
	 * Check four of a kind, full house, three of a kind, two pair and pair.
	 * Since a flush or a straight are not possible when a four of a kind or a full house are attained, the conditional
	 * statements below won't affect those hands (which rank higher than a flush or a straight)
	 */
	valScore = valComboCheck(cards);

	if (flushRes.at(0) != 0xff) { // Flush
		score = FLUSH; // Base score
		unsigned long suit = flushRes.at(0), matches = 0;
		for (size_t c = 0; c < numOfCards; c++) // All cards in flush
			if (cards.at(c)->getSuit() == suit)
				score += cards.at(c)->getCardValue() / (20.0 * pow(10, matches++));
	} else if (straightRes.at(0) == 1) // Straight
		//       Base    Highest card in straight
		score = STRT + (straightRes.at(1) / 20.0);
	else if (valScore == 0) // High card, base score of 0
		for (size_t c = 0; c < numOfCards; c++) // Copy over all cards, for comparison
			score += cards.at(c)->getCardValue() / (20.0 * pow(10, c));

	return score > valScore ? score : valScore;
}

double Hand::valComboCheck(const std::vector<Card *> &cards) {
	double score = 0;

	// Count how many cards have any given value
	for (auto &card : cards)
		++vals.at(card->getCardValue());

	// Two card values of most interest (read as 'Card Of Interest'). First element is more interesting than the second.
	// Runs through all values and determines which card combinations will give the best hands.
	for (std::uint8_t c = 2; c < 15; ++c)
		for (std::uint8_t cois = 0; cois < 2; ++cois)
			if (vals.at(c) >= vals.at(coi.at(cois))) {
				if (cois == 0) {
					coi.at(1) = coi.at(0);
					coi.at(0) = c;
					break;
				} else if (c > coi.at(1))
					coi.at(1) = c;
			}

	if (coi.at(0) != 3 || coi.at(1) != 2) { // Gets kickers unless Full House, which does not have any
		size_t kicker = 0;
		for (size_t c = 0; c < cards.size() && kicker < 3; ++c)
			if (cards.at(c)->getCardValue() != coi.at(0) && cards.at(c)->getCardValue() != coi.at(1))
				kickers.at(kicker++) = cards.at(c)->getCardValue();
	}
	// TODO: DOCUMENT SCORING METHOD...
	if (vals.at(coi.at(0)) == 2 && vals.at(coi.at(1)) != 2) { // Pair
		//       Base    Card in pair
		score += PAIR + (coi.at(0) / 20.0);
		for (size_t kicker = 0; kicker < 3; kicker++) // Kickers
			score += kickers.at(kicker) / (200.0 * pow(10, kicker));
	} else if (vals.at(coi.at(0)) == 2 && vals.at(coi.at(1)) == 2) { // Two pair
		//       Base        Card in high pair    Card in low pair
		score += TWO_PAIR + (coi.at(0) / 20.0) + (coi.at(1) / 200.0);
		for (size_t kicker = 0; kicker < 2; kicker++) // Kickers
			score += kickers.at(kicker) / (1500.0 * pow(10, kicker));
	} else if (vals.at(coi.at(0)) == 3 && vals.at(coi.at(1)) != 2) { // Three of a kind
		//       Base         Card in three of a kind
		score += THREE_OAK + (coi.at(0) / 20.0);
		for (size_t kicker = 0; kicker < 2; kicker++) // Kickers
			score += kickers.at(kicker) / (200.0 * pow(10, kicker));
	} else if (vals.at(coi.at(0)) == 3 && vals.at(coi.at(1)) == 2) { // Full house
		//       Base  Card in three OAK    Card in pair
		score += FH + (coi.at(0) / 20.0) + (coi.at(1) / 200.0);
	} else if (vals.at(coi.at(0)) == 4) { // Four of a kind
		//       Base        Cards in four of a kind   Kicker
		score += FOUR_OAK + (coi.at(0) / 20.0) + (kickers.at(1) / 200.0);
	}

	return score;
}

void Hand::straightCheck(const std::vector<Card *> &cards) {
	std::uint8_t numOfCards = static_cast<uint8_t>(cards.size());
	if (numOfCards < 5)
		return;

	std::uint8_t seq = 0, v; // Length of potential sequence
	for (v = 1; v < numOfCards; v++) {
		// Checks if ordered cards descend in steps of 1 (or don't descend, that's fine)
		if (cards.at(v - 1u)->getCardValue() - cards.at(v)->getCardValue() == 1)
			straightRes.at(seq++ + 1) = (cards.at(v - 1u)->getCardValue());
		else if (cards.at(v - 1u)->getCardValue() - cards.at(v)->getCardValue() != 0) {
			if (seq > 3)
				break;
			else
				seq = 0;
		}
		// Special ace check as 1
		if (v == numOfCards - 1 && cards.at(0)->getCardValue() == 14 &&
		    cards.at(numOfCards - 1u)->getCardValue() - 1 == 1 && seq >= 3) {
			straightRes.at(seq++ + 1) = cards.at(numOfCards - 1u)->getCardValue();
			straightRes.at(seq + 1) = 14;
			straightRes.at(0) = 1;
			return;
		}
	}

	if (seq > 3) { // 1-2-3-4-5 (only 4 '-', so if 4 exist then a sequence must exist)
		straightRes.at(0) = 1; // Set straight found to true
		straightRes.at(seq + 1) = cards.at(v - 1u)->getCardValue();
	}
}

void Hand::flushCheck(const std::vector<Card *> &cards) {
	flushRes.at(0) = 0xff;
	std::uint8_t numOfCards = static_cast<uint8_t>(cards.size());

	if (numOfCards < 5)
		return;

	// Count each suit and add it to the results array
	for (size_t c = 0; c < numOfCards; c++) {
		suits.at(cards.at(c)->getSuit())++;
		flushRes.at(c + 1) = cards.at(c)->getSuit();
	}

	for (std::uint8_t s = 0; s < 4; s++) {
		if (suits.at(s) > 4) {
			flushRes.at(0) = s;
			return;
		}
	}
}