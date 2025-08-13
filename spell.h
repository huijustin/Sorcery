#ifndef SPELL_H
#define SPELL_H

#include "card.h"
#include <functional>

class Game;

// Spell class, inherits from Card
class Spell : public Card {
    std::string description;
    // The spell's effect is a function
    std::function<void(Player*, Player*, int)> effect; 
    bool requires_target;

public:
    Spell(const std::string& name, int cost, Player* owner, const std::string& desc, 
          bool req_target, std::function<void(Player*, Player*, int)> effect);

    void play(Player* p) override;
    void play(Player* p, Player* t, int i) override;
    card_template_t render() const override;
};

#endif
