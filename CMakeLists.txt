cmake_minimum_required(VERSION 3.9)
project(Texas_Holdem_NN)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS_DEBUG "-Og")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

add_executable(Texas_Holdem_NN src/holdem/main.cpp src/holdem/Hand.cpp headers/holdem/Hand.h src/holdem/Deck.cpp headers/holdem/Deck.h src/holdem/Table.cpp headers/holdem/Table.h src/holdem/Card.cpp headers/holdem/Card.h src/holdem/Player.cpp headers/holdem/Player.h src/nn/NeuralNetwork.cpp headers/nn/NeuralNetwork.h src/nn/NNEvolver.cpp headers/nn/NNEvolver.h src/nn/AIPlayer.cpp headers/nn/AIPlayer.h src/test/DeckTest.cpp headers/test/DeckTest.h src/test/HandTest.cpp headers/test/HandTest.h headers/gen_algo/GeneticAlgorithm.h src/gen_algo/ElitistCombination.cpp headers/gen_algo/ElitistCombination.h src/gen_algo/PairCrossover.cpp headers/gen_algo/PairCrossover.h)