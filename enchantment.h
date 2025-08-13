#ifndef ENCHANTMENT_H
#define ENCHANTMENT_H

#include "minion.h"

// Enchantment class, inherits from Minion. This is the "Decorator" in the pattern.
class Enchantment : public Minion {
protected:
    std::shared_ptr<Minion> component;

public:
    Enchantment(const std::string& name, int cost, Player* owner, std::shared_ptr<Minion> component);
    virtual ~Enchantment() = default;

    // Override play methods for enchantments
    void play(Player* p, Player* t, int i) override;
    void play(Player* p) override; // <-- ADDED DECLARATION

    // Pass-through methods that call the component
    int getAttack() const override;
    int getDefense() const override;
    int getActions() const override;
    std::shared_ptr<Ability> getAbility() const override;
    int getAbilityCost() const override;
    card_template_t renderBase() const override;
    std::vector<std::shared_ptr<Enchantment>> getEnchantments() const override;

    // Decorator-specific methods
    void setComponent(std::shared_ptr<Minion> comp);
    std::shared_ptr<Minion> getComponent() const;
};

// --- Specific Enchantment Implementations ---

class GiantStrength : public Enchantment {
public:
    GiantStrength(Player* owner, std::shared_ptr<Minion> component = nullptr);
    int getAttack() const override;
    int getDefense() const override;
    card_template_t render() const override;
};

class Enrage : public Enchantment {
public:
    Enrage(Player* owner, std::shared_ptr<Minion> component = nullptr);
    int getAttack() const override;
    int getDefense() const override;
    card_template_t render() const override;
};

class Haste : public Enchantment {
public:
    Haste(Player* owner, std::shared_ptr<Minion> component = nullptr);
    int getActions() const override;
    card_template_t render() const override;
    void play(Player* p, Player* t, int i) override;
};

class MagicFatigue : public Enchantment {
public:
    MagicFatigue(Player* owner, std::shared_ptr<Minion> component = nullptr);
    int getAbilityCost() const override;
    card_template_t render() const override;
};

class Silence : public Enchantment {
public:
    Silence(Player* owner, std::shared_ptr<Minion> component = nullptr);
    std::shared_ptr<Ability> getAbility() const override;
    card_template_t render() const override;
};

#endif
