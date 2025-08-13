#ifndef MINION_H
#define MINION_H

#include "card.h"
#include <vector>
#include <memory> // Required for std::enable_shared_from_this

class Ability;
class Enchantment;

// Minion class, inherits from Card. This is the "Component" in the Decorator pattern.
// It now also inherits from std::enable_shared_from_this to allow safe creation of shared_ptrs from 'this'.
class Minion : public Card, public std::enable_shared_from_this<Minion> {
protected:
    int attackVal; // Renamed from 'attack' to avoid name collision
    int defenseVal; // Renamed from 'defense' for consistency
    int actions;
    std::shared_ptr<Ability> ability;
    TriggerType triggerType = TriggerType::None;
    std::string triggerDesc;

    // The core of the Decorator pattern: a pointer to the enchantment wrapping this minion
    std::shared_ptr<Minion> component; 

public:
    Minion(const std::string& name, int cost, Player* owner, int attack, int defense, 
           std::shared_ptr<Ability> ability = nullptr, 
           TriggerType triggerType = TriggerType::None, const std::string& triggerDesc = "");

    // Getters that can be decorated
    virtual int getAttack() const;
    virtual int getDefense() const;
    virtual int getActions() const;
    virtual int getAbilityCost() const;
    virtual std::shared_ptr<Ability> getAbility() const;

    // Setters
    void setDefense(int new_defense);
    void takeDamage(int amount);
    void gainActions(int amount);
    void spendAction();

    // Core game actions (no longer collides with member variable)
    void attack(Player* target);
    void attack(Minion* target);

    // Ability and Trigger methods
    void useAbility(Player* p);
    void useAbility(Player* p, Player* t, int i);
    void useTrigger(TriggerType type, std::shared_ptr<Minion> target);

    // Decorator pattern methods
    void addEnchantment(std::shared_ptr<Enchantment> ench);
    std::shared_ptr<Minion> stripTopEnchantment();
    void stripEnchantments();
    bool isDead() const;

    // Rendering methods
    card_template_t render() const override;
    virtual card_template_t renderBase() const;
    virtual std::vector<std::shared_ptr<Enchantment>> getEnchantments() const;
};

#endif
