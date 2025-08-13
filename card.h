#ifndef CARD_H
#define CARD_H

#include <string>
#include <memory>
#include "ascii_graphics.h"

class Player;

// Enum for the type of card
enum class CardType {
    Minion,
    Spell,
    Enchantment,
    Ritual
};

// Enum for trigger types
enum class TriggerType {
    MinionEnters,
    MinionLeaves,
    StartOfTurn,
    EndOfTurn,
    None
};

// Abstract base class for all cards
class Card {
protected:
    std::string name;
    int cost;
    Player* owner;
    CardType type;

public:
    Card(const std::string& name, int cost, Player* owner, CardType type);
    virtual ~Card() = default;

    // Pure virtual methods that all cards must implement
    virtual card_template_t render() const = 0;
    
    // Virtual methods for playing cards, with and without targets
    virtual void play(Player* p);
    virtual void play(Player* p, Player* t, int i);

    // Getters
    const std::string& getName() const;
    int getCost() const;
    CardType getType() const;
    Player* getOwner() const; // <-- ADDED GETTER
};

#endif
