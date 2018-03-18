#ifndef TEXAS_HOLDEM_NN_NNEVOLVER_H
#define TEXAS_HOLDEM_NN_NNEVOLVER_H

#include "../holdem/Player.h"
#include "AIPlayer.h"

class NNEvolver {
public:
    NNEvolver();
    ~NNEvolver();
private:
    int population = 1000;
    int gensToEvolve = 50;
    int numOfParents = 10;
    int threads = 4;

    void train(AIPlayer*[], int, int);
    void quicksort(AIPlayer*[], int, int);
    void generateNextGen(AIPlayer* players[], AIPlayer* parents[]);
};


#endif //TEXAS_HOLDEM_NN_NNEVOLVER_H
