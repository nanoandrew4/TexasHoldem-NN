#ifndef TEXAS_HOLDEM_NN_NEURALNETWORK_H
#define TEXAS_HOLDEM_NN_NEURALNETWORK_H


#include <random>

class NeuralNetwork {
public:
    // Information about the network
    int layers;
    int* neuronsPerLayer;

    /**
     * Initializes a network with 'layers' layers and 'n' neurons per layer, where 'n' is each element of
     * 'neuronsPerLayer'.
     *
     * @param layers Length of array 'neuronsPerLayer', also the number of layers that compose the network
     * @param neuronsPerLayer Array of integers representing the desired neurons at each layer
     * @param randomize Whether to randomize the initial weights or leave them at initialized values (0)
     */
    NeuralNetwork(int layers, int neuronsPerLayer[], bool randomize);
    ~NeuralNetwork();

    /**
     * Returns the neuron that activated (which is the action the network would like to be performed)
     *
     * @param input First layer inputs, should be constant in size
     * @return Index of activated neuron
     */
    int getAction(double[]);
    void setWeightAt(int l, int n, int nn, double val) {weights[l][n][nn] = val;}
    double getWeightaAt(int l, int n, int nn) { return weights[l][n][nn];}

    NeuralNetwork* cloneNetworkStructure(bool randomize);
    NeuralNetwork* cloneNetworkInto(NeuralNetwork* nn);

private:
    // Activation values and weights that compose the network
    double** activ;
    double*** weights;

    /**
     * Forward propagates through the network, using a given set of inputs.
     *
     * @param input Inputs to use for propagation
     */
    void forward(double input[]);

    // Sigmoid function (standard in NN implementations)
    double sigmoid(double d);

    // Inverse sigmoid (reverses the sigmoid operation, used for raises)
    double invSigmoid(double d);
};


#endif //TEXAS_HOLDEM_NN_NEURALNETWORK_H
