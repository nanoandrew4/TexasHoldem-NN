#include <iostream>
#include <random>
#include "../../headers/holdem/Deck.h"

static unsigned long x = 123456789, y = 362436069, z = 521288629; // Used in XOR RNG

Deck::Deck(bool autoReShuffle) {
	deck.resize(defaultSize, Card((Suit) 0, 2));
	for (unsigned int c = 0; c < defaultSize; ++c) deck.at(c) = (Card((Suit) ((c / 13)), (c % 13) + 2));
	pos = defaultSize;
	this->autoReShuffle = autoReShuffle;
	shuffle();
}

Deck::Deck(std::vector<Card *> cards) {
	size_t cardsEliminated = 0;
	deck.resize(52, Card((Suit) 0, 2));

	for (unsigned int c = 51; c < 0xffff; --c) { // Wait for underflow, then stop
		if (cardsEliminated >= cards.size() || cards.at(cardsEliminated)->getCardValue() != (c % 13) + 2 ||
		    cards.at(cardsEliminated)->getSuit() != (Suit) ((c / 13)))
			deck.at(c) = Card((Suit) ((c / 13)), (c % 13) + 2);
		else
			cardsEliminated++;
	}

	// Remove any that were not caught in the initialization
	while (cardsEliminated != cards.size()) {
		// 'c' is unsigned, so when it overflows, exit the loop
		for (size_t c = deck.size() - 1; c < 0xffffffff && cardsEliminated != cards.size(); --c)
			if (cards.at(cardsEliminated)->getCardValue() == deck.at(c).getCardValue() &&
			    cards.at(cardsEliminated)->getSuit() == deck.at(c).getSuit()) {
				deck.at(c) = Card((Suit) 0, 2);
				cardsEliminated++;
			}
	}

	pos = deck.size();
	shuffle();

}

Deck::~Deck() = default;

/*
 * XORshift RNG.
 */
unsigned long xorshf96() {
	unsigned long t;
	x ^= x << 16;
	x ^= x >> 5;
	x ^= x << 1;

	t = x;
	x = y;
	y = z;
	z = t ^ x ^ y;

	return z;
}

void Deck::shuffle() {
	size_t deckSize = deck.size();
	size_t cardsToShuffle = deckSize / 2;

	unsigned long newPos;
	Card tmp = deck.at(0);
	for (size_t i = 0; i < cardsToShuffle; ++i) {
		newPos = xorshf96() % deckSize;
		tmp = deck.at(i);
		deck.at(i) = deck.at(newPos);
		deck.at(newPos) = tmp;
	}
}

Card *Deck::deal() {
	// To prevent breaking, decks can be reused. Re-shuffle and reset pos if deck "runs out"
	if (pos == 0) {
		if (autoReShuffle)
			shuffle();
		pos = deck.size();
	}

	return &deck.at(--pos);
}
