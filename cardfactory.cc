#include "cardfactory.h"
#include "card.h"
#include "minion.h"
#include "spell.h"
#include "ritual.h"
#include "enchantment.h"
#include "ability.h"
#include "player.h"
#include "game.h"
#include <stdexcept>
#include <vector>

// Helper lambda for Banish spell effect
auto banish_effect = [](Player* self, Player* target_player, int target_card_idx) {
    if (!target_player) throw std::runtime_error("Banish requires a target.");
    if (target_card_idx == 5) { // Target is a ritual (index 5 is special case)
        target_player->removeRitual();
    } else if (target_card_idx >= 0 && target_card_idx < 5) { // Target is a minion
        target_player->removeMinion(target_card_idx, true);
    } else {
        throw std::runtime_error("Invalid target for Banish.");
    }
};

// Helper lambda for Unsummon spell effect
auto unsummon_effect = [](Player* self, Player* target_player, int target_card_idx) {
    if (!target_player || target_card_idx < 0 || target_card_idx > 4) throw std::runtime_error("Invalid target for Unsummon.");
    auto& target_minions = target_player->getMinions();
    if (!target_minions[target_card_idx]) throw std::runtime_error("Target minion does not exist.");
    
    // Return card to owner's hand
    auto minion_card = target_minions[target_card_idx];
    minion_card->stripEnchantments(); // Remove enchantments
    target_player->getHand().push_back(minion_card);
    target_player->removeMinion(target_card_idx, false); // Remove from board, don't send to graveyard
};

// Helper lambda for Recharge spell effect
auto recharge_effect = [](Player* self, Player* target_player, int target_card_idx) {
    if (!self->getRitual()) throw std::runtime_error("You have no ritual to recharge.");
    self->getRitual()->gainCharges(3);
};

// Helper lambda for Disenchant spell effect
auto disenchant_effect = [](Player* self, Player* target_player, int target_card_idx) {
    if (!target_player || target_card_idx < 0 || target_card_idx > 4) throw std::runtime_error("Invalid target for Disenchant.");
    auto& target_minions = target_player->getMinions();
    if (!target_minions[target_card_idx]) throw std::runtime_error("Target minion does not exist.");
    target_minions[target_card_idx]->stripTopEnchantment();
};

// Helper lambda for Raise Dead spell effect
auto raise_dead_effect = [](Player* self, Player* target_player, int target_card_idx) {
    self->resurrect();
};

// Helper lambda for Blizzard spell effect
auto blizzard_effect = [](Player* self, Player* target_player, int target_card_idx) {
    // Damage all minions on the board
    for (auto& minion : self->getMinions()) {
        if (minion) minion->takeDamage(2);
    }
    Player* opponent = self->getPlayerId() == 1 ? self->getGame()->getPlayer(2) : self->getGame()->getPlayer(1);
    for (auto& minion : opponent->getMinions()) {
        if (minion) minion->takeDamage(2);
    }
};


// The factory method itself
std::shared_ptr<Card> CardFactory::createCard(const std::string& cardName, Player* owner) {
    // Minions
    if (cardName == "Air Elemental") return std::make_shared<Minion>(cardName, 0, owner, 1, 1);
    if (cardName == "Earth Elemental") return std::make_shared<Minion>(cardName, 3, owner, 4, 4);
    if (cardName == "Bone Golem") return std::make_shared<Minion>(cardName, 2, owner, 1, 3, std::make_shared<BoneGolemAbility>(), TriggerType::MinionLeaves, "Gain +1/+1 whenever a minion leaves play.");
    if (cardName == "Fire Elemental") return std::make_shared<Minion>(cardName, 2, owner, 2, 2, std::make_shared<FireElementalAbility>(), TriggerType::MinionEnters, "Whenever an opponent's minion enters play, deal 1 damage to it.");
    if (cardName == "Potion Seller") return std::make_shared<Minion>(cardName, 2, owner, 1, 3, std::make_shared<PotionSellerAbility>(), TriggerType::EndOfTurn, "At the end of your turn, all your minions gain +0/+1.");
    if (cardName == "Novice Pyromancer") return std::make_shared<Minion>(cardName, 1, owner, 0, 1, std::make_shared<NovicePyromancerAbility>());
    if (cardName == "Apprentice Summoner") return std::make_shared<Minion>(cardName, 1, owner, 1, 1, std::make_shared<ApprenticeSummonerAbility>());
    if (cardName == "Master Summoner") return std::make_shared<Minion>(cardName, 3, owner, 2, 3, std::make_shared<MasterSummonerAbility>());

    // Spells
    if (cardName == "Banish") return std::make_shared<Spell>(cardName, 2, owner, "Destroy target minion or ritual", true, banish_effect);
    if (cardName == "Unsummon") return std::make_shared<Spell>(cardName, 1, owner, "Return target minion to its owner's hand", true, unsummon_effect);
    if (cardName == "Recharge") return std::make_shared<Spell>(cardName, 1, owner, "Your ritual gains 3 charges", false, recharge_effect);
    if (cardName == "Disenchant") return std::make_shared<Spell>(cardName, 1, owner, "Destroy the top enchantment on target minion", true, disenchant_effect);
    if (cardName == "Raise Dead") return std::make_shared<Spell>(cardName, 1, owner, "Resurrect the top minion in your graveyard and set its defense to 1", false, raise_dead_effect);
    if (cardName == "Blizzard") return std::make_shared<Spell>(cardName, 3, owner, "Deal 2 damage to all minions", false, blizzard_effect);

    // Enchantments
    if (cardName == "Giant Strength") return std::make_shared<GiantStrength>(owner);
    if (cardName == "Enrage") return std::make_shared<Enrage>(owner);
    if (cardName == "Haste") return std::make_shared<Haste>(owner);
    if (cardName == "Magic Fatigue") return std::make_shared<MagicFatigue>(owner);
    if (cardName == "Silence") return std::make_shared<Silence>(owner);

    // Rituals
    if (cardName == "Dark Ritual") return std::make_shared<Ritual>(cardName, 0, owner, 5, 1, TriggerType::StartOfTurn, "At the start of your turn, gain 1 magic", std::make_shared<DarkRitualAbility>());
    if (cardName == "Aura of Power") return std::make_shared<Ritual>(cardName, 1, owner, 4, 1, TriggerType::MinionEnters, "Whenever a minion enters play under your control, it gains +1/+1", std::make_shared<AuraOfPowerAbility>());
    if (cardName == "Standstill") return std::make_shared<Ritual>(cardName, 3, owner, 4, 2, TriggerType::MinionEnters, "Whenever a minion enters play, destroy it", std::make_shared<StandstillAbility>());

    throw std::runtime_error("Unknown card name: " + cardName);
}
