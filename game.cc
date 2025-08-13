#include "game.h"
#include "cardfactory.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <random>
#include <chrono>

// Constructor: Initializes game settings and prepares for setup
Game::Game(const std::string& d1, const std::string& d2, const std::string& init, bool testing, bool graphics)
    : deck1_file(d1), deck2_file(d2), init_file(init), testing_mode(testing), graphics_mode(graphics) {
    if (!init_file.empty()) {
        init_fs = std::make_unique<std::ifstream>(init_file);
        if (!init_fs->is_open()) {
            std::cerr << "Error: Could not open init file " << init_file << std::endl;
            // Fallback to not using an init file
            init_fs.reset(); 
        }
    }
}

Game::~Game() {}

// Sets up the players, decks, and initial game state
void Game::setup() {
    std::istream& in = init_fs ? *init_fs : std::cin;
    
    std::string p1_name, p2_name;
    std::cout << "Enter Player 1's name: " << std::endl;
    std::getline(in, p1_name);
    std::cout << "Enter Player 2's name: " << std::endl;
    std::getline(in, p2_name);

    player1 = std::make_unique<Player>(1, p1_name, this);
    player2 = std::make_unique<Player>(2, p2_name, this);
    board = std::make_unique<Board>(this);

    player1->loadDeck(deck1_file);
    player2->loadDeck(deck2_file);

    if (!testing_mode) {
        player1->shuffleDeck();
        player2->shuffleDeck();
    }

    for (int i = 0; i < 5; ++i) {
        player1->drawCard();
        player2->drawCard();
    }
    
    activePlayer = player1.get();
    nonActivePlayer = player2.get();
}

// Main game loop
void Game::run() {
    setup();

    std::string line;
    std::istream* current_in = init_fs ? init_fs.get() : &std::cin;

    while (true) {
        board->display();
        std::cout << activePlayer->getName() << "'s turn:" << std::endl;

        if (current_in->eof()) {
            if (current_in == init_fs.get()) {
                current_in = &std::cin; // Switch to standard input
            } else {
                break; // EOF from std::cin
            }
        }

        if (!std::getline(*current_in, line)) {
             if (current_in == init_fs.get()) {
                current_in = &std::cin; // Switch to standard input
                if (!std::getline(*current_in, line)) break;
            } else {
                break; // EOF from std::cin
            }
        }

        std::stringstream ss(line);
        std::string cmd;
        ss >> cmd;

        if (cmd.empty()) continue;

        try {
            process_command(cmd, ss);
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }

        if (player1->getLife() <= 0) {
            std::cout << player2->getName() << " wins!" << std::endl;
            break;
        }
        if (player2->getLife() <= 0) {
            std::cout << player1->getName() << " wins!" << std::endl;
            break;
        }
    }
}


// Processes a single command from the input stream
void Game::process_command(const std::string& cmd, std::istream& in) {
    if (cmd == "help") {
        std::cout << "Commands: help, end, quit, attack, play, use, inspect, hand, board" << std::endl;
        if(testing_mode) std::cout << "Testing Commands: draw, discard" << std::endl;
    } else if (cmd == "end") {
        switch_turns();
    } else if (cmd == "quit") {
        throw std::runtime_error("Game quit by user.");
    } else if (cmd == "draw" && testing_mode) {
        activePlayer->drawCard();
    } else if (cmd == "discard" && testing_mode) {
        int i;
        if (in >> i) {
            activePlayer->discard(i - 1);
        } else {
            std::cout << "Invalid discard command." << std::endl;
        }
    } else if (cmd == "attack") {
        int i, j;
        if (in >> i) {
            if (in >> j) { // attack i j
                activePlayer->attack(i - 1, j - 1);
            } else { // attack i
                activePlayer->attack(i - 1);
            }
        } else {
            std::cout << "Invalid attack command." << std::endl;
        }
    } else if (cmd == "play") {
        int i, p, t_val;
        char t_char;
        if (in >> i) {
            if (in >> p >> t_char) { // play i p t
                if (t_char == 'r') {
                    t_val = 6; // Special value for ritual
                } else {
                    // Convert char to int, assuming it's a digit
                    t_val = t_char - '0';
                }
                activePlayer->play(i - 1, p, t_val - 1);
            } else { // play i
                activePlayer->play(i - 1);
            }
        } else {
            std::cout << "Invalid play command." << std::endl;
        }
    } else if (cmd == "use") {
        int i, p, t_val;
        char t_char;
        if (in >> i) {
            if (in >> p >> t_char) { // use i p t
                 if (t_char == 'r') {
                    t_val = 6; // Special value for ritual
                } else {
                    t_val = t_char - '0';
                }
                activePlayer->use(i - 1, p, t_val - 1);
            } else { // use i
                activePlayer->use(i - 1);
            }
        } else {
            std::cout << "Invalid use command." << std::endl;
        }
    } else if (cmd == "inspect") {
        int i;
        if (in >> i) {
            board->inspectMinion(activePlayer->getPlayerId(), i - 1);
        } else {
            std::cout << "Invalid inspect command." << std::endl;
        }
    } else if (cmd == "hand") {
        board->displayHand(activePlayer->getPlayerId());
    } else if (cmd == "board") {
        board->display();
    } else {
        std::cout << "Unknown command: " << cmd << std::endl;
    }
}


// Switches the active player and handles turn start/end logic
void Game::switch_turns() {
    end_turn();
    if (activePlayer->getPlayerId() == 1) {
        activePlayer = player2.get();
        nonActivePlayer = player1.get();
    } else {
        activePlayer = player1.get();
        nonActivePlayer = player2.get();
    }
    start_turn();
}

// Logic for the start of a player's turn
void Game::start_turn() {
    activePlayer->gainMagic(1);
    activePlayer->drawCard();
    activePlayer->resetMinionActions();
    execute_triggers(TriggerType::StartOfTurn);
}

// Logic for the end of a player's turn
void Game::end_turn() {
    execute_triggers(TriggerType::EndOfTurn);
}

// Executes all triggers of a certain type in APNAP order
void Game::execute_triggers(TriggerType type, std::shared_ptr<Minion> target) {
    // APNAP order: Active Player's Minions, Active Player's Ritual,
    // Non-Active Player's Minions, Non-Active Player's Ritual.

    // Active Player Minions
    for (auto& minion : activePlayer->getMinions()) {
        if (minion) minion->useTrigger(type, target);
    }
    // Active Player Ritual
    if (activePlayer->getRitual()) {
        activePlayer->getRitual()->useTrigger(type, target);
    }

    // Non-Active Player Minions
    for (auto& minion : nonActivePlayer->getMinions()) {
        if (minion) minion->useTrigger(type, target);
    }
    // Non-Active Player Ritual
    if (nonActivePlayer->getRitual()) {
        nonActivePlayer->getRitual()->useTrigger(type, target);
    }
}

// Notifies the game that a minion has entered play
void Game::notifyMinionEnters(std::shared_ptr<Minion> m) {
    execute_triggers(TriggerType::MinionEnters, m);
}

// Notifies the game that a minion has left play
void Game::notifyMinionLeaves(std::shared_ptr<Minion> m) {
    execute_triggers(TriggerType::MinionLeaves, m);
}

// Getter for a player by ID
Player* Game::getPlayer(int id) {
    if (id == 1) return player1.get();
    if (id == 2) return player2.get();
    return nullptr;
}

Player* Game::getActivePlayer() { return activePlayer; }
Player* Game::getNonActivePlayer() { return nonActivePlayer; }
Board* Game::getBoard() { return board.get(); }
bool Game::isTestingMode() { return testing_mode; }
