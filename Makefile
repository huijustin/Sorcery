# Makefile for Sorcery

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Werror -g
# Set to 1 for the simple graphics shown in the PDF, 0 for fancier unicode graphics
SIMPLE_GRAPHICS_FLAG = -DSIMPLE_GRAPHICS=0

# All source files
SRCS = main.cc ascii_graphics.cc card.cc player.cc board.cc game.cc \
       minion.cc spell.cc ritual.cc enchantment.cc ability.cc cardfactory.cc

# All object files
OBJS = $(SRCS:.cc=.o)

# Executable name
EXEC = sorcery

# Default target
all: $(EXEC)

# Rule to link the executable
$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(EXEC)

# Rule to compile .cc files into .o files
%.o: %.cc
	$(CXX) $(CXXFLAGS) $(SIMPLE_GRAPHICS_FLAG) -c $< -o $@

# Target to clean up generated files
clean:
	rm -f $(OBJS) $(EXEC) default.deck

# Create a default deck file for convenience
default.deck:
	@echo "Air Elemental" > default.deck
	@echo "Earth Elemental" >> default.deck
	@echo "Bone Golem" >> default.deck
	@echo "Fire Elemental" >> default.deck
	@echo "Potion Seller" >> default.deck
	@echo "Novice Pyromancer" >> default.deck
	@echo "Apprentice Summoner" >> default.deck
	@echo "Master Summoner" >> default.deck
	@echo "Banish" >> default.deck
	@echo "Unsummon" >> default.deck
	@echo "Recharge" >> default.deck
	@echo "Disenchant" >> default.deck
	@echo "Raise Dead" >> default.deck
	@echo "Blizzard" >> default.deck
	@echo "Giant Strength" >> default.deck
	@echo "Enrage" >> default.deck
	@echo "Haste" >> default.deck
	@echo "Magic Fatigue" >> default.deck
	@echo "Silence" >> default.deck
	@echo "Dark Ritual" >> default.deck
	@echo "Aura of Power" >> default.deck
	@echo "Standstill" >> default.deck

# Phony targets
.PHONY: all clean
