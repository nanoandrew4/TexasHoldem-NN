#include <iostream>
#include "../headers/Deck.h"
#include "../headers/Player.h"
#include "../headers/Table.h"


int main() {
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

    return 0;
}