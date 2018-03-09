#include <iostream>
#include "../headers/Table.h"

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
                    flop[f] = deck.deal();
            else
                flop[2 + br] = deck.deal();
            playRound(r + 1u);
        }

        // show();

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
            int opt = players[p]->playTurn(lastRaise);
            if (opt == -1) {
                activePlayers--;
                for (int np = 0; np < numOfPlayers; np++)
                    if (players[(p + numOfPlayers - np) % numOfPlayers]->isPlaying())
                        lastPlayerRaised = (p + np) % numOfPlayers;
            }
//            if (opt == 0)
//                pot += lastRaise;
            else if (opt > 0) {
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