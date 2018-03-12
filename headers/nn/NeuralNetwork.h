#ifndef TEXAS_HOLDEM_NN_NEURALNETWORK_H
#define TEXAS_HOLDEM_NN_NEURALNETWORK_H


#include <random>

class NeuralNetwork {
public:
    NeuralNetwork(int[], int);
    ~NeuralNetwork();
    int getAction(double[]);

private:
    double** activ;
    double*** weights;
    int layers;
    int* neuronsPerLayer;

    void forward(double[]);
    double sigmoid(double);
};


#endif //TEXAS_HOLDEM_NN_NEURALNETWORK_H
