#include "enchantment.h"
#include "player.h"
#include "game.h"
#include <stdexcept>

// --- Base Enchantment ---
Enchantment::Enchantment(const std::string& name, int cost, Player* owner, std::shared_ptr<Minion> component)
    : Minion(name, cost, owner, 0, 0), component(component) {
    this->type = CardType::Enchantment;
}

void Enchantment::play(Player* p, Player* t, int i) {
    if (i < 0 || i > 4) throw std::runtime_error("Invalid target for enchantment.");
    // Since we need to modify the vector, we get the non-const version
    auto& target_minions = t->getMinions(); 
    if (!target_minions[i]) throw std::runtime_error("Target minion does not exist.");
    
    // The target minion's shared_ptr will now point to this enchantment object.
    // This enchantment's component will point to the original target minion.
    this->component = target_minions[i];
    target_minions[i] = std::dynamic_pointer_cast<Enchantment>(shared_from_this());
}

// Overload of the base card 'play' to allow enchantments to be played without a target (which is an error)
void Enchantment::play(Player* p) {
    throw std::runtime_error("Enchantments require a target minion.");
}


// Pass-through getters
int Enchantment::getAttack() const { return component->getAttack(); }
int Enchantment::getDefense() const { return component->getDefense(); }
int Enchantment::getActions() const { return component->getActions(); }
std::shared_ptr<Ability> Enchantment::getAbility() const { return component->getAbility(); }
int Enchantment::getAbilityCost() const { return component->getAbilityCost(); }
card_template_t Enchantment::renderBase() const { return component->renderBase(); }
std::vector<std::shared_ptr<Enchantment>> Enchantment::getEnchantments() const {
    auto sub_list = component->getEnchantments();
    // We need to cast 'this' to get a shared_ptr to ourself to add to the list.
    // const_cast is needed because getEnchantments is a const method.
    sub_list.insert(sub_list.begin(), std::dynamic_pointer_cast<Enchantment>(std::const_pointer_cast<Minion>(shared_from_this())));
    return sub_list;
}

void Enchantment::setComponent(std::shared_ptr<Minion> comp) { component = comp; }
std::shared_ptr<Minion> Enchantment::getComponent() const { return component; }


// --- Giant Strength ---
GiantStrength::GiantStrength(Player* owner, std::shared_ptr<Minion> component)
    : Enchantment("Giant Strength", 1, owner, component) {}
int GiantStrength::getAttack() const { return component->getAttack() + 2; }
int GiantStrength::getDefense() const { return component->getDefense() + 2; }
card_template_t GiantStrength::render() const {
    return display_enchantment_attack_defence(name, cost, "", "+2", "+2");
}

// --- Enrage ---
Enrage::Enrage(Player* owner, std::shared_ptr<Minion> component)
    : Enchantment("Enrage", 2, owner, component) {}
int Enrage::getAttack() const { return component->getAttack() * 2; }
int Enrage::getDefense() const { return component->getDefense() * 2; }
card_template_t Enrage::render() const {
    return display_enchantment_attack_defence(name, cost, "", "*2", "*2");
}

// --- Haste ---
Haste::Haste(Player* owner, std::shared_ptr<Minion> component)
    : Enchantment("Haste", 1, owner, component) {}
int Haste::getActions() const { return component->getActions() + 1; }
card_template_t Haste::render() const {
    return display_enchantment(name, cost, "Enchanted minion gains +1 action each turn");
}
void Haste::play(Player* p, Player* t, int i) {
    Enchantment::play(p, t, i); // Do the base play logic
    // After play, the minion is wrapped, so we access it via the player's board again.
    t->getMinions()[i]->gainActions(1); // Grant the action immediately
}


// --- Magic Fatigue ---
MagicFatigue::MagicFatigue(Player* owner, std::shared_ptr<Minion> component)
    : Enchantment("Magic Fatigue", 0, owner, component) {}
int MagicFatigue::getAbilityCost() const { return component->getAbilityCost() + 2; }
card_template_t MagicFatigue::render() const {
    return display_enchantment(name, cost, "Enchanted minion's activated ability costs 2 more");
}

// --- Silence ---
Silence::Silence(Player* owner, std::shared_ptr<Minion> component)
    : Enchantment("Silence", 1, owner, component) {}
std::shared_ptr<Ability> Silence::getAbility() const { return nullptr; }
card_template_t Silence::render() const {
    return display_enchantment(name, cost, "Enchanted minion cannot use abilities");
}
