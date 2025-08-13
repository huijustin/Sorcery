#ifndef GAME_H
#define GAME_H

#include <memory>
#include <vector>
#include <string>
#include "player.h"
#include "board.h"
#include "card.h"
#include "ritual.h"
#include "minion.h"
#include "spell.h"
#include "enchantment.h"
#include "ability.h"

class Game {
    std::unique_ptr<Player> player1;
    std::unique_ptr<Player> player2;
    std::unique_ptr<Board> board;

    Player* activePlayer = nullptr;
    Player* nonActivePlayer = nullptr;

    std::string deck1_file;
    std::string deck2_file;
    std::string init_file;
    bool testing_mode;
    bool graphics_mode; // Note: Graphics mode is not implemented in this version

    std::unique_ptr<std::ifstream> init_fs; // Input stream for init file

    void setup();
    void switch_turns();
    void start_turn();
    void end_turn();
    void process_command(const std::string& cmd, std::istream& in);

public:
    Game(const std::string& d1, const std::string& d2, const std::string& init, bool testing, bool graphics);
    ~Game();

    void run();
    Player* getPlayer(int id);
    Player* getActivePlayer();
    Player* getNonActivePlayer();
    Board* getBoard();
    bool isTestingMode();

    // Trigger notification methods
    void notifyMinionEnters(std::shared_ptr<Minion> m);
    void notifyMinionLeaves(std::shared_ptr<Minion> m);
    void notifyTurnStart();
    void notifyTurnEnd();

private:
    void execute_triggers(TriggerType type, std::shared_ptr<Minion> target = nullptr);
};

#endif
