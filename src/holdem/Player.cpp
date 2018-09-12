#include <iostream>
#include "../../headers/holdem/Player.h"

Player::Player() = default;

Player::Player(std::string name) {
    this->name = std::move(name);
}

Player::~Player() = default;

int Player::play(std::vector<double> tableInfo) {
    std::cout << name << "'s hand: "; hand.displayHand();
    std::cout << name << "'s money: $" << money << std::endl;
    char opt = ' ';

    do {
        std::cout << "Enter 'f' to fold, 'c' to call or 'r' to raise: ";
        std::cin >> opt;
    } while (opt != 'f' && opt != 'c' && opt != 'r');

    switch (opt) {
        case 'r':
            std::cout << "Enter how much you would like to raise by (Current: " << tableInfo.at(0) << "): $";
            int amount;
            std::cin >> amount;
            while (amount > money || amount < tableInfo.at(0)) {
                if (amount <= tableInfo.at(0))
                    std::cout << "Your raise is not bigger than the current bet, try again: $";
                else if (amount > money)
                    std::cout << "You do not have enough to make that bet, try again: $";
                std::cin >> amount;
            }
            return amount;
        case 'c':
            if (money > 0 || allIn)
                return 0;
            return -1; // Force fold if cannot afford to continue
        case 'f':
            playing = false;
            return -1;
    }

    std::cout << std::endl;
}

unsigned long Player::anteUp(unsigned long ante) {
    if (ante >= money) {
        unsigned long anteAvail = ante - money;
        this->money = 0;
        allIn = true;
        return anteAvail;
    } else {
        this->money -= ante;
        return ante;
    }
}

void Player::collectWinnings(unsigned long winnings) {
    money += winnings;
}

int Player::getMoney() {
    return money;
}

void Player::newRound() {
    this->playing = (money > 0);
    allIn = false;
}