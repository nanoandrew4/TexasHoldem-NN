#include <iostream>
#include "../../headers/holdem/Deck.h"
#include "../../headers/holdem/Player.h"
#include "../../headers/holdem/Table.h"
#include "../../headers/nn/NNEvolver.h"
#include "../../headers/test/DeckTest.h"
#include "../../headers/test/HandTest.h"

int main() {

    std::cout << "Preliminary testing..." << std::endl;
    DeckTest::test();
    HandTest::test();

    std::cout << std::endl;

    do {
        std::cout << "Welcome to TexasHoldemNN - Where you can play your computer in a game of Texas Hold'em poker"
                  << std::endl;
        std::cout << "FYI, your name is now Steve. Get used to it!" << std::endl << std::endl;
        std::cout << "1. Play against AI agent" << std::endl;
        std::cout << "2. Train AI agent(s)" << std::endl;
        std::cout << "0. Exit" << std::endl << std::endl;

        int opt;
        std::string file;

        do {
            std::cout << ">> ";
            std::cin >> opt;
        } while (opt < 0 || opt > 2);

        if (opt == 1) {
            std::cout << "Enter number of agents to play against: ";
            std::cin >> opt;

            std::vector<Player *> players;
            players.push_back(new Player("Steve"));

            std::cout << "Enter the paths to the \".dat\" files to load the agents from" << std::endl;

            for (int i = 0; i < opt; i++) {
                std::cin >> file;
                players.push_back(new AIPlayer());
            }

            Table table(players);
            table.play();
        } else if (opt == 2) {
            std::cout << "Enter population size, generations to evolve, number of parents, game iterations per "
                      << "generation and number of threads to be used" << std::endl;
            NNEvolver evolver(std::cin.get(), std::cin.get(), std::cin.get(), std::cin.get(), std::cin.get());
            std::cout << "Enter filename to save agent data to: ";
            std::cin >> file;
            evolver.setOutFileName(file);
            evolver.evolve();
        } else if (opt == 0)
            break;
    } while (true);

    return 0;
}