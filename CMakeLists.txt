cmake_minimum_required(VERSION 3.9)
project(Texas_Holdem_NN)

set(CMAKE_CXX_STANDARD 11)

add_executable(Texas_Holdem_NN src/main.cpp src/Hand.cpp headers/Hand.h src/Deck.cpp headers/Deck.h src/Table.cpp headers/Table.h src/Card.cpp headers/Card.h src/Player.cpp headers/Player.h)