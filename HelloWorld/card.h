 /**
  *
  * @author Charles Wood
  * Demo for YT video
  * 
  */
#pragma once

#include <string>
#include <vector>

class Card {
public:
    static const std::vector<std::string> cardVals;// = { "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K" };
    static const std::vector<std::string> cardSuits;// = { "Spade", "Hearts", "Clubs", "Diamonds" };
private:
    int cardId;
    int cardNo;
    int cardValue;
    int suit;
    std::string cardName = "";
    bool up = true;
public:
    inline Card() {
        int c = 0;
        cardId = c;          
        suit = c / 13;       //Spade, Hearts, clubs, etc
        cardNo = (c % 13);   //A-K
        cardValue = cardNo + 1;  //score value
        cardName = getName();
    }

    inline int getCardNo() {
        return cardNo;
    }

    inline int getCardSuit() {
        return suit;
    }

    inline std::string getName() {
        return cardVals[cardNo] + cardSuits[suit][0];
    }
    // a = 1,2 = 2, j=11, etc
    inline int getCardValue() {
        return cardValue;
    }

    inline Card(int x, int y, int c) {
        cardId = c;
        suit = c / 13;
        cardNo = (c % 13);
        cardValue = cardNo + 1;
        cardName = getName();
    }

    //This is for debug testing
    inline Card(int x, int y, std::string card) {
        int c = 0;
        switch (toupper(card[0])) {
        case 'A': c = 0; break;
        case '2': c = 1; break;
        case '3': c = 2; break;
        case '4': c = 3; break;
        case '5': c = 4; break;
        case '6': c = 5; break;
        case '7': c = 6; break;
        case '8': c = 7; break;
        case '9': c = 8; break;
        case 'T': c = 9; break;
        case 'J': c = 10; break;
        case 'Q': c = 11; break;
        case 'K': c = 12; break;
        default: exit(0);
        }
        switch (toupper(card[1])) {
        case 'S': c += 13 * 0; break;
        case 'H': c += 13 * 1; break;
        case 'C': c += 13 * 2; break;
        case 'D': c += 13 * 3; break;
        default: exit(0);
        }
        cardId = c;
        suit = cardId / 13;
        cardNo = (cardId % 13);
        cardValue = cardNo + 1;
        cardName = getName();
    }

    /// <summary>Sorts by suit, then order, for runs basically.</summary>
    /// <returns>The position in a deck of newly sorted cards</returns>
    inline int getDeckOrder() {
        return (suit * 13 + cardNo);
    }
    /// <summary>Sorts by value, 11123334JA for 3+ of a kind.</summary>
    /// <returns>The position in Rank Order<returns>
    inline int getRankOrder() {
        return cardValue;
    }
};


