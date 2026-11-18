FLAGS=-std=c++17 -Wall -Wextra -g
all: app tests timing
app: main.cpp *.h ; g++ $(FLAGS) -o $@ main.cpp
tests: tests.cpp *.h ; g++ $(FLAGS) -o $@ tests.cpp
timing: timing.cpp Algorithms.h ; g++ -std=c++17 -O2 -o $@ timing.cpp
asan: ; g++ $(FLAGS) -fsanitize=address,undefined -fno-omit-frame-pointer -o tests_asan tests.cpp && ./tests_asan
