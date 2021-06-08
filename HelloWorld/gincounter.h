#pragma once
#include <utility>
#include <vector>
#include <algorithm>
#include <string>
#include <iostream>
#include <iterator>
#include <stdlib.h>
#include "card.h"

class GinCounter {
private:
    static inline int clip(int v) {
        if (v > 10) return 10;
        return v;
    }
    //static inline std::pair<int, int> count(std::vector<Card> icards, Card remove) {

        /// Comparators
    static inline bool sortByNewDeckOrder(Card& a, Card& b)
    {
        return (a.getDeckOrder() < b.getDeckOrder());
    }

    static inline bool sortByRankOrder(Card& a, Card& b)
    {
        return (a.getRankOrder() < b.getRankOrder());
    }

public:
    /// <summary>
    /// Return the count as a pair< deadwood, score >
    /// </summary>
    /// <param name="icards"></param>
    /// <returns></returns>
    static inline std::pair<int, int> count(std::vector<Card>& icards) {
        std::vector<Card> cards;
        std::vector<Card> good;
        std::vector<Card> cards4Pairs;
        std::vector<Card> cards4Runs;

        //Need to copy the passed cards
        for (auto& card : icards) cards.push_back(card);

        //Set our limits
        int bestDead = 1000;
        int bestScore = -1000;

        int max = 1 << (cards.size());
        int len = cards.size();

        //This will loop 1024, more on 11 cards
        for (int i = 0; i < max; i++) {
            cards4Pairs.clear();
            cards4Runs.clear();
            good.clear();
            //Split them into two sets of cards
            //We try all combinations, it's not that many, and it's not called often
            for (int c = 0; c < len; c++) {
                if ((i & (1 << c)) >= 1) {
                    cards4Pairs.push_back(cards[c]);
                }
                else {
                    cards4Runs.push_back(cards[c]);
                }
            }

            //Move out the runs, sort if 3+
            if (cards4Runs.size() >= 3) {
                sort(cards4Runs.begin(), cards4Runs.end(), &sortByNewDeckOrder);
                while (moveOutRuns(cards4Runs, good)) {
                }
            }
            //Move out the pairs, sort if 3+
            if (cards4Pairs.size() >= 3) {
                sort(cards4Pairs.begin(), cards4Pairs.end(), &sortByRankOrder);
                while (moveOutPairs(cards4Pairs, good)) {
                }
            }
            /*
            if (good.size() > 0) {
                std::cout << "Good cards:";
                showHand(good);
            }*/
            std::vector<Card> dead;
            //Calculate the deadwood
            int deadwood = 0;
            for (Card& c : cards4Runs) {
                deadwood += clip(c.getCardValue());
                dead.push_back(c);
            }
            for (Card& c : cards4Pairs) {
                deadwood += clip(c.getCardValue());
                dead.push_back(c);
            }
            /*
            if (dead.size() > 0) {
                std::cout << "Dead Wood:";
                showHand(dead);
            }*/

            //Now the score
            int score = 0;
            for (Card& c : good) {
                score += clip(c.getCardValue());
            }
            //Update the bestScore/bestDead
            if (deadwood <= bestDead) {
                if (score > bestScore) {
                    bestDead = deadwood;
                    bestScore = score;
                }
            }
        }
        return { bestDead, bestScore };
    }

    static int calcDead(std::vector<Card> cards) {
        int deadwood = 0;
        for (Card cc : cards) deadwood += clip(cc.getCardValue());
        return deadwood;
    }

    inline static bool moveOutRuns(std::vector<Card>& cards, std::vector<Card>& dest) {
        int lastSuit = -2;   //invalid value (nothing found yet)
        int lastValue = -2;  //invalid value
        int runs = 0;
        int bitc = 0;
        int selected = 0;
        int len = cards.size();
        if (len < 3) return false;
        for (int i = 0; i < len; i++) {
            Card& c = cards[i];
            if (((c.getCardNo() - 1) == (lastValue)) && (c.getCardSuit() == lastSuit)) {
                runs++;
                lastSuit = c.getCardSuit();
                lastValue = c.getCardNo();
            }
            else {
                if (runs >= 2) {
                    for (int ii = 0; ii <= runs; ii++) {
                        int idx = i - runs - 1;
                        Card v = cards[idx];
                        cards.erase(std::next(cards.begin(), idx));
                        dest.push_back(v);
                    }
                    return true;
                }
                runs = 0;
                lastSuit = c.getCardSuit();
                lastValue = c.getCardNo();
            }
        }

        if (runs >= 2) {
            for (int ii = 0; ii <= runs; ii++) {
                int idx = len - runs - 1;
                Card v = cards[idx];
                cards.erase(std::next(cards.begin(), idx));
                dest.push_back(v);
            }
            return true;
        }
        return false;
    }

    static inline bool moveOutPairs(std::vector<Card>& cards, std::vector<Card>& dest) {
        int lastSuit = -2;
        int lastValue = -2;
        int runs = 0;

        int len = cards.size();
        if (len < 3) return false;
        //std::cout << "Pairs moved out";
        for (int i = 0; i < len; i++) {
            Card& c = cards[i];
            /*
            println("" + c);
            println("last:" + lastSuit + "." + lastValue + " this " + c.suit + "." + c.cardValue);
            */
            //Scan for 3oK/4oK
            if (((c.getCardNo()) == (lastValue))) {
                runs++;
                lastValue = c.getCardNo();
            }
            else {
                if (runs >= 2) {
                    //                    print("--------detected run:");
                    for (int ii = 0; ii <= runs; ii++) {
                        int idx = i - runs - 1;
                        if (idx < 0) {
                            std::cout << "Error card: " << idx << "\n";
                        }
                        Card& v = cards[idx];
                        cards.erase(std::next(cards.begin(), idx));
                        dest.push_back(v);
                        //std::cout << v.getName() + " ";
                    }
                    return true;
                }
                runs = 0;
                lastValue = c.getCardNo();
            }
        }

        //It's all over now, 
        if (runs >= 2) {
            //            print("--------detected run:");
            for (int ii = 0; ii <= runs; ii++) {
                int idx = len - runs - 1;
                Card v = cards[idx];
                cards.erase(std::next(cards.begin(), idx));
                dest.push_back(v);
                //                std::cout << v.getName() + " ";
            }
            //   println("");
            return true;
        }
        return false;
    }

    static inline int rand52() {
        return rand() % 52;
    }
    static inline void showHand(std::vector<Card>& cards) {
        for (auto& card : cards) {
            std::cout << card.getName() << " ";
        }
        std::cout << "\n";
    }

    inline static void test() {
        std::vector<Card> cards;
        std::vector<Card> deck;
        srand(1);
        for (int i = 0; i < 100; i++) {
            cards.clear();
            deck.clear();
            //Full random card test.  no dups
            for (int c = 0; c < 52; c++) {
                deck.push_back(Card(0, 0, c));
            }
            for (int c = 0; c < 10; c++) {
                int pos = rand() % deck.size();
                cards.push_back(deck[pos]);
                deck.erase(std::next(deck.begin(), pos));
            }


            /*
            cards.push_back(Card(0, 0, "AH"));
            cards.push_back(Card(0, 0, "4H"));
            cards.push_back(Card(0, 0, "4C"));//
            cards.push_back(Card(0, 0, "5H"));//
            cards.push_back(Card(0, 0, "6C"));//
            cards.push_back(Card(0, 0, "QH"));
            cards.push_back(Card(0, 0, "JC"));
            cards.push_back(Card(0, 0, "8D"));//
            cards.push_back(Card(0, 0, "8S"));//
            cards.push_back(Card(0, 0, "8H"));//
            */
            std::cout << "Before sort\n";
            //That will give us 10 random cards.
            showHand(cards);
            sort(cards.begin(), cards.end(), &sortByNewDeckOrder);
            std::cout << "\nAfter sort New Deck Order\n";
            showHand(cards);

            std::cout << "\nAfter sort Rank Order\n";
            sort(cards.begin(), cards.end(), &sortByRankOrder);
            showHand(cards);

            std::cout << "\nCounting Hand\n";
            auto v = GinCounter::count(cards);
            std::cout << "Dead:" << v.first << " Score:" << v.second << "\n";
        }
    }
};

