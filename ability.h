#ifndef ABILITY_H
#define ABILITY_H

#include <string>
#include <memory>

class Player;

// Base class for all abilities
class Ability {
protected:
    int cost;
    std::string description;
public:
    Ability(int cost, const std::string& desc);
    virtual ~Ability() = default;
    virtual void apply(Player* self, Player* target_player, int target_card_idx) = 0;
    int getCost() const;
    const std::string& getDescription() const;
};

// --- Specific Ability Implementations ---

class NovicePyromancerAbility : public Ability {
public:
    NovicePyromancerAbility();
    void apply(Player* self, Player* target_player, int target_card_idx) override;
};

class ApprenticeSummonerAbility : public Ability {
public:
    ApprenticeSummonerAbility();
    void apply(Player* self, Player* target_player, int target_card_idx) override;
};

class MasterSummonerAbility : public Ability {
public:
    MasterSummonerAbility();
    void apply(Player* self, Player* target_player, int target_card_idx) override;
};

// Trigger abilities
class BoneGolemAbility : public Ability {
public:
    BoneGolemAbility();
    void apply(Player* self, Player* target_player, int target_card_idx) override;
};

class FireElementalAbility : public Ability {
public:
    FireElementalAbility();
    void apply(Player* self, Player* target_player, int target_card_idx) override;
};

class PotionSellerAbility : public Ability {
public:
    PotionSellerAbility();
    void apply(Player* self, Player* target_player, int target_card_idx) override;
};

// Ritual abilities
class DarkRitualAbility : public Ability {
public:
    DarkRitualAbility();
    void apply(Player* self, Player* target_player, int target_card_idx) override;
};

class AuraOfPowerAbility : public Ability {
public:
    AuraOfPowerAbility();
    void apply(Player* self, Player* target_player, int target_card_idx) override;
};

class StandstillAbility : public Ability {
public:
    StandstillAbility();
    void apply(Player* self, Player* target_player, int target_card_idx) override;
};

#endif
