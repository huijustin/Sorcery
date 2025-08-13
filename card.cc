#include "card.h"
#include "player.h"
#include <stdexcept>

// Card constructor
Card::Card(const std::string& name, int cost, Player* owner, CardType type)
    : name(name), cost(cost), owner(owner), type(type) {}

// Getters
const std::string& Card::getName() const { return name; }
int Card::getCost() const { return cost; }
CardType Card::getType() const { return type; }
Player* Card::getOwner() const { return owner; } // <-- IMPLEMENTED GETTER

// Default play implementation (for cards that don't need a target)
void Card::play(Player* p) {
    throw std::runtime_error("This card cannot be played without a target.");
}

// Default play implementation (for cards that need a target)
void Card::play(Player* p, Player* t, int i) {
    throw std::runtime_error("This card cannot be played with a target.");
}
