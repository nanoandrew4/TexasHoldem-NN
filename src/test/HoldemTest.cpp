#include <iostream>
#include "../../headers/test/HoldemTest.h"
#include "../../headers/holdem/Hand.h"
#include "../../headers/holdem/Player.h"
#include "../../headers/nn/AIPlayer.h"
#include "../../headers/holdem/Table.h"

void HoldemTest::test() {
    Suit SPADES = Suit::SPADES, CLUBS = Suit::CLUBS, HEARTS = Suit::HEARTS, DIAMONDS = Suit::DIAMONDS;

    std::vector<Card*> communityCards = {new Card(SPADES, 5), new Card(HEARTS, 5), new Card(HEARTS, 2),
                      new Card(HEARTS, 3), new Card(HEARTS, 4)};

    const int handNum = 8;
    Hand* hands[handNum] = {
                       new Hand(new Card(SPADES, 2), new Card(HEARTS, 10)), // Flush
                       new Hand(new Card(DIAMONDS, 8), new Card(DIAMONDS, 5)), // TOAK
                       new Hand(new Card(CLUBS, 12), new Card(DIAMONDS, 12)), // TP
                       new Hand(new Card(CLUBS, 5), new Card(DIAMONDS, 5)), // FOAK
                       new Hand(new Card(HEARTS, 14), new Card(CLUBS, 14)), // SF
                       new Hand(new Card(CLUBS, 6), new Card(CLUBS, 7)), // STRAIGHT
                       new Hand(new Card(CLUBS, 13), new Card(SPADES, 13)), // PAIR (comm cards)
                       new Hand(new Card(SPADES, 4), new Card(DIAMONDS, 5)) // FH
    };

//    int scores[8][9];
//    for (int h = 0; h < 8; h++)
//        hands[h]->recordBestHand(h, communityCards, scores[h]);

    int bestType = HC;

    // Compare hands
    std::vector<std::vector<int>> bestHands(handNum);
    int activPlayer = 0;
    for (uint p = 0; p < handNum; p++) {
        hands[p]->recordBestHand(p, communityCards, bestHands.at(activPlayer++));
        if (bestHands.back()[1] < bestType)
            bestType = bestHands.back()[1];
    }

    // Erase all players with hands worse than the best type from the pool of potential victors
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

    std::cout << "Winning hands are: " << std::endl;
    for (int wp = 0; wp < bestHands.size(); wp++)
        hands[bestHands.at(wp)[0]]->displayHand();

//    Player* players[5];
//    for (int p = 0; p < 5; p++)
//        players[p] = new AIPlayer();
//
//    Table t(players, 5);
//    t.play();


    for (int h = 0; h < 5; h++) {
        delete communityCards[h];
//        delete players[h];
    }

    for (int h = 0; h < handNum; h++)
        delete hands[h];
}