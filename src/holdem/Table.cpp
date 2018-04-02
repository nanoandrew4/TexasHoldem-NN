#include <iostream>
#include <utility>
#include "../../headers/holdem/Table.h"

Table::Table(std::vector<Player*> players) {
    if (numOfPlayers < 2) {
        std::cout << "Table must have at least 2 players" << std::endl;
        return;
    }
    this->players = std::move(players);
    this->numOfPlayers = players.size();
}

Table::~Table() {};

void Table::play() {
    for (int r = 0; r < numOfPlayers; r++) {
        activePlayers = numOfPlayers;
        pot = 0;

        // Count how many players are able to play the next round
        for (int p = 0; p < numOfPlayers; p++) {
            players.at(p)->newRound();
            if (!players.at(p)->isPlaying())
                activePlayers--;
        }

        if (activePlayers <= 1)
            break;

        // Set special players
        dealer = players.at(r);
//        std::cout << "Dealer: " << dealer->getName() << std::endl;
        bool smallBlindSet = false, bigBlindSet = false;
        for (int p = 1; p < numOfPlayers; p++) {
            if (players.at((r + p) % numOfPlayers)->isPlaying()) {
                if (!smallBlindSet) {
                    lastPlayerRaised = (r + p) % numOfPlayers;
                    smallBlind = players.at(lastPlayerRaised);
                    smallBlindSet = true;

                    if (activePlayers == 2)
                        break;
//                    std::cout << "Small Blind: " << smallBlind->getName() << std::endl;
                } else {
                    lastPlayerRaised = (r + p) % numOfPlayers;
                    bigBlind = players.at(lastPlayerRaised);
                    bigBlindSet = true;
//                    std::cout << "Big Blind: " << bigBlind->getName() << std::endl;
                    break;
                }
            }
        }

        // Small blind ante before round starts (lost to time... Might need fixing)
        int initBet = SB;
        if (smallBlind->getMoney() < SB)
            initBet = SB - smallBlind->getMoney();
        smallBlind->anteUp(initBet);
        pot += initBet;

        if (bigBlindSet) { // Big blind ante before round starts, if enough players to set big blind
            initBet = BB;
            if (bigBlind->getMoney() < BB)
                initBet = BB - smallBlind->getMoney();
            bigBlind->anteUp(initBet);
            pot += initBet;
        }

        Deck deck;

        std::vector<Card*> cards;
        for (int p = 0; p < numOfPlayers * 2; p++)
            cards.push_back(deck.deal());

        for (int p = 0; p < numOfPlayers; p++)
            players.at(p)->hand = new Hand(cards.at(p), cards.at(p + numOfPlayers));

//        std::cout << "Starting round\n\n";

        // Pre-flop
        playRound((lastPlayerRaised + 1) % numOfPlayers, initBet);

        // Return initial ante to small blind if still playing, since they will have committed an excess to the pot
        if (smallBlind->isPlaying() && bigBlindSet) {
            smallBlind->collectWinnings(SB);
            pot -= SB;
        }

//        std::cout << "Pot: " << pot << std::endl << std::endl;
        for (int br = 0; br < 3 && activePlayers > 1; br++) {
            if (br == 0)
                for (int f = 0; f < 3; f++)
                    communityCards.push_back(deck.deal());
            else
                communityCards.push_back(deck.deal());
//            std::cout << "Community cards: ";
//            players.at(0)->hand->displayHand(communityCards, 3+ br);
//            std::cout << std::endl;
            playRound((r + 1) % numOfPlayers);
//            std::cout << "Pot: " << pot << std::endl << std::endl;
        }

        if (activePlayers > 1) { // Determine winner(s) and split pot if necessary
            int bestType = HC;

            // Compare hands
            std::vector<std::vector<int>> bestHands(activePlayers);
            int activPlayer = 0;
            for (int p = 0; p < numOfPlayers; p++) {
                if (players.at(p)->isPlaying()) {
                    bestHands.at(activPlayer).resize(9);
                    players.at(p)->hand->recordBestHand(p, communityCards, bestHands.at(activPlayer));
                    if (bestHands.at(activPlayer).at(1) < bestType)
                        bestType = bestHands.at(activPlayer).at(1);
                    activPlayer++;
                }
            }

            // Erase all players with hands worse than the best type from the pool of potential victors
            for (ulong ap = 0; ap < bestHands.size();) {
                if (bestHands.at(ap).at(1) > bestType)
                    bestHands.erase(bestHands.begin() + ap);
                else
                    ap++;
            }

            // If players have same hand type, compare values to determine winner
            for (int hs = 2; bestHands.size() > 1 && hs < 9; hs++)
                for (ulong h = 0; h < bestHands.size() - 1;) {
                    if (bestHands.at(h).at(hs) > bestHands.at(h + 1).at(hs))
                        bestHands.erase(bestHands.begin() + h + 1);
                    else if (bestHands.at(h).at(hs) < bestHands.at(h + 1).at(hs))
                        bestHands.erase(bestHands.begin() + h);
                    else
                        h++;
                }

            // Split pot amongst winners (if more than one player has the winning hand)
            for (int wp = 0; wp < bestHands.size(); wp++) {
//                std::cout << players.at(bestHands.at(wp).at(0))->getName() << " won" << std::endl << std::endl;
                players.at(bestHands.at(wp).at(0))->collectWinnings(pot / (bestHands.size() - wp));
                pot -= pot / (bestHands.size() - wp);
            }
        } else // Last standing player takes all the cash
            for (int p = 0; p < numOfPlayers; p++)
                if (players.at(p)->isPlaying())
                    players.at(p)->collectWinnings(pot);

        for (int p = 0; p < numOfPlayers; p++)
            delete players.at(p)->hand;
        communityCards.clear();
    }
}

void Table::playRound(int startPlayer, int initRaise) {
    lastRaise = initRaise;

    // If pre-flop, big blind has already payed in, so do not go to him (essentially go to startPlayer - 1)
    lastPlayerRaised = initRaise == 0 ? startPlayer : lastPlayerRaised;

//    std::cout << "Starting betting\n\n";

    int p, opt = -2;
    for (p = startPlayer; p != lastPlayerRaised || opt == -2; p = (p + 1) % numOfPlayers) {
        if (players.at(p)->isPlaying()) {
            std::vector<double> tableInfo(activePlayers - 1 + 4);
            getTableInfo(tableInfo, players.at(p)); // Table info to pass to player
            opt = players.at(p)->play(tableInfo);
            if (opt == -1) { // Fold
                activePlayers--;
                if (activePlayers == 1)
                    return;
            } else if (opt > lastRaise) { // Raise
                lastRaise = opt;
                lastPlayerRaised = p;
            }
        }
    }

    for (p = 0; p < numOfPlayers; p++)
        if (players.at(p)->isPlaying() && (initRaise != 0 ? players.at(p) != bigBlind : true)) {
            int ante = (players.at(p)->getMoney() > lastRaise ? lastRaise : players.at(p)->getMoney()); // Allows for all-in
            players.at(p)->anteUp(ante);
            pot += ante;
        }
}

void Table::getTableInfo(std::vector<double> tableInfo, Player* currPlayer) {
    tableInfo.at(0) = lastRaise;
    tableInfo.at(1) = pot;
    tableInfo.at(2) = activePlayers - 1; // Alternatively use numOfPlayers
    tableInfo.at(3) = currPlayer->getHandPotential(communityCards); // To be filled in by AIPlayer
    int opp = 0;
    for (int p = 0; opp != activePlayers - 1; p++)
        if (players.at(p)->isPlaying() && players.at(p) != currPlayer)
            tableInfo.at(opp++ + 4) = players.at(p)->getMoney();
}