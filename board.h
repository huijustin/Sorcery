#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <memory>
#include "ascii_graphics.h"
#include "enchantment.h"

class Game;
class Player;
class Card;
class Minion;

class Board {
    Game* game; // Raw pointer, does not own

    // Helper to print a row of cards
    void print_card_row(const std::vector<card_template_t>& cards);

public:
    Board(Game* game);
    void display();
    void displayHand(int player_id);
    void inspectMinion(int player_id, int minion_idx);
};

#endif
