#include <chrono>
#include <iostream>
#include <fstream>
#include "../../headers/nn/NeuralNetwork.h"

std::mt19937_64 mt_rand(std::chrono::high_resolution_clock::now().time_since_epoch().count());

NeuralNetwork::NeuralNetwork(std::vector<int>& neuronsPerLayer, bool randomize) {
    if (neuronsPerLayer.size() < 2) {
        std::cout << "Invalid network with: " << neuronsPerLayer.size() << " layers" << std::endl;
        exit(1);
    }

    activ.resize(neuronsPerLayer.size());
    weights.resize(neuronsPerLayer.size() - 1);
    this->neuronsPerLayer = neuronsPerLayer;

    // Initialize neurons
    for (int l = 0; l < neuronsPerLayer.size(); l++)
        activ.at(l).resize(neuronsPerLayer.at(l));

    // Initialize weights connecting the various neurons randomly (between ~0-1)
    for (int l = 0; l < neuronsPerLayer.size() - 1; l++) {
        weights.at(l).resize(neuronsPerLayer.at(l));
        for (int n = 0; n < neuronsPerLayer.at(l); n++) {
            weights.at(l).at(n).resize(neuronsPerLayer.at(l + 1));
            if (randomize) {
                for (int nn = 0; nn < neuronsPerLayer.at(l + 1); nn++)
                    weights.at(l).at(n).at(nn) = (mt_rand() % 100) / (double) (mt_rand() % 999 + 1);
            }
        }
    }
}

NeuralNetwork::~NeuralNetwork() {}


void NeuralNetwork::cloneNetworkInto(NeuralNetwork* nNet) {
    for (int l = 0; l < neuronsPerLayer.size() - 1; l++)
        for (int n = 0; n < neuronsPerLayer.at(l); n++)
            for (int nn = 0; nn < neuronsPerLayer.at(l + 1); nn++)
                nNet->setWeightAt(l, n, nn, weights.at(l).at(n).at(nn));
}

NeuralNetwork* NeuralNetwork::cloneNetworkStructure(bool randomize) {
    return new NeuralNetwork(neuronsPerLayer, randomize);
}

void NeuralNetwork::serialize(std::string fileName) {
    std::ofstream out(fileName);

    out << neuronsPerLayer.size() << " ";
    for (int l = 0; l < neuronsPerLayer.size(); l++)
        out << neuronsPerLayer.at(l) << " ";

    for (int l = 0; l < neuronsPerLayer.size() - 1; l++)
        for (int n = 0; n < neuronsPerLayer.at(l); n++)
            for (int nn = 0; nn < neuronsPerLayer.at(l + 1); nn++)
                out << weights.at(l).at(n).at(nn) << " ";
    out.close();
}

NeuralNetwork* NeuralNetwork::deserialize(const std::string& fileName) {
    std::ifstream* in = new std::ifstream(fileName);

    std::string file;
    while (!in->is_open()) {
        std::cout << "Error deserializing agent file, please try entering the path to the file again... ";
        std::cin >> file;

        delete in;
        in = new std::ifstream(file);
    }

    int layers;
    *in >> layers;

    std::vector<int> neuronsPerLayer(layers);
    for (int l = 0; l < layers; l++)
        *in >> neuronsPerLayer.at(l);

    NeuralNetwork* nNet = new NeuralNetwork(neuronsPerLayer, false);

    for (int l = 0; l < layers - 1; l++)
        for (int n = 0; n < neuronsPerLayer.at(l); n++)
            for (int nn = 0; nn < neuronsPerLayer.at(l + 1); nn++)
                *in >> nNet->weights.at(l).at(n).at(nn);
    in->close();

    delete in;
    return nNet;
}

double NeuralNetwork::sigmoid(double d) {
    return (1.0 / (1.0 + exp(-d)));
}

double NeuralNetwork::invSigmoid(double d) {
    return -log((1 / d) - 1);
}

void NeuralNetwork::forward(std::vector<double> input) {

    // Zero all activations in preparation for data ingestion
    for (int l = 0; l < neuronsPerLayer.size(); l++)
        for (int n = 0; n < neuronsPerLayer.at(l); n++)
            activ.at(l).at(n) = 0;

    // Copy input data to input layer
    for (int n = 0; n < neuronsPerLayer.at(0); n++)
        activ.at(0).at(n) = sigmoid(input.at(n));

    // Forward propagate to all layers
    for (int l = 1; l < neuronsPerLayer.size(); l++) {
        for (int n = 0; n < neuronsPerLayer.at(l); n++) {
            for (int pn = 0; pn < neuronsPerLayer.at(l - 1); pn++)
                activ.at(l).at(n) += activ.at(l - 1).at(pn) * weights.at(l - 1).at(pn).at(n);
            activ.at(l).at(n) = sigmoid(activ.at(l).at(n));
        }
    }
}

int NeuralNetwork::getAction(const std::vector<double> &input) {
    forward(input);

    int maxIndex = 0;
    for (int n = 1; n < neuronsPerLayer.back(); n++)
        if (activ.back().at(n) > activ.back().at(maxIndex))
            maxIndex = n;

    if (maxIndex == 2)
        return (int)(invSigmoid(activ.back().at(maxIndex)));

    return maxIndex - 1;
}