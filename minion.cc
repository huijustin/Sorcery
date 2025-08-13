#include "minion.h"
#include "player.h"
#include "game.h"
#include "ability.h"
#include "enchantment.h"
#include <iostream>

Minion::Minion(const std::string& name, int cost, Player* owner, int attack, int defense, 
               std::shared_ptr<Ability> ability, TriggerType triggerType, const std::string& triggerDesc)
    : Card(name, cost, owner, CardType::Minion), 
      attackVal(attack), defenseVal(defense), actions(0), ability(ability), 
      triggerType(triggerType), triggerDesc(triggerDesc), component(nullptr) {}

// --- Getters (can be decorated) ---
int Minion::getAttack() const { return attackVal; }
int Minion::getDefense() const { return defenseVal; }
int Minion::getActions() const { return actions; }
std::shared_ptr<Ability> Minion::getAbility() const { return ability; }
int Minion::getAbilityCost() const { return ability ? ability->getCost() : 0; }

// --- Setters ---
void Minion::setDefense(int new_defense) { defenseVal = new_defense; }
void Minion::takeDamage(int amount) { defenseVal -= amount; }
void Minion::gainActions(int amount) { actions = std::max(actions, amount); }

void Minion::spendAction() {
    if (actions <= 0) throw std::runtime_error("No actions left.");
    actions--;
}

// --- Core game actions ---
void Minion::attack(Player* target) {
    spendAction();
    target->setLife(target->getLife() - getAttack());
}

void Minion::attack(Minion* target) {
    spendAction();
    target->takeDamage(getAttack());
    this->takeDamage(target->getAttack());
}

// --- Ability and Trigger methods ---
void Minion::useAbility(Player* p) {
    if (!getAbility()) throw std::runtime_error("Minion has no ability.");
    spendAction();
    int oldMagic = p->getMagic();
    try {
        p->spendMagic(getAbilityCost());
        getAbility()->apply(p, nullptr, -1);
    } catch (...) {
        if (p->getMagic() < oldMagic) {
            p->gainMagic(oldMagic - p->getMagic());
        }
        throw;
    }
}

void Minion::useAbility(Player* p, Player* t, int i) {
    if (!getAbility()) throw std::runtime_error("Minion has no ability.");
    spendAction();
    int oldMagic = p->getMagic();
    try {
        p->spendMagic(getAbilityCost());
        getAbility()->apply(p, t, i);
    } catch (...) {
        if (p->getMagic() < oldMagic) {
            p->gainMagic(oldMagic - p->getMagic());
        }
        throw;
    }
}

void Minion::useTrigger(TriggerType type, std::shared_ptr<Minion> target) {
    if (this->triggerType == type && getAbility()) {
        std::cout << getOwner()->getName() << "'s " << name << " trigger activated." << std::endl;
        // Note: Triggers don't cost actions or magic
        // The target of the trigger is the minion that caused the event
        Player* target_owner = target ? target->getOwner() : nullptr; // <-- FIXED
        
        // Find target minion's index on its owner's board
        int target_idx = -1;
        if(target_owner) {
            const auto& minions = target_owner->getMinions();
            for(size_t i = 0; i < minions.size(); ++i) {
                if (minions[i] == target) {
                    target_idx = i;
                    break;
                }
            }
        }
        
        getAbility()->apply(getOwner(), target_owner, target_idx);
    }
}


// --- Decorator pattern methods ---
void Minion::addEnchantment(std::shared_ptr<Enchantment> ench) {
    if (component) {
        // Pass down the chain
        std::dynamic_pointer_cast<Enchantment>(component)->addEnchantment(ench);
    } else {
        // End of the chain, add here
        component = ench;
    }
}

std::shared_ptr<Minion> Minion::stripTopEnchantment() {
    if (!component) return nullptr;

    auto top_enchantment = std::dynamic_pointer_cast<Enchantment>(component);
    if (!top_enchantment) return nullptr;

    auto next_component = top_enchantment->getComponent();
    component = next_component; // This minion now points to the one below the stripped one
    top_enchantment->setComponent(nullptr); // Isolate the stripped enchantment
    return top_enchantment;
}

void Minion::stripEnchantments() {
    component = nullptr;
}

bool Minion::isDead() const { return getDefense() <= 0; }

// --- Rendering ---
card_template_t Minion::render() const {
    if (component) return component->render();
    return renderBase();
}

card_template_t Minion::renderBase() const {
    if (ability && ability->getCost() > 0) {
        return display_minion_activated_ability(name, cost, attackVal, defenseVal, ability->getCost(), ability->getDescription());
    } else if (triggerType != TriggerType::None) {
        return display_minion_triggered_ability(name, cost, attackVal, defenseVal, triggerDesc);
    } else {
        return display_minion_no_ability(name, cost, attackVal, defenseVal);
    }
}

std::vector<std::shared_ptr<Enchantment>> Minion::getEnchantments() const {
    if (component) {
        auto sub_list = component->getEnchantments();
        sub_list.insert(sub_list.begin(), std::dynamic_pointer_cast<Enchantment>(component));
        return sub_list;
    }
    return {};
}
