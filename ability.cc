#include "ability.h"
#include "player.h"
#include "minion.h"
#include "cardfactory.h"
#include <stdexcept>
#include <iostream>

Ability::Ability(int cost, const std::string& desc) : cost(cost), description(desc) {}
int Ability::getCost() const { return cost; }
const std::string& Ability::getDescription() const { return description; }

// --- Novice Pyromancer ---
NovicePyromancerAbility::NovicePyromancerAbility() : Ability(1, "Deal 1 damage to target minion") {}
void NovicePyromancerAbility::apply(Player* self, Player* target_player, int target_card_idx) {
    if (!target_player || target_card_idx < 0 || target_card_idx > 4) throw std::runtime_error("Invalid target for Novice Pyromancer.");
    auto& target_minions = target_player->getMinions();
    if (!target_minions[target_card_idx]) throw std::runtime_error("Target minion does not exist.");
    target_minions[target_card_idx]->takeDamage(1);
}

// --- Apprentice Summoner ---
ApprenticeSummonerAbility::ApprenticeSummonerAbility() : Ability(1, "Summon a 1/1 air elemental") {}
void ApprenticeSummonerAbility::apply(Player* self, Player* target_player, int target_card_idx) {
    try {
        self->addMinion(std::static_pointer_cast<Minion>(CardFactory::createCard("Air Elemental", self)));
    } catch (const std::runtime_error& e) {
        std::cout << "Could not summon Air Elemental: " << e.what() << std::endl;
    }
}

// --- Master Summoner ---
MasterSummonerAbility::MasterSummonerAbility() : Ability(2, "Summon up to three 1/1 air elementals") {}
void MasterSummonerAbility::apply(Player* self, Player* target_player, int target_card_idx) {
    for (int i = 0; i < 3; ++i) {
        try {
            self->addMinion(std::static_pointer_cast<Minion>(CardFactory::createCard("Air Elemental", self)));
        } catch (const std::runtime_error& e) {
            std::cout << "Board is full, stopping summoning." << std::endl;
            break;
        }
    }
}

// --- Bone Golem ---
BoneGolemAbility::BoneGolemAbility() : Ability(0, "Gains +1/+1 whenever a minion leaves play.") {}
void BoneGolemAbility::apply(Player* self, Player* target_player, int target_card_idx) {
    // This ability is a trigger, but its effect is not directly implemented here.
    // The effect is hardcoded in the trigger logic for simplicity in this implementation.
    // A more robust system would have this method modify the Bone Golem.
    // This is a known simplification.
}


// --- Fire Elemental ---
FireElementalAbility::FireElementalAbility() : Ability(0, "Whenever an opponent's minion enters play, deal 1 damage to it.") {}
void FireElementalAbility::apply(Player* self, Player* target_player, int target_card_idx) {
    if (target_player && target_player != self && target_card_idx != -1) {
        auto& minions = target_player->getMinions();
        if (minions[target_card_idx]) {
            minions[target_card_idx]->takeDamage(1);
        }
    }
}

// --- Potion Seller ---
PotionSellerAbility::PotionSellerAbility() : Ability(0, "At the end of your turn, all your minions gain +0/+1.") {}
void PotionSellerAbility::apply(Player* self, Player* target_player, int target_card_idx) {
    for (auto& minion : self->getMinions()) {
        if (minion) {
            minion->setDefense(minion->getDefense() + 1);
        }
    }
}

// --- Dark Ritual ---
DarkRitualAbility::DarkRitualAbility() : Ability(0, "At the start of your turn, gain 1 magic") {}
void DarkRitualAbility::apply(Player* self, Player* target_player, int target_card_idx) {
    self->gainMagic(1);
}

// --- Aura of Power ---
AuraOfPowerAbility::AuraOfPowerAbility() : Ability(0, "Whenever a minion enters play under your control, it gains +1/+1") {}
void AuraOfPowerAbility::apply(Player* self, Player* target_player, int target_card_idx) {
     if (target_player == self && target_card_idx != -1) {
        auto& minions = self->getMinions();
        if (minions[target_card_idx]) {
            // This is a conceptual issue in the design. We don't have access to modify the minion directly here.
            // This effect is also hardcoded for simplicity.
        }
    }
}

// --- Standstill ---
StandstillAbility::StandstillAbility() : Ability(0, "Whenever a minion enters play, destroy it") {}
void StandstillAbility::apply(Player* self, Player* target_player, int target_card_idx) {
    if (target_player && target_card_idx != -1) {
        auto& minions = target_player->getMinions();
        if (minions[target_card_idx]) {
            minions[target_card_idx]->setDefense(0);
        }
    }
}
