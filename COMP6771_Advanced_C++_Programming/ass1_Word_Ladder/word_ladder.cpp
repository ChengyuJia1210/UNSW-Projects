#include <algorithm>
#include <deque>
#include <fstream>
#include <iostream>
#include <set>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <utility>
#include "assignments/wl/word_ladder.h"
#include "assignments/wl/lexicon.h"


void GetPath(std::unordered_map<std::string, std::vector<std::string>> graph,
             std::string start,
             std::string goal,
             std::vector<std::string>& path,
             std::vector<std::vector<std::string>>& paths) {
  std::stack<std::pair<std::string, std::vector<std::string>>> stack1;
  std::pair<std::string, std::vector<std::string>> pair1{start, path};
  stack1.push(pair1);
  std::string key;

  while (!stack1.empty()) {
    pair1 = stack1.top();
    stack1.pop();
    key = pair1.first;
    path = pair1.second;
    for (const auto& item : graph[key]) {
      if (item == goal) {
        path.push_back(item);
        paths.push_back(path);
        path.pop_back();
      } else {
        path.push_back(item);
        pair1.first = item;
        pair1.second = path;
        stack1.push(pair1);
        path.pop_back();
      }
    }
  }
}

bool CompareFunction(std::string a, std::string b) {
  return a < b;
}

void Search(std::unordered_set<std::string>& lexicon,
            std::unordered_map<std::string, std::vector<std::string>>& graph,
            std::unordered_set<std::string> current_level,
            std::unordered_set<std::string> temp_level,
            std::string goal) {
  std::unordered_set<std::string> shorter_lexicon;
  std::deque<std::unordered_set<std::string>> deque_set;
  std::string copy_key;

  for (const auto& item1 : lexicon) {
    if (item1.size() == goal.size()) {
      shorter_lexicon.insert(item1);
    }
  }
  deque_set.push_back(current_level);

  while (!deque_set.empty()) {
    current_level = deque_set.front();
    deque_set.pop_front();
    if (current_level.count(goal) != 0) {
      break;
    }
    for (const auto item : current_level) {
      shorter_lexicon.erase(item);
    }
    temp_level.clear();
    for (const auto& key : current_level) {
      for (char c = 'a'; c <= 'z'; ++c) {
        for (unsigned long index = 0; index < key.size(); ++index) {
          copy_key = key;
          copy_key[index] = c;
          if (key != copy_key && shorter_lexicon.count(copy_key) != 0) {
            graph[key].push_back(copy_key);
            temp_level.insert(copy_key);
          }
        }
      }
      std::sort(graph[key].begin(), graph[key].end(), CompareFunction);
    }
    deque_set.push_back(temp_level);
  }
}

std::string Bfs(std::unordered_set<std::string>& lexicon, std::string& start, std::string& goal) {
  std::unordered_map<std::string, std::vector<std::string>> graph;
  std::unordered_set<std::string> current_level{start};
  std::unordered_set<std::string> temp_level;
  std::vector<std::vector<std::string>> paths;
  std::vector<std::string> path = {start};
  std::stack<std::vector<std::string>> new_paths;
  std::string string_path = "";
  bool flag = false;

  Search(lexicon, graph, current_level, temp_level, goal);
  GetPath(graph, start, goal, path, paths);

  for (const auto& item : paths) {
    new_paths.push(item);
    flag = true;
  }
  if (flag) {
    string_path += "Found ladder: ";
    std::cout << "Found ladder: ";
    for (const auto& item : paths) {
      item.size();
      auto item1 = new_paths.top();
      new_paths.pop();
      for (const auto& item2 : item1) {
        string_path += item2;
        string_path += " ";
        std::cout << item2 << " ";
      }
      string_path += "\n";
      std::cout << "\n";
    }
  } else {
    string_path += "No ladder found.\n";
    std::cout << "No ladder found.\n";
  }
  return string_path;
}
