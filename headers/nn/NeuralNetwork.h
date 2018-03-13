#ifndef TEXAS_HOLDEM_NN_NEURALNETWORK_H
#define TEXAS_HOLDEM_NN_NEURALNETWORK_H


#include <random>

class NeuralNetwork {
public:

    /**
     * Initializes a network with 'layers' layers and 'n' neurons per layer, where 'n' is each element of
     * 'neuronsPerLayer'.
     *
     * @param neuronsPerLayer Array of integers representing the desired neurons at each layer
     * @param layers Length of array 'neuronsPerLayer', also the number of layers that compose the network
     */
    NeuralNetwork(int neuronsPerLayer[], int layers);
    ~NeuralNetwork();

    /**
     * Returns the neuron that activated (which is the action the network would like to be performed)
     *
     * @param input First layer inputs, should be constant in size
     * @return Index of activated neuron
     */
    int getAction(double[]);

private:
    // Activation values and weights that compose the network
    double** activ;
    double*** weights;

    // Information about the network
    int layers;
    int* neuronsPerLayer;

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
