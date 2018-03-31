#include <iostream>
#include "../../headers/holdem/Deck.h"
#include "../../headers/holdem/Player.h"
#include "../../headers/holdem/Table.h"
#include "../../headers/test/HoldemTest.h"
#include "../../headers/nn/NNEvolver.h"

int main() {
/*
    Player* players[4] = {new Player("Steve"), new Player("Andy"), new Player("Stu"), new Player("Jack")};
    Table t(players, 4);
    t.play();

    delete players[0];
    delete players[1];
    delete players[2];
    delete players[3];
*/

//    HoldemTest t;
//    t.test();

    NNEvolver nnEvolver;
/*
    Deck dd;
    std::vector<Card*> cards(5);
    for (int i = 0; i < 5; i++)
        cards.at(i) = dd.deal();

    Deck d;
    std::vector<int> vals(9);
    long time = 0;
    long start = std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1000000;
    for (int i = 0; i < 1000000; i++) {
        Hand hand(d.deal(), d.deal());
        hand.recordBestHand(0, cards, vals);
    }

    std::cout << "Calc took: "
              << std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1000000 - start << "ms"
              << std::endl;
              */
    return 0;
}