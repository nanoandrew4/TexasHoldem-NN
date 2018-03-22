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
    int currGen = 0;
    int numOfParents = 10;
    static const int itersPerGen = 10;
    static const int threads = 4;

    static bool threadReady[threads];

    void train(AIPlayer*[]);
    void trainThread(AIPlayer* players[], int playersPerTable, int threadNum, int startPlayer, int endPlayer);
    void blockUntilReady();
    void quicksort(AIPlayer*[], int, int);
    void generateNextGen(AIPlayer* players[], AIPlayer* parents[]);
};

#endif //TEXAS_HOLDEM_NN_NNEVOLVER_H
