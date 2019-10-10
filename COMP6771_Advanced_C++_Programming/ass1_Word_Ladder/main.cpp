#include <iostream>
#include <string>
#include "assignments/wl/lexicon.h"
#include "assignments/wl/word_ladder.h"


int main() {
  auto lexicon = GetLexicon("assignments/wl/words.txt");
  std::string start, goal;

  // TODO(students): Replace this with your code
  while (true) {
    std::cout << "Enter start word (RETURN to quit): ";
    std::cin >> start;
    std::cout << "Enter destination word: ";
    std::cin >> goal;
    if (start.size() == goal.size()) {
      break;
    } else {
      std::cout << "Wrong input please input agian!\n";
    }
  }
  Bfs(lexicon, start, goal);
}
