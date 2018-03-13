#include <iostream>
#include "../../headers/holdem/Player.h"

Player::Player() {}

Player::~Player() = default;

int Player::play(int tableInfo[]) {
    std::cout << "Your hand: "; hand->displayHand(hand->pocket);
    char opt = ' ';

    do {
        std::cout << "Enter 'f' to fold, 'c' to call or 'r' to raise" << std::endl;
        std::cin >> opt;
    } while (opt != 'f' && opt != 'c' && opt != 'r');

    switch (opt) {
        case 'r':
            std::cout << "Enter how much you would like to raise by: $";
            uint amount;
            std::cin >> amount;
            while (amount > money) {
                std::cout << "You do not have enough to make that bet, try again: $";
                std::cin >> amount;
            }
            return amount;
        case 'c':
            if (money > tableInfo[0])
                return 0;
        case 'f':
            playing = false;
            return -1;
    }

    std::cout << std::endl;
}

void Player::anteUp(int ante) {
    this->money -= ante;
}

void Player::collectWinnings(uint winnings) {
    money += winnings;
}

uint Player::getMoney() {
    return money;
}

void Player::newRound() {
    this->playing = money > 0;
}