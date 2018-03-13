#include <iostream>
#include "../../headers/holdem/Deck.h"
#include "../../headers/holdem/Player.h"
#include "../../headers/holdem/Table.h"
#include "../../headers/test/HoldemTest.h"
#include "../../headers/nn/NNEvolver.h"


int main() {
    /*
    Player* players[4] = {new Player(), new Player(), new Player(), new Player()};
    players[0]->humanPlayer();
    players[1]->humanPlayer();
    players[2]->humanPlayer();
    players[3]->humanPlayer();
    Table t(players, 4);
    t.play();

    delete players[0];
    delete players[1];
    delete players[2];
    delete players[3];
    */

    HoldemTest t;
    t.test();

//    NNEvolver nnEvolver;
    return 0;
}