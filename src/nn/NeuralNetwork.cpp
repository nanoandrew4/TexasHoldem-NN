#include <chrono>
#include <fstream>
#include "../../headers/nn/NeuralNetwork.h"

std::mt19937_64 mt_rand(std::chrono::high_resolution_clock::now().time_since_epoch().count());

NeuralNetwork::NeuralNetwork(int layers, int neuronsPerLayer[], bool randomize) {
    activ = new double*[layers];
    weights = new double**[layers];
    this->layers = layers;
    this->neuronsPerLayer = neuronsPerLayer;

    // Initialize neurons
    for (int l = 0; l < layers; l++)
        activ[l] = new double[neuronsPerLayer[l]];

    // Initialize weights connecting the various neurons randomly (between ~0-1)
    for (int l = 0; l < layers - 1; l++) {
        weights[l] = new double *[neuronsPerLayer[l]];
        for (int n = 0; n < neuronsPerLayer[l]; n++) {
            weights[l][n] = new double[neuronsPerLayer[l + 1]];
            if (randomize) {
                for (int nn = 0; nn < neuronsPerLayer[l + 1]; nn++)
                    weights[l][n][nn] = (mt_rand() % 100) / (double) (mt_rand() % 999 + 1);
            }
        }
    }
}

NeuralNetwork::~NeuralNetwork() {
    for (int l = 0; l < layers; l++)
        delete [] activ[l];
    delete [] activ;

    for (int l = 0; l < layers - 1; l++) {
        for (int n = 0; n < neuronsPerLayer[l]; n++)
            delete [] weights[l][n];
        delete [] weights[l];
    }
    delete [] weights;
}

void NeuralNetwork::cloneNetworkInto(NeuralNetwork* nNet) {
    for (int l = 0; l < layers - 1; l++)
        for (int n = 0; n < neuronsPerLayer[l]; n++)
            for (int nn = 0; nn < neuronsPerLayer[l + 1]; nn++)
                nNet->setWeightAt(l, n, nn, weights[l][n][nn]);
}

NeuralNetwork* NeuralNetwork::cloneNetworkStructure(bool randomize) {
    return new NeuralNetwork(layers, neuronsPerLayer, randomize);
}

void NeuralNetwork::serialize(std::string fileName) {
    std::ofstream out(fileName);

    out << layers << " ";
    for (int l = 0; l < layers; l++)
        out << neuronsPerLayer[l] << " ";

    for (int l = 0; l < layers - 1; l++)
        for (int n = 0; n < neuronsPerLayer[l]; n++)
            for (int nn = 0; nn < neuronsPerLayer[l + 1]; nn++)
                out << weights[l][n][nn] << " ";
    out.close();
}

NeuralNetwork* NeuralNetwork::deserialize(std::string fileName) {
    std::ifstream in(fileName);
    int layers;
    in >> layers;

    int neuronsPerLayer[layers];
    for (int l = 0; l < layers; l++)
        in >> neuronsPerLayer[l];

    NeuralNetwork* nNet = new NeuralNetwork(layers, neuronsPerLayer, false);

    for (int l = 0; l < layers - 1; l++)
        for (int n = 0; n < neuronsPerLayer[l]; n++)
            for (int nn = 0; nn < neuronsPerLayer[l + 1]; nn++)
                in >> nNet->weights[l][n][nn];
    in.close();
}

double NeuralNetwork::sigmoid(double d) {
    return (1.0 / (1.0 + exp(-d)));
}

double NeuralNetwork::invSigmoid(double d) {
    return -log((1 / d) - 1);
}

void NeuralNetwork::forward(double input[]) {

    // Zero all activations in preparation for data ingestion
    for (int l = 0; l < layers; l++)
        for (int n = 0; n < neuronsPerLayer[l]; n++)
            activ[l][n] = 0;

    // Copy input data to input layer
    for (int n = 0; n < neuronsPerLayer[0]; n++)
        activ[0][n] = sigmoid(input[n]);

    // Forward propagate to all layers
    for (int l = 1; l < layers; l++) {
        for (int n = 0; n < neuronsPerLayer[l]; n++) {
            for (int pn = 0; pn < neuronsPerLayer[l - 1]; pn++)
                activ[l][n] += activ[l - 1][pn] * weights[l - 1][pn][n];
            activ[l][n] = sigmoid(activ[l][n]);
        }
    }
}

int NeuralNetwork::getAction(double input[]) {
    forward(input);

    int maxIndex = 0;
    for (int n = 1; n < neuronsPerLayer[layers - 1]; n++)
        if (activ[layers - 1][n] > activ[layers - 1][maxIndex])
            maxIndex = n;

    if (maxIndex == 2)
        return (int)(invSigmoid(activ[layers - 1][maxIndex]));

    return maxIndex - 1;
}