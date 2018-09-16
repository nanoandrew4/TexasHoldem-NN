#include <iostream>
#include <thread>
#include "../../headers/holdem/Deck.h"
#include "../../headers/holdem/Player.h"
#include "../../headers/holdem/Table.h"
#include "../../headers/nn/NNEvolver.h"
#include "../../headers/test/DeckTest.h"
#include "../../headers/test/HandTest.h"

int main(int argc, char *argv[]) {
	if (argc > 1) {
		NNEvolver evolver(1000, 1000, 2, std::thread::hardware_concurrency(), 0, 10, 0, 0);
		evolver.train();
		return 0;
	}

	do {
		std::cout << "Welcome to TexasHoldemNN - Where you can play your computer in a game of Texas Hold'em poker"
		          << std::endl;
		std::cout << "FYI, your name is now Steve. Get used to it!" << std::endl << std::endl;
		std::cout << "1. Play against AI agent" << std::endl;
		std::cout << "2. Train AI agent(s)" << std::endl;
		std::cout << "3. Test game logic integrity" << std::endl;
		std::cout << "0. Exit" << std::endl << std::endl;

		int opt;
		std::string file;

		do {
			std::cout << ">> ";
			std::cin >> opt;
		} while (opt < 0 || opt > 3);

		std::cin.clear();

		if (opt == 1) {
			std::cout << "Enter number of agents to play against: ";
			std::cin >> opt;

			std::vector<Player *> players;
			players.push_back(new Player("Steve"));

			std::cout << "Enter the paths to the \".dat\" files to load the agents from" << std::endl;

			for (int i = 0; i < opt; i++) {
				std::cin >> file;
				players.push_back(new AIPlayer(file, "AI"));
			}

			Table table(players);
			Table::output = true;
			table.playUntilOneLeft();
			Table::output = false;
		} else if (opt == 2) {
			NNEvolver evolver;
			evolver.train();
		} else if (opt == 3) {
			std::cout << "Enter number of times to run each test: ";
			std::cin >> opt;
			std::cout << "Testing... If any anomalies occur, they will be reported. "
			          << "If there is no output, code has passed all tests." << std::endl;
			for (int i = 0; i < opt; i++) {
				DeckTest::test();
				HandTest::test();
			}
			std::cout << std::endl << std::endl;
		} else if (opt == 0)
			break;

		std::cin.clear();
	} while (true);

	return 0;
}