#include "spell.h"
#include "player.h"
#include "game.h"
#include <stdexcept>

Spell::Spell(const std::string& name, int cost, Player* owner, const std::string& desc,
             bool req_target, std::function<void(Player*, Player*, int)> effect)
    : Card(name, cost, owner, CardType::Spell), description(desc), effect(effect), requires_target(req_target) {}

// Play without a target
void Spell::play(Player* p) {
    if (requires_target) {
        throw std::runtime_error("This spell requires a target.");
    }
    effect(p, nullptr, -1);
}

// Play with a target
void Spell::play(Player* p, Player* t, int i) {
    if (!requires_target) {
        throw std::runtime_error("This spell does not take a target.");
    }
    effect(p, t, i);
}

// Render the spell card
card_template_t Spell::render() const {
    return display_spell(name, cost, description);
}
