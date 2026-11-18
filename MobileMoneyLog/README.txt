Mobile Money Agent Transaction Log (C++17)

Files
  LinkedList.h    singly linked list with head and tail (class template)  - A1
  Transaction.h   the record struct and file load / save                  - A1
  Algorithms.h    insertion sort, merge sort, binary search               - A2
  main.cpp        the menu program
  tests.cpp       automatic tests                                         - A3
  timing.cpp      timing experiment for both sorts                        - A4
  transactions.txt  sample data (12 records)
  Makefile        build recipes

Build and run
  make            builds app, tests and timing
  ./app           run the menu program
  ./tests         run the tests
  make asan       build and run the tests with AddressSanitizer
  ./timing > timing.csv    run the timing experiment

Without make
  g++ -std=c++17 -Wall -Wextra -o app main.cpp
