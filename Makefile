NAME=main

CXXFLAGS += -O3 -flto -march=native -g
CXXFLAGS += -Wall -Wextra -Wpedantic -std=c++17

LIBS := libpng glfw3

LDLIBS += -lm $(shell pkg-config $(LIBS) --libs)
CXXFLAGS += $(shell pkg-config $(LIBS) --cflags)

OBJ := main.o

all: $(NAME)

run: all
	./$(NAME)

force: clean
	$(MAKE) all

clean:
	rm -f $(OBJ) $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OBJ) $(LDLIBS) -o $@


main.o: result.h

.PHONY: all run clean force
