#include <chrono>
#include "../../headers/nn/NeuralNetwork.h"

std::mt19937_64 mt_rand(std::chrono::high_resolution_clock::now().time_since_epoch().count());

NeuralNetwork::NeuralNetwork(int neuronsPerLayer[], int layers) {
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
            for (int nn = 0; nn < neuronsPerLayer[l + 1]; nn++)
                weights[l][n][nn] = 1.0 / (double)(mt_rand() % 100000 + 1);
        }
    }
}

NeuralNetwork::~NeuralNetwork() {
    for (int l = 0; l < layers; l++)
        delete activ[l];
    delete activ;

    delete neuronsPerLayer;

    for (int l = 0; l < layers - 1; l++) {
        for (int n = 0; n < neuronsPerLayer[l]; n++)
            delete weights[l][n];
        delete weights[l];
    }
    delete weights;
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