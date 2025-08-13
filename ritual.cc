#include "ritual.h"
#include "player.h"
#include "ability.h"
#include "game.h"
#include "minion.h" // Include full minion definition
#include <iostream>

Ritual::Ritual(const std::string& name, int cost, Player* owner, int charges, int activation_cost, 
               TriggerType triggerType, const std::string& triggerDesc, std::shared_ptr<Ability> ability)
    : Card(name, cost, owner, CardType::Ritual), charges(charges), activation_cost(activation_cost),
      ability(ability), triggerType(triggerType), triggerDesc(triggerDesc) {}

// Playing a ritual places it on the player's board
void Ritual::play(Player* p) {
    // Find the shared_ptr to this card in the hand to pass to setRitual
    auto& hand = p->getHand();
    for(auto it = hand.begin(); it != hand.end(); ++it) {
        if (it->get() == this) {
            p->setRitual(std::dynamic_pointer_cast<Ritual>(*it));
            return;
        }
    }
}

// Render the ritual card
card_template_t Ritual::render() const {
    return display_ritual(name, cost, activation_cost, triggerDesc, charges);
}

// Check for and use the ritual's triggered ability
void Ritual::useTrigger(TriggerType type, std::shared_ptr<Minion> target) {
    if (this->triggerType == type && charges >= activation_cost) {
        std::cout << getOwner()->getName() << "'s " << name << " trigger activated." << std::endl;
        charges -= activation_cost;
        Player* target_owner = target ? target->getOwner() : nullptr; // <-- FIXED
        int target_idx = -1; // Ritual triggers often don't have specific targets
        ability->apply(getOwner(), target_owner, target_idx);
    }
} 

void Ritual::gainCharges(int amount) {
    charges += amount;
}
