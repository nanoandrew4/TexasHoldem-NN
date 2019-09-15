#include <chrono>
#include <iostream>
#include <fstream>
#include "../../headers/nn/NeuralNetwork.h"

std::mt19937_64 NeuralNetwork::mt_rand(std::random_device().operator()());

std::array<std::uint8_t, NeuralNetwork::getNumOfLayers()> NeuralNetwork::neuronsPerLayer = {5, 20, 10, 3};

NeuralNetwork::NeuralNetwork(bool randomize) {
	activ.resize(neuronsPerLayer.size());
	weights.resize(neuronsPerLayer.size() - 1);

	// Initialize neurons
	for (size_t l = 0; l < neuronsPerLayer.size(); l++)
		activ.at(l).resize(neuronsPerLayer.at(l));

	// Initialize weights connecting the various neurons randomly (between ~0-1)
	for (size_t l = 0; l < neuronsPerLayer.size() - 1; l++) {
		weights.at(l).resize(neuronsPerLayer.at(l));
		for (size_t n = 0; n < neuronsPerLayer.at(l); n++) {
			weights.at(l).at(n).resize(neuronsPerLayer.at(l + 1));
			if (randomize) {
				for (size_t nn = 0; nn < neuronsPerLayer.at(l + 1); nn++)
					weights.at(l).at(n).at(nn) = (mt_rand() % 100) / (double) (mt_rand() % 999 + 1);
			}
		}
	}
}

NeuralNetwork::NeuralNetwork(std::vector<std::uint8_t> neuronsPerLayer, bool randomize) {
	for (size_t i = 0; i < neuronsPerLayer.size(); i++)
		this->neuronsPerLayer.at(i) = neuronsPerLayer.at(i);

	activ.resize(neuronsPerLayer.size());
	weights.resize(neuronsPerLayer.size() - 1);

	// Initialize neurons
	for (size_t l = 0; l < neuronsPerLayer.size(); l++)
		activ.at(l).resize(neuronsPerLayer.at(l));

	// Initialize weights connecting the various neurons randomly (between ~0-1)
	for (size_t l = 0; l < neuronsPerLayer.size() - 1; l++) {
		weights.at(l).resize(neuronsPerLayer.at(l));
		for (size_t n = 0; n < neuronsPerLayer.at(l); n++) {
			weights.at(l).at(n).resize(neuronsPerLayer.at(l + 1));
			if (randomize) {
				for (size_t nn = 0; nn < neuronsPerLayer.at(l + 1); nn++)
					weights.at(l).at(n).at(nn) = (mt_rand() % 100) / (double) (mt_rand() % 999 + 1);
			}
		}
	}
//		this->neuronsPerLayer = std::move(neuronsPerLayer);
}

NeuralNetwork::~NeuralNetwork() = default;


void NeuralNetwork::cloneNetworkInto(NeuralNetwork *nNet) {
	for (size_t l = 0; l < neuronsPerLayer.size() - 1; l++)
		for (size_t n = 0; n < neuronsPerLayer.at(l); n++)
			for (size_t nn = 0; nn < neuronsPerLayer.at(l + 1); nn++)
				nNet->setWeightAt(l, n, nn, weights.at(l).at(n).at(nn));
}

NeuralNetwork *NeuralNetwork::cloneNetworkStructure(bool randomize) {
	return new NeuralNetwork(randomize);
}

void NeuralNetwork::serialize(const std::string &fileName) {
	std::ofstream out(fileName);
	serialize(out);
	out.close();
}

void NeuralNetwork::serialize(std::ofstream &outputStream) {
	outputStream << neuronsPerLayer.size() << " ";
	for (unsigned long npl : neuronsPerLayer)
		outputStream << npl << " ";

	for (size_t l = 0; l < neuronsPerLayer.size() - 1; l++)
		for (size_t n = 0; n < neuronsPerLayer.at(l); n++)
			for (size_t nn = 0; nn < neuronsPerLayer.at(l + 1); nn++)
				outputStream << weights.at(l).at(n).at(nn) << " ";
	outputStream << "\n";
}


NeuralNetwork *NeuralNetwork::deserialize(const std::string &fileName) {
	std::ifstream in(fileName);

	std::string file;
	while (!in.is_open()) {
		std::cout << "Error deserializing agent file, please try entering the path to the file again... ";
		std::cin >> file;

		in = std::ifstream(file);
	}

	int layers;
	in >> layers;

	std::vector<std::uint8_t> neuronsPerLayer;
	for (size_t l = 0; l < layers; l++)
		in >> neuronsPerLayer.at(l);

	NeuralNetwork *nNet = new NeuralNetwork(neuronsPerLayer, false);

	for (size_t l = 0; l < layers - 1; l++)
		for (size_t n = 0; n < neuronsPerLayer.at(l); n++)
			for (size_t nn = 0; nn < neuronsPerLayer.at(l + 1); nn++)
				in >> nNet->weights.at(l).at(n).at(nn);
	in.close();

	return nNet;
}

double NeuralNetwork::sigmoid(const double d) {
	return (1.0 / (1.0 + exp(-d)));
}

double NeuralNetwork::invSigmoid(double d) {
	return -log((1 / d) - 1);
}

void NeuralNetwork::forward(const std::vector<double> &input) {
	// Zero all activations in preparation for data ingestion, except input layer since it will be overwritten
	for (size_t l = 1; l < numOfLayers; ++l)
		for (size_t n = 0; n < neuronsPerLayer.at(l); ++n)
			activ.at(l).at(n) = 0;

	// Copy input data to input layer
	for (size_t n = 0; n < input.size(); ++n)
		activ.at(0).at(n) = sigmoid(input.at(n));

	// Forward propagate to all layers
	for (size_t l = 1; l < numOfLayers; ++l) {
		for (size_t n = 0; n < neuronsPerLayer.at(l); ++n) {
			for (size_t pn = 0; pn < neuronsPerLayer.at(l - 1); ++pn)
				activ.at(l).at(n) += activ.at(l - 1).at(pn) * weights.at(l - 1).at(pn).at(n);
			activ.at(l).at(n) = sigmoid(activ.at(l).at(n));
		}
	}
}

int NeuralNetwork::getAction(const std::vector<double> &input) {
	forward(input);

	size_t maxIndex = 0;
	for (size_t n = 1; n < neuronsPerLayer.back(); ++n)
		if (activ.back().at(n) > activ.back().at(maxIndex))
			maxIndex = n;

	if (maxIndex == 2)
		return (int) (invSigmoid(activ.back().at(maxIndex)));

	return (int) (maxIndex - 1);
}