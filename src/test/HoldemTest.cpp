#include "../../headers/test/HoldemTest.h"
#include "../../headers/holdem/Hand.h"
#include "../../headers/holdem/Player.h"
#include "../../headers/nn/AIPlayer.h"
#include "../../headers/holdem/Table.h"

void HoldemTest::test() {
    Suit SPADES = Suit::SPADES, CLUBS = Suit::CLUBS, HEARTS = Suit::HEARTS, DIAMONDS = Suit::DIAMONDS;

    Card* flop[5] = {new Card(SPADES, 10), new Card(HEARTS, 5), new Card(HEARTS, 2),
                      new Card(HEARTS, 3), new Card(HEARTS, 4)};

    Hand* hands[5] = {new Hand(new Card(SPADES, 2), new Card(HEARTS, 10)),
                       new Hand(new Card(DIAMONDS, 6), new Card(DIAMONDS, 5)),
                       new Hand(new Card(CLUBS, 12), new Card(DIAMONDS, 12)),
                       new Hand(new Card(HEARTS, 9), new Card(DIAMONDS, 11)),
                       new Hand(new Card(HEARTS, 14), new Card(CLUBS, 14))};

//    int scores[5][9];
//    for (int h = 0; h < 5; h++)
//        hands[h]->recordBestHand(h, communityCards, scores[h]);

    Player* players[5];
    for (int p = 0; p < 5; p++)
        players[p] = new AIPlayer();

    Table t(players, 5);
    t.play();

    for (int h = 0; h < 5; h++) {
        delete hands[h];
        delete flop[h];
        delete players[h];
    }
}