#include <iostream>
#include "../../headers/holdem/Table.h"

Table::Table(Player** players, uint numOfPlayers) {
    if (numOfPlayers < 2) {
        std::cout << "Table must have at least 2 players" << std::endl;
        return;
    }
    this->players = players;
    this->numOfPlayers = numOfPlayers;
}

Table::~Table() = default;

void Table::play() {
    for (int r = 0; r < numOfPlayers; r++) {
        this->activePlayers = numOfPlayers;

        for (int p = 0; p < numOfPlayers; p++) {
            players[p]->newRound();
            if (!players[p]->isPlaying())
                activePlayers--;
        }

        if (activePlayers == 1)
            break;
        else if (activePlayers < 1) {
            std::cout << "Table::play gotcha!" << std::endl;
            exit(2);
        }

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
        if (smallBlind->getMoney() > SB)
            initBet = SB - smallBlind->getMoney();
        smallBlind->anteUp(initBet);
        pot += initBet;

        initBet = BB;
        if (bigBlind->getMoney() > BB)
            initBet = BB - smallBlind->getMoney();

        bigBlind->anteUp(initBet);
        pot += initBet;

        Deck deck;

        Card* cards[numOfPlayers * 2];
        for (int p = 0; p < numOfPlayers * 2; p++)
            cards[p] = deck.deal();

        for (int p = 0; p < numOfPlayers; p++)
            players[p]->hand = new Hand(cards[p], cards[p + numOfPlayers]);

        std::cout << "Starting round\n\n";

        playRound((r + (numOfPlayers > 2 ? 3u : 2u)) % numOfPlayers);

        for (int br = 0; br < 3 && activePlayers > 1; br++) {
            if (br == 0)
                for (int f = 0; f < 3; f++)
                    communityCards[f] = deck.deal();
            else
                communityCards[br + 2] = deck.deal();
            playRound(r + 1u);
        }

        int bestType = HC;

        // Compare hands
        std::vector<int*> bestHands;
        int activPlayer = 0;
        for (uint p = 0; p < numOfPlayers; p++) {
            if (players[p]->isPlaying()) {
                bestHands.push_back(new int[9]);
                players[p]->hand->recordBestHand(p, communityCards, bestHands.at(activPlayer++));
                if (bestHands.back()[1] < bestType)
                    bestType = bestHands.back()[1];
            }
        }

        // Active players
        for (ulong ap = 0; ap < bestHands.size();) {
            if (bestHands.at(ap)[1] > bestType)
                bestHands.erase(bestHands.begin() + ap);
            else
                ap++;
        }

        // If players have same hand type, compare values to determine winner
        for (int hs = 2; bestHands.size() > 1 && hs < 9; hs++)
            for (ulong h = 0; h < bestHands.size() - 1;) {
                if (bestHands.at(h)[hs] > bestHands.at(h + 1)[hs])
                    bestHands.erase(bestHands.begin() + h + 1);
                else if (bestHands.at(h)[hs] < bestHands.at(h + 1)[hs])
                    bestHands.erase(bestHands.begin() + h);
                else
                    h++;
            }

        // Split pot
        for (int wp = 0; wp < bestHands.size(); wp++) {
            players[bestHands.at(wp)[0]]->collectWinnings(pot / (bestHands.size() - wp));
            pot -= pot / (bestHands.size() - wp);
        }

        for (int p = 0; p < numOfPlayers; p++)
            delete players[p]->hand;
    }
}

void Table::playRound(uint startPlayer) {
    int p = startPlayer;
    lastRaise = 0;
    lastPlayerRaised = startPlayer;

    std::cout << "Starting betting\n\n";

    do {
        if (players[p]->isPlaying()) {
            int tableInfo[activePlayers - 1 + 4];
            getTableInfo(tableInfo, players[p]);
            int opt = players[p]->play(tableInfo);
            if (opt == -1) {
                activePlayers--;
                for (int np = 0; np < numOfPlayers; np++)
                    if (players[(p + numOfPlayers - np) % numOfPlayers]->isPlaying())
                        lastPlayerRaised = (p + np) % numOfPlayers;
            }
            else if (opt > lastRaise) { // TODO: NEEDS REVIEWING
                lastRaise = (uint) opt;
                lastPlayerRaised = (uint) (p);
            }
        }

        p = (p + 1) % numOfPlayers;
    } while (p != lastPlayerRaised && activePlayers > 1);

    std::cout << "Betting finished\n\n";

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
    for (int p = 0; p < numOfPlayers; p++)
        if (players[p]->isPlaying() && players[p] != currPlayer)
            tableInfo[opp++] = players[p]->getMoney();
}