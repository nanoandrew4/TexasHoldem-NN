#include <iostream>
#include "../../headers/test/HandTest.h"
#include "../../headers/holdem/Hand.h"

void HandTest::test() {
    testStraightFlush();
    testFourOfAKind();
    testFullHouse();
    testFlush();
    testStraight();
    testThreeOfAKind();
    testTwoPair();
    testPair();
    testHighCard();
}

void HandTest::testStraightFlush() {
    for (int s = 0; s < 4; s++)
        for (int c = 5; c < 14; c++) {
            Card c1((Suit) s, (c + 9) % 15), c2((Suit) s, c - 3);
            Hand hand(&c1, &c2);
            std::vector<Card*> commCards(3);
            for (int h = 0; h < 3; h++)
                commCards.at(h) = new Card((Suit) s, c - h);

            double score = hand.getHandScore(commCards);
            if (score <= STR_FLUSH || score > STR_FLUSH + 1) {
                testFail("Straight flush", hand.pocket, commCards);
                return;
            }
        }
}

void HandTest::testFourOfAKind() {
    for (int s = 0; s < 4; s++)
        for (int c = 2; c < 14; c++) {
            Card c1((Suit) s, c), c2((Suit) s, c);
            Hand hand(&c1, &c2);
            std::vector<Card*> commCards(2);
            for (int h = 0; h < 2; h++)
                commCards.at(h) = new Card((Suit) s, c);

            double score = hand.getHandScore(commCards);
            if (score <= FOUR_OAK || score >= STR_FLUSH) {
                testFail("Four of a kind", hand.pocket, commCards);
                return;
            }
        }
}

void HandTest::testFullHouse() {
    for (int s = 0; s < 4; s++)
        for (int cc = 2; cc < 14; cc++)
            for (int c = 3; c < 14; c++) {
                if (c == cc)
                    continue;

                Card c1((Suit) s, c), c2((Suit) s, c);
                Hand hand(&c1, &c2);
                std::vector<Card*> commCards(3);
                for (int h = 0; h < 3; h++)
                    commCards.at(h) = new Card((Suit) ((s + 1) % 4), cc);

                double score = hand.getHandScore(commCards);
                if (score <= FH|| score >= FOUR_OAK) {
                    testFail("Full House", hand.pocket, commCards);
                    return;
                }
            }
}

void HandTest::testFlush() {

}

void HandTest::testStraight() {

}

void HandTest::testThreeOfAKind() {

}

void HandTest::testTwoPair() {

}

void HandTest::testPair() {

}

void HandTest::testHighCard() {

}

void HandTest::testFail(std::string testName, std::vector<Card*> pocket, std::vector<Card*> commCards) {
    std::cout << "Hand -> " << testName << " test failed... Cards: ";
    commCards.insert(commCards.end(), pocket.begin(), pocket.end());
    Hand::displayHand(commCards);
}
