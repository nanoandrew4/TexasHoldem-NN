#include <iostream>
#include <utility>
#include "../../headers/holdem/Table.h"

bool Table::output = false;

Table::Table(std::vector<Player *> &players) {
	if (players.size() < 2) {
		std::cout << "Table must have at least 2 players" << std::endl;
		return;
	}
	this->numOfPlayers = players.size();
	this->players = std::move(players);
}

Table::~Table() = default;

void Table::play() {
	for (unsigned long dealer = 0; dealer < numOfPlayers; dealer++) {
		if (!newRound())
			return;

		int blinds = assignSpecialRoles(dealer);
		initAntes(blinds == 2);

		// Pre-flop
		playRound((lastPlayerRaised + 1) % numOfPlayers, (int) lastRaise /* Will always be a small val */);

		// Return initial ante to small blind if still playing, since they will have committed an excess to the pot
		if (smallBlind->isPlaying() && blinds == 2) {
			smallBlind->collectWinnings(SB);
			pot -= SB;
		}

		if (output)
			std::cout << "Pot: " << pot << std::endl << std::endl;
		for (int br = 0; br < 3 && activePlayers > 1; br++) {
			if (br == 0)
				for (int f = 0; f < 3; f++)
					communityCards.push_back(deck.deal());
			else
				communityCards.push_back(deck.deal());
			if (output) {
				std::cout << "Community cards: ";
				players.at(0)->hand.displayHand(communityCards);
				std::cout << std::endl;
			}
			playRound((dealer + 1) % numOfPlayers);
			if (output)
				std::cout << "Pot: " << pot << std::endl << std::endl;
		}

		if (activePlayers > 1) // Determine winner(s) and split pot if necessary
			splitPot();
		else // Last standing player takes all the cash
			try {
				for (size_t p = 0; p < numOfPlayers; p++)
					if (players.at(p)->isPlaying()) {
						if (output)
							std::cout << players.at(p)->getName() << " won" << std::endl << std::endl;
						players.at(p)->collectWinnings(pot);
					}
			} catch (std::exception &e) {
				std::cout << "Split for one" << std::endl;
			}
		communityCards.clear();
	}
}

void Table::playUntilOneLeft() {
	while (activePlayers > 1)
		play();
}

bool Table::newRound() {
	try {
		activePlayers = numOfPlayers;
		// Count how many players are able to play the next round
		for (size_t p = 0; p < numOfPlayers; p++) {
			players.at(p)->newRound();
			if (!players.at(p)->isPlaying())
				activePlayers--;
		}

		int count = 0;
		for (size_t i = 0; i < numOfPlayers; i++) if (players.at(i)->isPlaying()) count++;
		if (count != activePlayers) {
			std::cout << "Error in activPlayers: " << activePlayers << " " << count << std::endl;
			exit(11);
		}

		if (activePlayers < 2)
			return false;

		pot = lastRaise = lastPlayerRaised = 0;
		try {
			deck = Deck();
		} catch (std::bad_alloc &e) {
			std::cout << "Deck alloc is bad" << std::endl;
		}

		std::vector<Card *> cards;
		for (int p = 0; p < activePlayers * 2; p++)
			cards.push_back(deck.deal());

		unsigned long cardCount = 0;
		for (size_t p = 0; p < numOfPlayers; p++) {
			if (players.at(p)->isPlaying()) {
				players.at(p)->hand = Hand(cards.at(cardCount), cards.at(cardCount + activePlayers));
				cardCount++;
			}
//        std::cout << players.at(p)->getName() << "'s hand: "; players.at(p)->hand->displayHand();
		}
	} catch (std::exception &e) {
		std::cout << "NRound" << std::endl;
	}
	return true;
}

int Table::assignSpecialRoles(unsigned long round) {
	int blinds = 0;
	try {
		dealer = players.at(round);
		if (output)
			std::cout << "Dealer: " << dealer->getName() << std::endl;
		for (int p = 1; p < numOfPlayers; p++)
			if (players.at((round + p) % numOfPlayers)->isPlaying()) {
				if (blinds == 0) {
					lastPlayerRaised = (round + p) % numOfPlayers;
					smallBlind = players.at(lastPlayerRaised);
					blinds++;

					if (output)
						std::cout << "Small Blind: " << smallBlind->getName() << std::endl;
					if (activePlayers == 2)
						break;
				} else {
					lastPlayerRaised = (round + p) % numOfPlayers;
					bigBlind = players.at(lastPlayerRaised);
					blinds++;

					if (output)
						std::cout << "Big Blind: " << bigBlind->getName() << std::endl;
					break;
				}
			}
	} catch (std::exception &e) {
		std::cout << "SpecRoles" << std::endl;
	}
	return blinds;
}

void Table::initAntes(bool bigBlindSet) {
	// Small blind ante before round starts
	pot += smallBlind->anteUp(SB);
	lastRaise = SB;

	if (bigBlindSet) { // Big blind ante before round starts, if enough players to set big blind
		pot += bigBlind->anteUp(BB);
		lastRaise = BB;
	}
}

/*
 * initRaise = 0 by default, and is only different on the pre-flop round
 */
void Table::playRound(unsigned long startPlayer, int initRaise/* = 0*/) {
	lastRaise = (unsigned long) initRaise;
	rounds++;

	// If pre-flop, big blind has already payed in, so do not go to him (essentially go to startPlayer - 1)
	lastPlayerRaised = (initRaise == 0 ? startPlayer : lastPlayerRaised);

	if (output)
		std::cout << "Starting betting\n\n";

	long long playerAction = -2;

	int count = 0;
	for (size_t i = 0; i < numOfPlayers; i++) if (players.at(i)->isPlaying()) count++;
	if (count != activePlayers) {
		std::cout << "Error in activPlayers in play, out: " << activePlayers << " " << count << std::endl;
		exit(13);
	}

	/*
	 * Iterates through the players. If round is pre-flop, start from player after small blind, since small blind antes
	 * by default, so their play is done. If round is not pre-flop, start from small blind.
	 * Keep iterating until all players have either checked the last raise made, or folded.
	 */
	for (size_t p = startPlayer; p != lastPlayerRaised || playerAction == -2; p = (p + 1) % numOfPlayers) {
//        try {
//            players.at(p)->isPlaying();
//        } catch (std::exception &e) {
//            std::cout << "\nP: " << p << std::endl;
//            std::cout << "Players size: " << players.size() << std::endl;
//            std::cout << "PrevP: " << prevP << std::endl;
//            exit(12);
//        }
		if (players.at(p)->isPlaying() && !players.at(p)->isAllIn()) {
			std::vector<double> tableInfo(activePlayers - 1 + 4);
			getTableInfo(tableInfo, p); // Table info to pass to player, see getTableInfo()
			playerAction = players.at(p)->play(tableInfo); // Get what the player decided to do
			if (playerAction == -1) { // Fold
				folds++;
				activePlayers--;
				count = 0;
				for (size_t i = 0; i < numOfPlayers; i++) if (players.at(i)->isPlaying()) count++;
				if (count != activePlayers) {
					std::cout << "Error in activPlayers in play: " << activePlayers << " " << count << std::endl;
					exit(14);
				}
				if (activePlayers == 1)
					return;
			} else if (playerAction > lastRaise) { // Raise
				raises++;
				lastRaise = (unsigned long) playerAction;
				lastPlayerRaised = p;
			} else
				checks++;
		}
	}
	/*
	 * For all players that have not folded, remove the amount bet, and add it to the pot.
	 * If a player does not have enough to play but decided to stay in, treat it as all in
	 * (this is done in the Player class).
	 *
	 * Second portion of if statement prevents big blind from paying in twice (init raise equals zero on all post-flop
	 * rounds)
	 */
	try {
		for (size_t p = 0; p < numOfPlayers; p++)
			if (players.at(p)->isPlaying() && (initRaise != 0 ? players.at(p) != bigBlind : true))
				pot += players.at(p)->anteUp(lastRaise);
	} catch (std::exception &e) {
		std::cout << "Play 2" << std::endl;
	}
}

void Table::splitPot() {
	if (activePlayers == 0) {
		std::cout << "Shouldn't happen, or FPE" << std::endl;
		return;
	}
	double bestHand = 0;
	unsigned long winners = activePlayers;

	std::vector<double> bestHands(activePlayers);
	try {
		// Compare hands, and determine which is the best
		unsigned long activePlayer = 0;
		for (size_t p = 0; p < numOfPlayers; p++) {
			if (players.at(p)->isPlaying()) {
				bestHands.at(activePlayer) = players.at(p)->hand.getHandScore(communityCards);
				if (bestHands.at(activePlayer) > bestHand)
					bestHand = bestHands.at(activePlayer);
				activePlayer++;
			}
		}
	} catch (const std::exception &e) {
		std::cout << "Split 1" << std::endl;
	}

	try {
		// Erase all players with hands worse than the best hand from the pool of potential victors
		for (size_t ap = 0; ap < activePlayers; ap++) {
			if (bestHands.at(ap) < bestHand) {
				bestHands.at(ap) = 0.0;
				winners--;
			}
		}
	} catch (const std::exception &e) {
		std::cout << "Split 2" << std::endl;
	}

	if (winners == 0) {
		std::cout << "FPE would be thrown" << std::endl; // TODO: if this shows, check logic... this should not happen
		std::cout << bestHand << std::endl;
		for (size_t i = 0; i < bestHands.size(); i++)std::cout << bestHands.at(i) << " ";
		std::cout << std::endl;
		exit(12);
	}

	try {
		// Split pot amongst winners (if more than one player has the winning hand)
		for (size_t wp = 0; wp < activePlayers; wp++) {
			if (bestHands.at(wp) != 0.0) {
				players.at(wp)->collectWinnings(pot / winners);
				if (output)
					std::cout << players.at(wp)->getName() << " won" << std::endl << std::endl;
			}
		}
	} catch (const std::exception &e) {
		std::cout << "Split 3" << std::endl;
	}
}

void Table::getTableInfo(std::vector<double> &tableInfo, unsigned long currPlayer) {
	tableInfo.at(0) = lastRaise;
	tableInfo.at(1) = pot;
	tableInfo.at(2) = activePlayers - 1; // Alternatively use numOfPlayers
	tableInfo.at(3) = players.at(currPlayer)->getHandPotential(communityCards);
	unsigned long opp = 0;
	for (size_t p = 0; p < numOfPlayers; p++)
		try {
			if (players.at(p)->isPlaying() && p != currPlayer) { // Write the money each player has, for NN evaluation
				tableInfo.at(opp + 4) = players.at(p)->getMoney();
				opp++;
			}
		} catch (std::out_of_range &e) {
			std::cout << "\nP: " << p << std::endl;
			std::cout << "Opp: " << opp << std::endl;
			std::cout << "TableInfo: " << tableInfo.size() << std::endl;
			std::cout << "ActivePlayers: " << activePlayers << std::endl;
			for (size_t i = 0; i < numOfPlayers; i++) std::cout << players.at(i)->isPlaying() << " ";
			std::cout << std::endl;
//            for (int i = 0; i < numOfPlayers; i++) std::cout << players.at(i) << " ";
//            std::cout << std::endl;
			for (size_t i = 0; i < numOfPlayers; i++) std::cout << players.at(i)->isAllIn() << " ";
			std::cout << std::endl;
			std::cout << "Player: " << p << std::endl;
			std::cout << "Curr player: " << currPlayer << std::endl;
			std::cout << e.what() << std::endl;
			throw e;
		}
}