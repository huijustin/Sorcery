#ifndef PLAYER_H
#define PLAYER_H

#include <vector>
#include <string>
#include <memory>

// Forward declarations to avoid circular dependencies
class Card;
class Minion;
class Ritual;
class Game;

class Player {
    int id;
    std::string name;
    int life;
    int magic;
    Game* game; // Raw pointer to game, doesn't own it

    std::vector<std::shared_ptr<Card>> deck;
    std::vector<std::shared_ptr<Card>> hand;
    std::vector<std::shared_ptr<Minion>> minions;
    std::vector<std::shared_ptr<Minion>> graveyard;
    std::shared_ptr<Ritual> ritual;

public:
    Player(int id, const std::string& name, Game* game);

    // Getters
    int getPlayerId() const;
    const std::string& getName() const;
    int getLife() const;
    int getMagic() const;
    Game* getGame() const;
    const std::vector<std::shared_ptr<Card>>& getHand() const; // Const-version for read-only access
    std::vector<std::shared_ptr<Card>>& getHand();             // Non-const version for modification
    const std::vector<std::shared_ptr<Minion>>& getMinions() const; // Const-version for read-only access
    std::vector<std::shared_ptr<Minion>>& getMinions();             // Non-const version for modification
    const std::vector<std::shared_ptr<Minion>>& getGraveyard() const;
    std::shared_ptr<Ritual> getRitual() const;

    // Setters & Modifiers
    void setLife(int new_life);
    void gainMagic(int amount);
    void spendMagic(int amount);
    void loadDeck(const std::string& filename);
    void shuffleDeck();
    void drawCard();
    void discard(int i);
    void addMinion(std::shared_ptr<Minion> minion);
    void setRitual(std::shared_ptr<Ritual> new_ritual);
    void removeRitual();
    void resurrect();
    void bury(std::shared_ptr<Minion> minion);
    void resetMinionActions();
    void removeMinion(int i, bool toGraveyard);

    // Player Actions
    void play(int i);
    void play(int i, int p, int t);
    void use(int i);
    void use(int i, int p, int t);
    void attack(int i);
    void attack(int i, int j);
};

#endif
