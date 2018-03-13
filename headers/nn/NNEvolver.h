#ifndef TEXAS_HOLDEM_NN_NNEVOLVER_H
#define TEXAS_HOLDEM_NN_NNEVOLVER_H

#include "../holdem/Player.h"

class NNEvolver {
public:
    NNEvolver();
    ~NNEvolver();
private:
    int population = 1000;
    int gensToEvolve = 1000;
    int threads = 4;

    void train(Player***, int, int);
};


#endif //TEXAS_HOLDEM_NN_NNEVOLVER_H
