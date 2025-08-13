#ifndef CARDFACTORY_H
#define CARDFACTORY_H

#include <string>
#include <memory>

class Card;
class Player;

// A factory class to create card objects from their names
class CardFactory {
public:
    static std::shared_ptr<Card> createCard(const std::string& cardName, Player* owner);
};

#endif
