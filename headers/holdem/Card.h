#ifndef TEXAS_HOLDEM_NN_CARD_H
#define TEXAS_HOLDEM_NN_CARD_H

#include <vector>

enum Suit {
	CLUBS, SPADES, HEARTS, DIAMONDS
};

/**
 * Contains info that a physical card would.
 * A value between [2-14] ([2-Ace]) and a suit to which the card belongs.
 */
class Card {
public:

	/**
	 * Creates a new card with the passed values.
	 *
	 * @param suit Suit the new card belongs to (see Suit enum)
	 * @param cardVal Value of card ([2-14])
	 */
	Card(Suit suit, std::uint8_t cardVal);

	/**
	 * Returns the first letter of the suit name assigned to this card.
	 *
	 * @return Character of first letter of the suit name
	 */
	[[nodiscard]] char getSuitSymbol() const;

	/**
	 * Returns the value assigned to this card.
	 *
	 * @return Integer representing value assigned to this card.
	 */
	[[nodiscard]] std::uint8_t getCardValue() const { return cardValue; }

	/**
	 * Returns the suit assigned to this card.
	 *
	 * @return Suit (see Suit enum) assigned to this card
	 */
	[[nodiscard]] Suit getSuit() const { return suit; }

	friend bool operator==(const Card lhs, const Card rhs) {
		return lhs.cardValue == rhs.cardValue && lhs.suit == rhs.suit;
	}

	/**
	 *
	 * @param cards
	 * @return
	 */
	[[nodiscard]] long isContainedIn(const std::vector<Card *> &cards) const {
		for (long c = 0; c < cards.size(); ++c) if (*cards.at(c) == *this) return c;
		return -1;
	}

private:
	Suit suit;
	std::uint8_t cardValue;
};

#endif //TEXAS_HOLDEM_NN_CARD_H
