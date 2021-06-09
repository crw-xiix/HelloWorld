//This is 6-9-2021 Morning screen shot

#ifdef __NOPE__

#pragma once
#include <utility>
#include <vector>
#include <algorithm>
#include <string>
#include <iostream>
#include <iterator>
//#include <numeric>
#include <tuple>
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
    inline int getBitCount(int n) {
        unsigned int count = 0;
        while (n) {
            count += n & 1;
            n >>= 1;
        }
        return count;
    }

public:
    /// <summary>
    /// Return the count as a pair< deadwood, score >
    /// </summary>
    /// <param name="icards"></param>
    /// <returns></returns>
    inline std::tuple<int, int, std::vector<Card> > count(std::vector<Card>& icards) {
        std::vector<Card> cards;
        std::vector<Card> good;
        std::vector<Card> cards4Pairs;
        std::vector<Card> cards4Runs;
        std::vector<Card> bestGood;

        //Need to copy the passed cards
        for (auto& card : icards) cards.push_back(card);

        //Set our limits
        int bestDead = 1000;
        int bestScore = -1000;

        int max = 1 << (cards.size());
        int len = cards.size();

        //This will loop 1024, more on 11 cards
        for (int i = 0; i < max; i++) {
            int bits = getBitCount(i);
            //if (bits < 4) continue;
            //if (max - bits < 4) continue;

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
                while (moveOutSets(cards4Pairs, good)) {
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
                    //Hopefully this is a copy.
                    bestGood = good;
                    for (auto& c : dead) {
                        //Just to make sure
                        c.set = false;
                        c.run = false;
                        bestGood.push_back(c);
                    }
                }
            }
        }
        return { bestDead, bestScore, bestGood };
    }

    inline int calcDead(std::vector<Card> cards) {
        int deadwood = 0;
        for (Card cc : cards) deadwood += clip(cc.getCardValue());
        return deadwood;
    }

    inline bool moveOutRuns(std::vector<Card>& cards, std::vector<Card>& dest) {
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
                        v.run = true;
                        v.set = false;
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
                v.run = true;
                v.set = false;
                dest.push_back(v);
            }
            return true;
        }
        return false;
    }

    inline void moveCardOut(std::vector<Card>& cards, std::vector<Card>& dest, int sidx) {
        cards.erase(std::next(cards.begin(), sidx));
    }

    inline bool moveOutSets(std::vector<Card>& cards, std::vector<Card>& dest) {

        int sets = 0;

        int len = cards.size();
        if (len < 3) return false;

        //We are going to look back step this
        //Look left, look right - if same, inc sets
        bool setDone = false;
        //Mark them
        for (int i = 0; i < (cards.size() - 1); i++) {
            len = cards.size() - i;
            //Test for 4
            //xxxc  Bailed, gotta figure this part out.......  It's going beyond limits.
            if (len >= 4) {
                if (cards[i].getCardNo() == cards[i + 1].getCardNo())
                    if (cards[i + 1].getCardNo() == cards[i + 2].getCardNo())
                        if (cards[i + 2].getCardNo() == cards[i + 3].getCardNo()) {
                            //remove the first 4 now
                            for (int c = 0; c < 4; c++) {
                                //remove the first 3 now
                                Card& card = cards[i];
                                card.set = true;
                                card.run = false;
                                dest.push_back(card);
                                moveCardOut(cards, dest, i);
                                int p = 0;

                            }
                            continue;
                        }
            }
            if (len >= 3) {
                if (cards[i].getCardNo() == cards[i + 1].getCardNo()) {
                    if (cards[i + 1].getCardNo() == cards[i + 2].getCardNo()) {
                        for (int c = 0; c < 3; c++) {
                            //remove the first 3 now
                            Card& card = cards[i];
                            card.set = true;
                            card.run = false;
                            dest.push_back(card);
                            moveCardOut(cards, dest, i);
                            int p = 0;
                        }
                    }
                }
            }


            /*int found = getSetNumFromIdx(cards, i);
            if (found > 1) {  //We got 3+ from this
                int asdf = 1;
                for (int j = 0; j <= found; j++) {
                    cards[i].set = true;
                    dest.push_back(cards[i]);
                    cards.erase(std::next(cards.begin(), i ));
                }
                //cards.erase(std::next(cards.begin(), i), std::next(cards.begin(), i + found));
                asdf = 2;
            }*/
            /*
                        Card& l = cards[i];
                        Card& r = cards[i+1];
                        l.set = false;
                        r.run = false;
            */
            /*
                        if (l.getCardNo() == r.getCardNo()) {
                            //They are the same, so we have 2 cards now...
                            l.set = true;
                            r.set = true;
                            sets++;
                        }
                        else {
                            return true;
                        }
                    }
                    //Move them
                    for (int i = 0; i < len; i++) {
                        if (cards[i].set) {
                            dest.push_back(cards[i]);
                        }
                    }

                    cards.erase(cards.begin(), cards.end());*/
        }
        return false;
    }

    inline int rand52() {
        return rand() % 52;
    }

    inline void showHand(std::vector<Card>& cards, bool showSets = false) {
        int sum = 0;
        for (auto& card : cards) {
            std::cout << card.getName() << " ";
            sum += clip(card.getCardValue());
        }
        std::cout << " T: " << sum;
        std::cout << "\n";
        if (showSets) {
            for (auto& card : cards) {
                if (card.set) {
                    std::cout << "SS ";
                }
                else
                    if (card.run) {
                        std::cout << "RR ";
                    }
                    else
                    {
                        std::cout << "-- ";
                    }
            }
            std::cout << "\n";
        }

    }

    inline static void test() {
        GinCounter counter;

        std::vector<Card> cards;
        std::vector<Card> deck;
        std::vector<Card> dest;
        //set
        cards.push_back(Card(0, 0, "4C"));
        cards.push_back(Card(0, 0, "4D"));
        cards.push_back(Card(0, 0, "4S"));
        //run
        cards.push_back(Card(0, 0, "8H"));
        cards.push_back(Card(0, 0, "9H"));
        cards.push_back(Card(0, 0, "TH"));
        //set
        cards.push_back(Card(0, 0, "6H"));
        cards.push_back(Card(0, 0, "6C"));
        cards.push_back(Card(0, 0, "6S"));
        cards.push_back(Card(0, 0, "6D"));


        counter.moveOutSets(cards, dest);
        while (counter.moveOutRuns(cards, dest)) {};
        sort(dest.begin(), dest.end(), &sortByRankOrder);
        counter.showHand(dest, true);
        std::cout << "\n[" << " " << "]" << "-------------------------------------------\n";
        /*
        auto v = counter.count(cards);
//        std::cout << "\n[" << (i + 1) << "]" << "-------------------------------------------\n";
        counter.showHand(cards);
        //std::cout << "\nCounting Hand\n";

        auto bestGood = std::get<2>(v);
        sort(bestGood.begin(), bestGood.end(), &sortByRankOrder);
        counter.showHand(bestGood, true);
        std::cout << "Dead:" << std::get<0>(v) << " Score:" << std::get<1>(v) <<
            " Summed:" << (std::get<0>(v) + std::get<1>(v)) << "\n";
            */

        srand(1);
        //std::cin.ignore();
        //return ;
        std::cout << "\n[" << "]" << "-------------------------------------------\n";
        for (int i = 0; i < 120; i++) {
            cards.clear();
            deck.clear();
            dest.clear();

            //Full random card test.  no dups
            for (int c = 0; c < 52; c++) {
                deck.push_back(Card(0, 0, c));
            }
            for (int c = 0; c < 10; c++) {
                int pos = rand() % deck.size();
                cards.push_back(deck[pos]);
                deck.erase(std::next(deck.begin(), pos));
            }

            //std::cout << "\nNew hand\n";
            sort(cards.begin(), cards.end(), &sortByRankOrder);
            counter.moveOutSets(cards, dest);
            while (counter.moveOutRuns(cards, dest)) {};
            sort(dest.begin(), dest.end(), &sortByRankOrder);
            counter.showHand(dest, true);
            counter.moveOutSets(cards, dest);
            std::cout << "\n[" << (i + 1) << "]" << "-------------------------------------------\n";
            /*
            auto v = counter.count(cards);
            std::cout << "\n[" << (i + 1) << "]" << "-------------------------------------------\n";

            counter.showHand(cards);
            //std::cout << "\nCounting Hand\n";

            auto bestGood = std::get<2>(v);
            sort(bestGood.begin(), bestGood.end(), &sortByRankOrder);
            counter.showHand(bestGood, true);
            std::cout << "Dead:" << std::get<0>(v) << " Score:" << std::get<1>(v) <<
                " Summed:" << (std::get<0>(v) + std::get<1>(v)) << "\n";

            */
        }
        std::cin.get();
    }
};

#endif
