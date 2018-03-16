#include <iostream>
#include "../../headers/holdem/Table.h"

Table::Table(Player** players, int numOfPlayers) {
    if (numOfPlayers < 2) {
        std::cout << "Table must have at least 2 players" << std::endl;
        return;
    }
    this->players = players;
    this->numOfPlayers = numOfPlayers;
}

Table::~Table() {};

void Table::play() {
    for (int r = 0; r < numOfPlayers; r++) {
        this->activePlayers = numOfPlayers;

        // Count how many players are able to play the next round
        for (int p = 0; p < numOfPlayers; p++) {
            players[p]->newRound();
            if (!players[p]->isPlaying())
                activePlayers--;
        }

        if (activePlayers <= 1)
            break;

        // Set special players
        dealer = players[r];
        bool smallBlindSet = false;
        for (int p = 0; p < numOfPlayers; p++) {
            if (players[(r + p) % numOfPlayers]->isPlaying()) {
                if (!smallBlindSet) {
                    smallBlind = players[(r + p) % numOfPlayers];
                    smallBlindSet = true;
                } else {
                    bigBlind = players[(r + p) % numOfPlayers];
                    break;
                }
            }
        }

        int initBet = SB;
        if (smallBlind->getMoney() < SB)
            initBet = SB - smallBlind->getMoney();
        smallBlind->anteUp(initBet);
        pot += initBet;

        initBet = BB;
        if (bigBlind->getMoney() < BB)
            initBet = BB - smallBlind->getMoney();

        bigBlind->anteUp(initBet);
        pot += initBet;

        Deck deck;

        Card* cards[activePlayers * 2];
        for (int p = 0; p < activePlayers * 2; p++)
            cards[p] = deck.deal();

        for (int p = 0; p < activePlayers; p++)
            players[p]->hand = new Hand(cards[p], cards[p + activePlayers]);

//        std::cout << "Starting round\n\n";

        playRound((r + (activePlayers > 2 ? 3 : 2)) % numOfPlayers);

        for (int br = 0; br < 3 && activePlayers > 1; br++) {
            if (br == 0)
                for (int f = 0; f < 3; f++)
                    communityCards[f] = deck.deal();
            else
                communityCards[br + 2] = deck.deal();
            playRound((r + 1) % numOfPlayers);
        }

        if (activePlayers > 1) {
            for (int i = 0; i < 5; i++)
                communityCards[i]->getSuitSymbol();

            int bestType = HC;

            // Compare hands
            std::vector<int*> bestHands;
            int activPlayer = 0;
            for (int p = 0; p < numOfPlayers; p++) {
                if (players[p]->isPlaying()) {
                    bestHands.push_back(new int[9]);
                    players[p]->hand->recordBestHand(p, communityCards, bestHands.at(activPlayer++));
                    if (bestHands.back()[1] < bestType)
                        bestType = bestHands.back()[1];
                }
            }

            // Erase all players with hands worse than the best type from the pool of potential victors
            for (ulong ap = 0; ap < bestHands.size();) {
                if (bestHands.at(ap)[1] > bestType) {
                    delete [] bestHands.at(ap);
                    bestHands.erase(bestHands.begin() + ap);
                } else
                    ap++;
            }

            // If players have same hand type, compare values to determine winner
            for (int hs = 2; bestHands.size() > 1 && hs < 9; hs++)
                for (ulong h = 0; h < bestHands.size() - 1;) {
                    if (bestHands.at(h)[hs] > bestHands.at(h + 1)[hs]) {
                        delete [] bestHands.at(h + 1);
                        bestHands.erase(bestHands.begin() + h + 1);
                    } else if (bestHands.at(h)[hs] < bestHands.at(h + 1)[hs]) {
                        delete [] bestHands.at(h);
                        bestHands.erase(bestHands.begin() + h);
                    } else
                        h++;
                }

            // Split pot amongst winners (if more than one player has the winning hand)
            for (int wp = 0; wp < bestHands.size(); wp++) {
                players[bestHands.at(wp)[0]]->collectWinnings(pot / (bestHands.size() - wp));
                pot -= pot / (bestHands.size() - wp);
            }

            for (int i = 0; i < bestHands.size(); i++)
                delete [] bestHands.at(i);
        } else { // Last standing player takes all the cash
            for (int p = 0; p < numOfPlayers; p++)
                if (players[p]->isPlaying())
                    players[p]->collectWinnings(pot);
        }

        for (int p = 0; p < numOfPlayers; p++)
            delete players[p]->hand;
    }
}

void Table::playRound(int startPlayer) {
    int p = startPlayer;
    lastRaise = 0;
    lastPlayerRaised = startPlayer;

//    std::cout << "Starting betting\n\n";

    do {
        if (players[p]->isPlaying()) {
            int tableInfo[activePlayers - 1 + 4];
            getTableInfo(tableInfo, players[p]);
            int opt = players[p]->play(tableInfo);
            if (opt == -1) {
                activePlayers--;
                if (activePlayers == 1)
                    return;
                for (int np = 0; np < numOfPlayers; np++)
                    if (players[(p + numOfPlayers - np) % numOfPlayers]->isPlaying())
                        lastPlayerRaised = (p + np) % numOfPlayers;
            }
            else if (opt > lastRaise) { // TODO: NEEDS REVIEWING
                lastRaise = opt;
                lastPlayerRaised = p;
            }
        }

        p = (p + 1) % numOfPlayers;
    } while (p != lastPlayerRaised && activePlayers > 1);

//    std::cout << "Betting finished\n\n";

    for (p = 0; p < numOfPlayers; p++)
        if (players[p]->isPlaying()) {
            players[p]->anteUp(lastRaise);
            pot += lastRaise;
        }
}

void Table::getTableInfo(int tableInfo[], Player* currPlayer) {
    tableInfo[0] = lastRaise;
    tableInfo[1] = pot;
    tableInfo[2] = activePlayers - 1; // Alternatively use numOfPlayers
    tableInfo[3] = 0; // To be filled in by AIPlayer
    int opp = 0;
    for (int p = 0; opp != activePlayers - 1; p++)
        if (players[p]->isPlaying() && players[p] != currPlayer)
            tableInfo[opp++ + 4] = players[p]->getMoney();
}