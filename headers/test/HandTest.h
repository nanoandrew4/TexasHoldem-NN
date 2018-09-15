#ifndef TEXAS_HOLDEM_NN_HANDTEST_H
#define TEXAS_HOLDEM_NN_HANDTEST_H

#include <vector>
#include <array>
#include "../holdem/Card.h"

class HandTest {
public:
    static void test();
private:
    static void testStraightFlush();
    static void testFourOfAKind();
    static void testFullHouse();
    static void testFlush();
    static void testStraight();
    static void testThreeOfAKind();
    static void testTwoPair();
    static void testPair();
    static void testHighCard();

    static void testFail(const std::string &testName, std::array<Card*, 2> pocket, std::vector<Card*> commCards);
    static void cleanCardVec(std::vector<Card*>& cards, int lim);
};

#endif //TEXAS_HOLDEM_NN_HANDTEST_H
