#include "board.h"
#include "game.h"
#include "player.h"
#include "card.h"
#include "minion.h"
#include "ritual.h"
#include <iostream>
#include <vector>

Board::Board(Game* game) : game(game) {}

// Prints a vector of card_template_t side-by-side
void Board::print_card_row(const std::vector<card_template_t>& cards) {
    if (cards.empty()) return;

    size_t num_lines = cards[0].size();
    for (size_t i = 0; i < num_lines; ++i) {
        for (const auto& card : cards) {
            if (i < card.size()) {
                std::cout << card[i];
            }
        }
        std::cout << std::endl;
    }
}

// Displays the entire game board
void Board::display() {
    Player* p1 = game->getPlayer(1);
    Player* p2 = game->getPlayer(2);

    const std::string border_h = std::string(185, EXTERNAL_BORDER_CHAR_LEFT_RIGHT[0]);
    std::cout << EXTERNAL_BORDER_CHAR_TOP_LEFT << border_h << EXTERNAL_BORDER_CHAR_TOP_RIGHT << std::endl;

    // --- Player 1 Row (Ritual, Player Card, Graveyard) ---
    std::vector<card_template_t> p1_top_row;
    p1_top_row.push_back(p1->getRitual() ? p1->getRitual()->render() : CARD_TEMPLATE_BORDER);
    p1_top_row.push_back(CARD_TEMPLATE_EMPTY);
    p1_top_row.push_back(display_player_card(1, p1->getName(), p1->getLife(), p1->getMagic()));
    p1_top_row.push_back(CARD_TEMPLATE_EMPTY);
    auto& p1_graveyard = p1->getGraveyard();
    p1_top_row.push_back(!p1_graveyard.empty() ? p1_graveyard.back()->render() : CARD_TEMPLATE_BORDER);
    print_card_row(p1_top_row);

    // --- Player 1 Minions ---
    std::vector<card_template_t> p1_minions_row;
    for (const auto& minion : p1->getMinions()) {
        p1_minions_row.push_back(minion ? minion->render() : CARD_TEMPLATE_BORDER);
    }
    print_card_row(p1_minions_row);

    // --- Center Graphic ---
    for (const auto& line : CENTRE_GRAPHIC) {
        std::cout << line << std::endl;
    }

    // --- Player 2 Minions ---
    std::vector<card_template_t> p2_minions_row;
    for (const auto& minion : p2->getMinions()) {
        p2_minions_row.push_back(minion ? minion->render() : CARD_TEMPLATE_BORDER);
    }
    print_card_row(p2_minions_row);

    // --- Player 2 Row (Ritual, Player Card, Graveyard) ---
    std::vector<card_template_t> p2_bottom_row;
    p2_bottom_row.push_back(p2->getRitual() ? p2->getRitual()->render() : CARD_TEMPLATE_BORDER);
    p2_bottom_row.push_back(CARD_TEMPLATE_EMPTY);
    p2_bottom_row.push_back(display_player_card(2, p2->getName(), p2->getLife(), p2->getMagic()));
    p2_bottom_row.push_back(CARD_TEMPLATE_EMPTY);
    auto& p2_graveyard = p2->getGraveyard();
    p2_bottom_row.push_back(!p2_graveyard.empty() ? p2_graveyard.back()->render() : CARD_TEMPLATE_BORDER);
    print_card_row(p2_bottom_row);

    std::cout << EXTERNAL_BORDER_CHAR_BOTTOM_LEFT << border_h << EXTERNAL_BORDER_CHAR_BOTTOM_RIGHT << std::endl;
}

// Displays the hand of a specific player
void Board::displayHand(int player_id) {
    Player* player = game->getPlayer(player_id);
    if (!player) return;

    std::vector<card_template_t> hand_row;
    for (const auto& card : player->getHand()) {
        hand_row.push_back(card->render());
    }
    print_card_row(hand_row);
}

// Displays a minion and all its enchantments
void Board::inspectMinion(int player_id, int minion_idx) {
    Player* player = game->getPlayer(player_id);
    if (!player || minion_idx < 0 || minion_idx >= 5) {
        std::cout << "Invalid minion to inspect." << std::endl;
        return;
    }

    const auto& minion = player->getMinions()[minion_idx];
    if (!minion) {
        std::cout << "No minion at that position." << std::endl;
        return;
    }

    // Get the list of enchantments from the minion
    auto enchantments = minion->getEnchantments();

    // Print the base minion
    print_card_row({minion->renderBase()});

    // Print enchantments, 5 per line
    if (!enchantments.empty()) {
        std::cout << "Enchantments:" << std::endl;
        std::vector<card_template_t> enchantment_row;
        for (size_t i = 0; i < enchantments.size(); ++i) {
            enchantment_row.push_back(enchantments[i]->render());
            if (enchantment_row.size() == 5 || i == enchantments.size() - 1) {
                print_card_row(enchantment_row);
                enchantment_row.clear();
            }
        }
    }
}
