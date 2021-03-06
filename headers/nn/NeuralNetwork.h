#ifndef TEXAS_HOLDEM_NN_NEURALNETWORK_H
#define TEXAS_HOLDEM_NN_NEURALNETWORK_H

#include <random>
#include <array>

class NeuralNetwork {
public:
	/**
	 * Number of layers in the neural network, should be updated alongside 'neuronsPerLayer'.
	 */
	static const std::uint8_t numOfLayers = 4;

	/**
	 * Initializes a network with 'layers' layers and 'n' neurons per layer, where 'n' is each element of
	 * 'neuronsPerLayer'.
	 *
	 * @param layers Length of array 'neuronsPerLayer', also the number of layers that compose the network
	 * @param neuronsPerLayer Array of integers representing the desired neurons at each layer
	 * @param randomize Whether to randomize the initial weights or leave them at initialized values (0)
	 */
	explicit NeuralNetwork(bool randomize);

	NeuralNetwork(std::vector<std::uint8_t> neuronsPerLayer, bool randomize);

	~NeuralNetwork();

	/**
	 * Returns the neuron that activated (which is the action the network would like to be performed)
	 *
	 * @param input First layer inputs, should be constant in size
	 * @return Index of activated neuron
	 */
	int getAction(const std::vector<double> &input);

	/**
	 * Sets a weight in the network to the desired value.
	 *
	 * @param l Layer in which the desired weight resides
	 * @param n Neuron in layer 'l' which is connected by the weight to be set
	 * @param nn Neuron in layer 'l + 1' which is connected by the weight to be set
	 * @param val Value to set weight to
	 */
	void setWeightAt(size_t l, size_t n, size_t nn, double val) { weights.at(l).at(n).at(nn) = val; }

	/**
	 * Returns the value of any given weight in the network.
	 *
	 * @param l Layer in which the desired weight resides
	 * @param n Neuron in layer 'l' which is connected by the weight to be retrieved
	 * @param nn Neuron in layer 'l + 1' which is connected by the the weight to be retrieved
	 * @return Value of the weight at [l][n][nn]
	 */
	double getWeightaAt(size_t l, size_t n, size_t nn) { return weights.at(l).at(n).at(nn); }

	/**
	 * Returns a pointer to an empty network with the same number of layers, and neurons per layer, as the network it
	 * was called on.
	 *
	 * @param randomize True to randomize weights, false to leave all weights at 0
	 * @return Pointer to cloned network
	 */
	NeuralNetwork *cloneNetworkStructure(bool randomize);

	/**
	 * Copy all weights from the network this function was called on to the network passed as an argument.
	 *
	 * @param nn Network to copy weights into
	 */
	void cloneNetworkInto(NeuralNetwork *nn);

	/**
	 * Writes the network structure and the value of all the weights to a file.
	 *
	 * @param fileName Name to give file containing the data for the network
	 */
	void serialize(const std::string &fileName);

	/**
	 * Writes the network structure and the value of all the weights to a stream, and writes a newline after the network
	 * data is successfully written.
	 *
	 * @param outputStream Stream to which to write the network data
	 */
	void serialize(std::ofstream &outputStream);

	/**
	 * Returns a pointer to a newly created neural network, with values loaded from a specified file. This file
	 * must have been written with the serialize(std::string) method contained in this class.
	 *
	 * @param fileName Name of file to read network information from
	 * @return Pointer to loaded network
	 */
	static NeuralNetwork *deserialize(const std::string &fileName);

	constexpr static unsigned long getNumOfLayers() { return numOfLayers; }

	static std::array<std::uint8_t, NeuralNetwork::numOfLayers> getNeuronsPerLayer() { return neuronsPerLayer; };

	static void setNeuronsPerLayer(std::array<std::uint8_t, NeuralNetwork::numOfLayers> newNeuronsPerLayer) {
		NeuralNetwork::neuronsPerLayer = newNeuronsPerLayer;
	}

	static unsigned int numOfInputs;

private:
	/**
	 * For our purposes, the number of neurons in a layer will not exceed 128 because more connections doesn't improve
	 * the networks performance, and for poker networks far less than 128 neurons per layer are necessary.
	 */
	static std::array<std::uint8_t, numOfLayers> neuronsPerLayer;

	// Activation values and weights that compose the network
	std::vector<std::vector<double>> activ;
	std::vector<std::vector<std::vector<double>>> weights;

	/**
	 * Forward propagates through the network, using a given set of inputs.
	 *
	 * @param input Inputs to use for propagation
	 */
	void forward(const std::vector<double> &input);

	// Sigmoid function (standard in NN implementations)
	double sigmoid(double d);

	// Inverse sigmoid (reverses the sigmoid operation, used for raises)
	double invSigmoid(double d);

	static std::mt19937_64 mt_rand;

	static std::uniform_real_distribution<> uniformRealDistribution;
};


#endif //TEXAS_HOLDEM_NN_NEURALNETWORK_H
