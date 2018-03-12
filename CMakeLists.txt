cmake_minimum_required(VERSION 3.9)
project(Texas_Holdem_NN)

set(CMAKE_CXX_STANDARD 11)

add_executable(Texas_Holdem_NN src/holdem/main.cpp src/holdem/Hand.cpp headers/holdem/Hand.h src/holdem/Deck.cpp headers/holdem/Deck.h src/holdem/Table.cpp headers/holdem/Table.h src/holdem/Card.cpp headers/holdem/Card.h src/holdem/Player.cpp headers/holdem/Player.h src/test/HoldemTest.cpp headers/test/HoldemTest.h src/nn/NeuralNetwork.cpp headers/nn/NeuralNetwork.h)