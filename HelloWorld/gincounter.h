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
    inline std::tuple<int, int, std::vector<Card> > count(std::vector<Card>& icards, bool order = false) {
        std::vector<Card> cards;
        std::vector<Card> good;

        //Need to copy the passed cards
        for (auto& card : icards) cards.push_back(card);

        //Set our limits
        int bestDead = 1000;
        int bestScore = -1000;

        int len = cards.size();


        good.clear();
        
        //Move out the runs, sort if 3+
        
        if (!order) {
            sort(cards.begin(), cards.end(), &sortByNewDeckOrder);
            while (moveOutRuns(cards, good)) {}
            
            sort(cards.begin(), cards.end(), &sortByRankOrder);
            while (moveOutSets(cards, good)) {}
        }
        else {
            sort(cards.begin(), cards.end(), &sortByRankOrder);
            while (moveOutSets(cards, good)) {}

            sort(cards.begin(), cards.end(), &sortByNewDeckOrder);
            while (moveOutRuns(cards, good)) {}
        }

        //Calculate the deadwood
        int deadwood = 0;
        for (Card& c : cards) {
            deadwood += clip(c.getCardValue());
            //dead.push_back(c);
        }
        
        //Now the score
        int score = 0;
        for (Card& c : good) {
            score += clip(c.getCardValue());
        }
        //Now move the rest over....
        moveOutRest(cards, good);
        return { deadwood, score, good };
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

    inline void moveOutRest(std::vector<Card>& cards, std::vector<Card>& dest) {
        for (;cards.size()>0;) {
            dest.push_back(cards[0]);
            moveCardOut(cards, dest, 0);
        }
    }
    inline bool moveOutSets(std::vector<Card> &cards, std::vector<Card>& dest) {
        int sets = 0;
        int len = cards.size();
        if (len < 3) return false;
        bool setDone = false;
        //Mark them
        for (size_t i = 0; i < (cards.size() - 1); i++) {
            len = cards.size() - i;
            //Test for 4
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
        }
        return false;
    }

    inline int rand52() {
        return rand() % 52;
    }

    inline void showHand(std::vector<Card>& cards, bool showSets=false) {
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


        /*
        counter.moveOutSets(cards, dest);
        while (counter.moveOutRuns(cards, dest)) {};
        sort(dest.begin(), dest.end(), &sortByRankOrder);
        counter.showHand(dest,true);
        std::cout << "\n[" << " " << "]" << "-------------------------------------------\n";
        */
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
        int totalDead = 0;

        for (int i = 0; i < 1000; i++) {
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
            
            std::cout << "\n[" << (i + 1) << "]" << "-------------------------------------------\n";
            sort(cards.begin(), cards.end(), &sortByRankOrder);
            sort(cards.begin(), cards.end(), &sortByNewDeckOrder);
            counter.showHand(cards);

            auto v = counter.count(cards,true);
            auto vr = counter.count(cards, false);
            
            if (std::get<0>(vr) < std::get<0>(v)) {
                //Swap 'em
                v = vr;
            }

            auto bestGood = std::get<2>(v);
            sort(bestGood.begin(), bestGood.end(), &sortByNewDeckOrder);
            sort(bestGood.begin(), bestGood.end(), &sortByRankOrder);
            counter.showHand(bestGood, true);
            totalDead += std::get<0>(v);
            std::cout << "Dead:" << std::get<0>(v) << " Score:" << std::get<1>(v) << " Summed:" << (std::get<0>(v) + std::get<1>(v)) << "\n";

            
        }
        std::cout << "Total Dead: " << totalDead;
        std::cin.get();
    }
};

