#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include "game.h"

// Main function: Entry point of the program
int main(int argc, char *argv[]) {
    // --- Command Line Argument Parsing ---
    std::string deck1_file = "default.deck";
    std::string deck2_file = "default.deck";
    std::string init_file = "";
    bool testing_mode = false;
    bool graphics_mode = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-deck1") {
            if (i + 1 < argc) {
                deck1_file = argv[++i];
            }
        } else if (arg == "-deck2") {
            if (i + 1 < argc) {
                deck2_file = argv[++i];
            }
        } else if (arg == "-init") {
            if (i + 1 < argc) {
                init_file = argv[++i];
            }
        } else if (arg == "-testing") {
            testing_mode = true;
        } else if (arg == "-graphics") {
            graphics_mode = true;
        }
    }

    // --- Game Initialization ---
    // The 'cin.exceptions(ios::eofbit)' line is crucial for handling Ctrl-D (EOF)
    // as an exception, which allows the program to terminate gracefully when input
    // is redirected from a file that ends, or when the user signals EOF.
    std::cin.exceptions(std::ios::eofbit);
    try {
        // Create the game object with the parsed settings
        auto game = std::make_unique<Game>(deck1_file, deck2_file, init_file, testing_mode, graphics_mode);
        
        // Run the game
        game->run();

    } catch (const std::ios::failure &e) {
        // This block catches the EOF exception. 
        // It can be left empty if no special action is needed on EOF.
        // This is a clean way to exit when input redirection ends.
    } catch (const std::exception &e) {
        // Catch any other standard exceptions that might occur
        std::cerr << "An unexpected error occurred: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
