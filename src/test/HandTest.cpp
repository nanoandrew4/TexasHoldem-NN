#include <iostream>
#include <algorithm>
#include "../../headers/test/HandTest.h"
#include "../../headers/holdem/Hand.h"
#include "../../headers/holdem/Deck.h"

void HandTest::test() {
	testStraightFlush();
	testFourOfAKind();
	testFullHouse();
	testFlush();
	testStraight();
	testThreeOfAKind();
	testTwoPair();
	testPair();
	testHighCard();
}

void HandTest::testStraightFlush() {
	for (int suit = 0; suit < 4; suit++)
		for (size_t extraCards = 0; extraCards < 3; extraCards++) {
			for (unsigned int cardVal = 6; cardVal <= 14; cardVal++) {
				Deck d(false);
				Card c1((Suit) suit, cardVal - 4), c2((Suit) suit, cardVal - 3);
				Hand hand(&c1, &c2);
				std::vector<Card *> commCards(3 + extraCards);
				for (unsigned int cc = 0; cc < 3; cc++) commCards.at(cc) = new Card((Suit) suit, cardVal - cc);

				Card *card;
				for (size_t ec = 0; ec < extraCards; ec++) {
					while ((card = d.deal())->getSuit() == suit); // Ensures only flush
					commCards.at(3 + ec) = card;
				}

				double score = hand.getHandScore(commCards);
				if (score <= STR_FLUSH || score > STR_FLUSH + 1) {
					std::cout << score << std::endl;
					testFail("Straight flush", hand.pocket, commCards);
					cleanCardVec(commCards, 3);
					return;
				}
				cleanCardVec(commCards, 3);
			}
		}
}

void HandTest::testFourOfAKind() {
	for (int suit = 0; suit < 4; suit++)
		for (size_t extraCards = 0; extraCards < 4; extraCards++) {
			for (unsigned int cardVal = 2; cardVal <= 14; cardVal++) {
				Deck d(false);
				Card c1((Suit) suit, cardVal), c2((Suit) suit, cardVal);
				Hand hand(&c1, &c2);
				std::vector<Card *> commCards(2 + extraCards);
				for (size_t cc = 0; cc < 2; cc++) commCards.at(cc) = new Card((Suit) suit, cardVal);

				Card *card;
				for (size_t ec = 0; ec < extraCards; ec++) {
					while ((card = d.deal())->getCardValue() == cardVal); // Ensures only four of a kind
					commCards.at(2 + ec) = card;
				}

				double score = hand.getHandScore(commCards);
				if (score <= FOUR_OAK || score >= STR_FLUSH) {
					testFail("Four of a kind", hand.pocket, commCards);
					cleanCardVec(commCards, 2);
					return;
				}
				cleanCardVec(commCards, 2);
			}
		}
}

void HandTest::testFullHouse() {
	for (int suit = 0; suit < 4; suit++)
		for (size_t extraCards = 0; extraCards < 3; extraCards++)
			for (unsigned int communityCardVal = 2; communityCardVal < 14; communityCardVal++) {
				Deck d(false);
				for (unsigned int cardVal = 2; cardVal <= 14; cardVal++) {
					if (cardVal == communityCardVal)
						continue;

					Card c1((Suit) suit, cardVal), c2((Suit) suit, cardVal);
					Hand hand(&c1, &c2);
					std::vector<Card *> commCards(3 + extraCards);
					for (size_t h = 0; h < 3; h++) commCards.at(h) = new Card((Suit) ((suit + 1) % 4), communityCardVal);

					for (size_t ec = 0; ec < extraCards; ec++) {
						Card *card;
						while ((card = d.deal())->getCardValue() == cardVal || // Ensures only full house
						       card->getCardValue() == communityCardVal);
						commCards.at(3 + ec) = card;
					}

					double score = hand.getHandScore(commCards);
					if (score <= FH || score >= FOUR_OAK) {
						testFail("Full House", hand.pocket, commCards);
						cleanCardVec(commCards, 3);
						return;
					}
					cleanCardVec(commCards, 3);
				}
			}
}

void HandTest::testFlush() {
	for (int suit = 0; suit < 4; suit++)
		for (size_t extraCards = 0; extraCards < 3; extraCards++) {
			for (unsigned int cardVal = 4; cardVal <= 14; cardVal++) {
				Deck d(false);
				Card c1((Suit) suit, cardVal - 2), c2((Suit) suit, cardVal);
				Hand hand(&c1, &c2);

				std::vector<Card *> commCards(3 + extraCards);

				Card *card;
				for (size_t c = 0; c < 3 + extraCards; c++) {
					while ((card = d.deal())->getSuit() != suit || // Ensures only flush
					       card->getCardValue() != (cardVal + 1 + 2 * c) % 13 + 2);
					commCards.at(c) = card;
				}

				double score = hand.getHandScore(commCards);
				if (score <= FLUSH || score >= FH) {
					testFail("Flush", hand.pocket, commCards);
					return;
				}
			}
		}
}

void HandTest::testStraight() {
	for (size_t extraCards = 0; extraCards < 3; extraCards++) {
		for (unsigned int cardVal = 6; cardVal <= 14; cardVal++) {
			Deck d(false);
			Card c1((Suit) 0, cardVal - 4), c2((Suit) 1, cardVal - 3);
			Hand hand(&c1, &c2);

			std::vector<Card *> commCards(3 + extraCards);
			for (unsigned int cc = 0; cc < 3; cc++)
				commCards.at(cc) = new Card((Suit) ((2 + cc) % 4), cardVal - cc);

			Card *card;
			for (size_t ec = 0; ec < extraCards; ec++) {
				while ((card = d.deal())->getSuit() == commCards.at(ec)->getSuit()); // Ensures only straight
				commCards.at(3 + ec) = card;
			}

			double score = hand.getHandScore(commCards);
			if (score <= STRT || score >= FLUSH) {
				std::cout << score << std::endl;
				testFail("Straight", hand.pocket, commCards);
				cleanCardVec(commCards, 3);
				return;
			}
			cleanCardVec(commCards, 3);
		}
	}
}

void HandTest::testThreeOfAKind() {
	for (size_t extraCards = 0; extraCards < 5; extraCards++) {
		for (unsigned int cardVal = 2; cardVal <= 14; cardVal++) {
			Deck d(false);
			Card c1((Suit) 0, cardVal), c2((Suit) 1, cardVal), c3((Suit) 2, cardVal);
			Hand hand(&c1, &c2);

			std::vector<Card *> commCards(1 + extraCards);
			commCards.at(0) = &c3;

			Card *card;
			for (size_t ec = 0; ec < extraCards; ec++) {
				while ((card = d.deal())->getSuit() == commCards.at(ec)->getSuit() || card->getCardValue() == cardVal
				       || card->getCardValue() != (cardVal + 3 * ec) % 13 + 2); // Ensures only three of a kind
				commCards.at(1 + ec) = card;
			}

			double score = hand.getHandScore(commCards);
			if (score <= THREE_OAK || score >= STRT) {
				testFail("Three of a kind", hand.pocket, commCards);
				return;
			}
		}
	}
}

void HandTest::testTwoPair() {
	for (size_t extraCards = 0; extraCards < 4; extraCards++) {
		for (unsigned int pairOneVal = 2; pairOneVal <= 14; pairOneVal++)
			for (unsigned int pairTwoVal = 2; pairTwoVal < 14; pairTwoVal++) {
				if (pairOneVal == (pairTwoVal + 2) % 13 + 2)
					continue;
				Deck d(false);
				Card c1((Suit) 0, pairOneVal), c2((Suit) 1, pairOneVal),
						c3((Suit) 2, (pairTwoVal + 2) % 13 + 2), c4((Suit) 3, (pairTwoVal + 2) % 13 + 2);
				Hand hand(&c1, &c2);

				std::vector<Card *> commCards(2 + extraCards);
				commCards.at(0) = &c3;
				commCards.at(1) = &c4;

				Card *card;
				for (size_t ec = 0; ec < extraCards; ec++) {
					while ((card = d.deal())->getSuit() == commCards.at(ec)->getSuit() || // Ensures only two pairs
					       card->getCardValue() == pairOneVal || card->getCardValue() == (pairTwoVal + 2) % 13 + 2 ||
					       abs((int) card->getCardValue() - (int) commCards.at(ec + 1)->getCardValue()) < 4);
					commCards.at(2 + ec) = card;
				}

				double score = hand.getHandScore(commCards);
				if (score <= TWO_PAIR || score >= THREE_OAK) {
					testFail("Two pair", hand.pocket, commCards);
					return;
				}
			}
	}
}

void HandTest::testPair() {
	for (size_t extraCards = 0; extraCards < 6; extraCards++) {
		for (unsigned int pairVal = 2; pairVal <= 14; pairVal++) {
			Deck d(false);
			Card c1((Suit) 0, pairVal), c2((Suit) 1, pairVal);
			Hand hand(&c1, &c2);

			std::vector<Card *> commCards(extraCards);

			Card *card;
			for (size_t ec = 0; ec < extraCards; ec++) {
				while ((card = d.deal())->getSuit() != ec % 4 || card->getCardValue() == pairVal ||
				       card->getCardValue() != (pairVal + 3 * ec) % 13 + 2); // Ensures no card combinations
				commCards.at(ec) = card;
			}

			double score = hand.getHandScore(commCards);
			if (score <= PAIR || score >= TWO_PAIR) {
				testFail("Pair", hand.pocket, commCards);
				return;
			}
		}
	}
}

void HandTest::testHighCard() {
	for (size_t extraCards = 0; extraCards < 5; extraCards++)
		for (unsigned int cards = 2; cards <= 14; cards++) {
			Deck d(false);
			Card c1((Suit) 0, cards), c2((Suit) 1, (cards + 2) % 13 + 2);
			Hand hand(&c1, &c2);

			std::vector<Card *> commCards(extraCards);
			Card *card;
			for (size_t ec = 0; ec < extraCards; ec++) {
				while ((card = d.deal())->getCardValue() != (cards + 3 + 5 * ec) % 13 + 2 ||
				       card->getSuit() == (2 + ec) % 4); // Ensures no card combinations
				commCards.at(ec) = card;
			}

			double score = hand.getHandScore(commCards);
			if (score >= PAIR) {
				testFail("High card", hand.pocket, commCards);
				return;
			}
		}
}

void HandTest::testFail(const std::string &testName, std::array<Card *, 2> pocket, std::vector<Card *> commCards) {
	std::cout << "Hand -> " << testName << " test failed... Cards: ";
	commCards.insert(commCards.end(), pocket.begin(), pocket.end());
	std::sort(commCards.begin(), commCards.end(), [](Card *left, Card *right) -> bool {
		return left->getCardValue() < right->getCardValue();
	});
	Hand::displayHand(commCards);
}

void HandTest::cleanCardVec(std::vector<Card *> &cards, int lim) {
	for (size_t i = 0; i < lim; i++)
		delete cards.at(i);
	cards.clear();
}
