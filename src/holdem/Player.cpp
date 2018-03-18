#include <iostream>
#include "../../headers/holdem/Player.h"

Player::Player() {}

Player::Player(std::string name) {
    this->name = name;
}

Player::~Player() = default;

int Player::play(int tableInfo[]) {
    std::cout << name << "'s hand: "; hand->displayHand(hand->pocket);
    char opt = ' ';

    do {
        std::cout << "Enter 'f' to fold, 'c' to call or 'r' to raise: ";
        std::cin >> opt;
    } while (opt != 'f' && opt != 'c' && opt != 'r');

    switch (opt) {
        case 'r':
            std::cout << "Enter how much you would like to raise by (Current: " << tableInfo[0] << "): $";
            int amount;
            std::cin >> amount;
            while (amount > money || amount < tableInfo[0]) {
                if (amount <= tableInfo[0])
                    std::cout << "Your raise is not bigger than the current bet, try again: $";
                else if (amount > money)
                    std::cout << "You do not have enough to make that bet, try again: $";
                std::cin >> amount;
            }
            return amount;
        case 'c':
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

void Player::collectWinnings(int winnings) {
    money += winnings;
}

int Player::getMoney() {
    return money;
}

void Player::newRound() {
    this->playing = money > 0;
}