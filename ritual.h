#ifndef RITUAL_H
#define RITUAL_H

#include "card.h"
#include "minion.h"

class Ability;
class Minion;

// Ritual class, inherits from Card
class Ritual : public Card {
    int charges;
    int activation_cost;
    std::shared_ptr<Ability> ability;
    TriggerType triggerType;
    std::string triggerDesc;

public:
    Ritual(const std::string& name, int cost, Player* owner, int charges, int activation_cost, 
           TriggerType triggerType, const std::string& triggerDesc, std::shared_ptr<Ability> ability);

    void play(Player* p) override;
    card_template_t render() const override;
    void useTrigger(TriggerType type, std::shared_ptr<Minion> target);
    void gainCharges(int amount);
};

#endif
