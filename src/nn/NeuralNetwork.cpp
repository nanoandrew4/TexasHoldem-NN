#include <chrono>
#include "../../headers/nn/NeuralNetwork.h"

std::mt19937_64 mt_rand(std::chrono::high_resolution_clock::now().time_since_epoch().count());

NeuralNetwork::NeuralNetwork(int neuronsPerLayer[], const int layers) {
    activ = new double*[layers];
    weights = new double**[layers];
    this->layers = layers;
    this->neuronsPerLayer = neuronsPerLayer;

    // Initialize neurons
    for (int i = 0; i < layers; i++)
        activ[i] = new double[neuronsPerLayer[i]];

    // Initialize weights connecting the various neurons randomly (between ~0-1)
    for (int i = 0; i < layers - 1; i++) {
        weights[i] = new double *[neuronsPerLayer[i]];
        for (int l = 0; l < neuronsPerLayer[i]; l++) {
            weights[i][l] = new double[neuronsPerLayer[l + 1]];
            for (int nl = 0; nl < neuronsPerLayer[i + 1]; nl++)
                weights[i][l][nl] = 1.0 / (double)(mt_rand() % 100000 + 1);
        }
    }
}

double NeuralNetwork::sigmoid(double d) {
    return (1.0 / (1.0 + exp(-d)));
}

void NeuralNetwork::forward(double input[]) {

    // Zero all activations in preparation for data ingestion
    for (int l = 0; l < layers; l++)
        for (int n = 0; n < neuronsPerLayer[l]; n++)
            activ[l][n] = 0;

    // Copy input data to input layer
    for (int n = 0; n < neuronsPerLayer[0]; n++)
        activ[0][n] = input[n];

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

    return maxIndex;
}