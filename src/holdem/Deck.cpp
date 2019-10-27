#include <iostream>
#include <random>
#include <algorithm>
#include "../../headers/holdem/Deck.h"

std::default_random_engine randomEngine(std::chrono::system_clock::now().time_since_epoch().count());

Deck::Deck(bool autoReShuffle) {
	deck.resize(defaultSize, Card((Suit) 0, 2));
	for (unsigned int c = 0; c < defaultSize; ++c) deck.at(c) = Card((Suit) ((c / 13)), (c % 13) + 2);
	pos = defaultSize;
	this->autoReShuffle = autoReShuffle;
	std::shuffle(deck.begin(), deck.end(), randomEngine);
}

Deck::Deck(std::vector<Card *> cards) {
	deck.resize(52 - cards.size(), Card((Suit) 0, 2));

	unsigned int omittedCards = 0;
	for (unsigned int c = 0; c < defaultSize; ++c) {
		const Card currCard = Card((Suit) ((c / 13)), (c % 13) + 2);
		long cardIdx = currCard.isContainedIn(cards);
		if (cardIdx != -1) {
			cards.erase(cards.begin() + cardIdx);
			++omittedCards;
		} else
			deck.at(c - omittedCards) = currCard;
	}

	pos = deck.size();
	std::shuffle(deck.begin(), deck.end(), randomEngine);
}

Deck::~Deck() = default;

Card *Deck::deal() {
	// To prevent breaking, decks can be reused. Re-shuffle and reset pos if deck "runs out"
	if (pos == 0) {
		if (autoReShuffle)
			std::shuffle(deck.begin(), deck.end(), randomEngine);
		pos = deck.size();
	}

	return &deck.at(--pos);
}
