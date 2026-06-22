# ============================================================================
#  Makefile alternativo (para quem nao usa CMake).
#  Uso:  make        -> compila o jogo em ./OrbRush
#        make run    -> compila e executa
#        make clean  -> remove artefatos
# ============================================================================

CXX      := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -Iinclude
LDLIBS   := -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

# Caminhos do Homebrew no macOS (SFML 3 costuma ser keg-only).
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    ifneq ($(wildcard /opt/homebrew/opt/sfml/include/SFML),)
        CXXFLAGS += -I/opt/homebrew/opt/sfml/include
        LDLIBS   += -L/opt/homebrew/opt/sfml/lib
    else ifneq ($(wildcard /usr/local/opt/sfml/include/SFML),)
        CXXFLAGS += -I/usr/local/opt/sfml/include
        LDLIBS   += -L/usr/local/opt/sfml/lib
    endif
endif

SRCS   := $(wildcard src/*.cpp)
TARGET := OrbRush

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET) $(LDLIBS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: run clean
