#include "player.h"
#include "game.h"
#include "card.h"
#include "minion.h"
#include "ritual.h"
#include "spell.h"
#include "enchantment.h"
#include "cardfactory.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <random>
#include <chrono>

Player::Player(int id, const std::string& name, Game* game)
    : id(id), name(name), life(20), magic(3), game(game) {
    minions.resize(5, nullptr); // 5 empty minion slots
}

// --- Getters ---
int Player::getPlayerId() const { return id; }
const std::string& Player::getName() const { return name; }
int Player::getLife() const { return life; }
int Player::getMagic() const { return magic; }
Game* Player::getGame() const { return game; }
const std::vector<std::shared_ptr<Card>>& Player::getHand() const { return hand; }
std::vector<std::shared_ptr<Card>>& Player::getHand() { return hand; } // Implementation of non-const version
const std::vector<std::shared_ptr<Minion>>& Player::getMinions() const { return minions; }
std::vector<std::shared_ptr<Minion>>& Player::getMinions() { return minions; } // Implementation of non-const version
const std::vector<std::shared_ptr<Minion>>& Player::getGraveyard() const { return graveyard; }
std::shared_ptr<Ritual> Player::getRitual() const { return ritual; }

// --- Setters & Modifiers ---
void Player::setLife(int new_life) { life = new_life; }
void Player::gainMagic(int amount) { magic += amount; }

void Player::spendMagic(int amount) {
    if (game->isTestingMode()) {
        if (magic < amount) magic = 0;
        else magic -= amount;
        return;
    }
    if (magic < amount) {
        throw std::runtime_error("Not enough magic.");
    }
    magic -= amount;
}

void Player::loadDeck(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error: Could not open deck file " << filename << std::endl;
        // Create the default.deck if it's the one that's missing
        if (filename == "default.deck") {
            system("make default.deck");
            file.open(filename); // Try again
            if (!file) {
                std::cerr << "Fatal: Could not create or open default.deck. Exiting." << std::endl;
                exit(1);
            }
        } else {
            return;
        }
    }
    std::string card_name;
    while (std::getline(file, card_name)) {
        if (!card_name.empty()) {
            deck.push_back(CardFactory::createCard(card_name, this));
        }
    }
}

void Player::shuffleDeck() {
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(deck.begin(), deck.end(), std::default_random_engine(seed));
}

void Player::drawCard() {
    if (deck.empty()) {
        std::cout << getName() << "'s deck is empty!" << std::endl;
        return;
    }
    if (hand.size() >= 5) {
        std::cout << getName() << "'s hand is full!" << std::endl;
        return;
    }
    hand.push_back(deck.back());
    deck.pop_back();
}

void Player::discard(int i) {
    if (i < 0 || i >= (int)hand.size()) {
        throw std::runtime_error("Invalid card index to discard.");
    }
    hand.erase(hand.begin() + i);
}

void Player::addMinion(std::shared_ptr<Minion> minion) {
    for (size_t i = 0; i < minions.size(); ++i) {
        if (!minions[i]) {
            minions[i] = minion;
            game->notifyMinionEnters(minion);
            return;
        }
    }
    throw std::runtime_error("Your board is full. Cannot play minion.");
}

void Player::setRitual(std::shared_ptr<Ritual> new_ritual) {
    ritual = new_ritual;
}

void Player::removeRitual() {
    ritual = nullptr;
}


void Player::resurrect() {
    if (graveyard.empty()) {
        throw std::runtime_error("Graveyard is empty.");
    }
    std::shared_ptr<Minion> minion_to_resurrect = graveyard.back();
    graveyard.pop_back();
    minion_to_resurrect->setDefense(1);
    addMinion(minion_to_resurrect);
}

void Player::bury(std::shared_ptr<Minion> minion) {
    minion->stripEnchantments();
    graveyard.push_back(minion);
}

void Player::resetMinionActions() {
    for (auto& minion : minions) {
        if (minion) {
            minion->gainActions(1);
        }
    }
}

void Player::removeMinion(int i, bool toGraveyard) {
    if (i < 0 || i >= 5 || !minions[i]) {
        throw std::runtime_error("Invalid minion to remove.");
    }
    std::shared_ptr<Minion> removed_minion = minions[i];
    
    game->notifyMinionLeaves(removed_minion);
    
    if (toGraveyard) {
        bury(removed_minion);
    }
    
    minions[i] = nullptr;
    // Shift minions to the left
    for (int j = i; j < 4; ++j) {
        minions[j] = minions[j+1];
        minions[j+1] = nullptr;
    }
}


// --- Player Actions ---

// Play a card without a target (e.g., a ritual or a non-targeted spell)
void Player::play(int i) {
    if (i < 0 || i >= (int)hand.size()) throw std::runtime_error("Invalid card index.");
    
    std::shared_ptr<Card> card_to_play = hand[i];
    
    spendMagic(card_to_play->getCost());
    
    // The card's play method will throw if it's an invalid play (e.g. enchantment w/o target)
    card_to_play->play(this); 
    
    // If play was successful, remove the card from hand. Erasing by index is safe here
    // because the non-targeted play actions do not reorder the hand.
    hand.erase(hand.begin() + i);
}

// Play a card with a target (e.g., an enchantment or a targeted spell)
void Player::play(int i, int p, int t) {
    if (i < 0 || i >= (int)hand.size()) throw std::runtime_error("Invalid card index.");
    std::shared_ptr<Card> card_to_play = hand[i];

    Player* target_player = game->getPlayer(p);
    if (!target_player) throw std::runtime_error("Invalid target player.");

    spendMagic(card_to_play->getCost());
    
    card_to_play->play(this, target_player, t);
    
    // After the effect resolves, find the card in the hand again and erase it.
    // This is safe even if the hand was modified by the card's effect.
    for(auto it = hand.begin(); it != hand.end(); ++it) {
        if (it->get() == card_to_play.get()) {
            hand.erase(it);
            break;
        }
    }
}

void Player::use(int i) {
    if (i < 0 || i >= 5 || !minions[i]) throw std::runtime_error("Invalid minion index.");
    minions[i]->useAbility(this);
}

void Player::use(int i, int p, int t) {
    if (i < 0 || i >= 5 || !minions[i]) throw std::runtime_error("Invalid minion index.");
    
    Player* target_player = game->getPlayer(p);
    if (!target_player) throw std::runtime_error("Invalid target player.");
    
    minions[i]->useAbility(this, target_player, t);
}

void Player::attack(int i) {
    if (i < 0 || i >= 5 || !minions[i]) throw std::runtime_error("Invalid minion index.");
    Player* opponent = game->getNonActivePlayer();
    minions[i]->attack(opponent);
}

void Player::attack(int i, int j) {
    if (i < 0 || i >= 5 || !minions[i]) throw std::runtime_error("Invalid attacker index.");
    
    Player* opponent = game->getNonActivePlayer();
    auto& opponent_minions = opponent->getMinions();
    if (j < 0 || j >= 5 || !opponent_minions[j]) throw std::runtime_error("Invalid target minion index.");

    minions[i]->attack(opponent_minions[j].get());
}
