#include "graph.h"
#include <vector>
#include <memory>
#include <iostream>
#include <string>

int main(){

  gdwg::Graph<int, double> g;
  g.InsertNode(1);
  g.InsertNode(2);
  g.InsertNode(3);
  g.InsertEdge(1,2,3);
  g.InsertEdge(1,3,3);
  g.InsertEdge(1,2,4);
  g.InsertEdge(1,2,5);
  g.InsertEdge(2,1,3);

//  g.InsertNode("Hello");
//  g.InsertNode("how");
//  g.InsertNode("are");
//  g.InsertNode("you");

//  gdwg::Graph<std::string, double> g;
//  g.InsertNode("Hello");
//  g.InsertNode("how");
//  g.InsertNode("are");
////  g.InsertNode("you");
//  g.InsertEdge("Hello", "how", 10);
////  g.InsertEdge("Hello", "how", 10);
//  g.InsertEdge("Hello", "Hello", 7);
////  g.InsertEdge("Hello", "you", 8);
////  g.InsertEdge("Hello", "are", 8);
//  g.InsertEdge("Hello", "are", 9);
//  g.InsertEdge("Hello", "how", 12);

//  g.InsertEdge("Hello", "how", 10);
////  g.InsertEdge("Hello", "how", 10);
//  g.InsertEdge("Hello", "Hello", 7);
////  g.InsertEdge("Hello", "you", 8);
////  g.InsertEdge("Hello", "are", 8);
//  g.InsertEdge("Hello", "are", 9);
//  g.InsertEdge("Hello", "how", 12);
////  g.InsertEdge("Hello", "how", 10);
////  g.InsertEdge("Hello", "how", 11);
//  g.InsertEdge("how", "are", 11);
//  g.InsertEdge("are", "how", 19);
//  g.InsertEdge("how", "are", 12);
//  g.InsertEdge("are", "how", 12);
//  g.InsertEdge("how", "Hello", 13);
//  g.Traverse_Graph();
////  g.DeleteNode("how");
//  std::cout << "\n================================================\n";
////  g.MergeReplace("Hello","are");
//  g.Traverse_Graph();
//  std::cout << "\n================================================\n";


    std::cout << g << '\n';
//  g.erase("are", "how", 12);
//  g.erase("are", "how", 13); // has no this edge
//  g.Traverse_Graph();
//  gdwg::Graph<std::string, double> g2{g};
//  if(g2 == g){
//    std::cout << "g2 == g";
//  }
//  g2.InsertNode("Hey");
//  if(g2 != g){
//    std::cout << "g2 != g";
//  }

//  std::string a1{"are"};
//  std::string a2{"how"};
//  int e = 12;
//  g.erase(g.find(a1, a2, e));
<<<<<<< HEAD
//  auto i  = g.find(a1, a2, e);





=======
  g.find2(a1, a2, e);
//  gdwg::Graph<std::string, double> move_g = std::move(g);
  std::cout << "\n========================Move========================\n";
//  move_g.Traverse_Graph();
  std::cout << "\n========================Move2========================\n";
//  g.Traverse_Graph();
//  gdwg::Graph<std::string, double> assign_g;
//  assign_g = move_g;
//  assign_g.Traverse_Graph();
>>>>>>> d85d147bb50b4b72b89caa8e3a0ee07fd907f858


//  for (const auto& [from, weight] : g) {
//    std::cout << from << " -> " << " (weight " << weight << ")\n";
//  }

////  g.Clear();
//  g.InsertNode("Hello1");
//  g.InsertNode("how1");
//  g.InsertNode("are1");
//  g.InsertEdge("Hello1", "are1", 8);
//  g.InsertEdge("Hello1", "are1", 9);
//  g.InsertEdge("Hello1", "are1", 9);
//  g.InsertEdge("Hello1", "how1", 8);
////  g.Traverse_Graph();
////  std::cout << g.IsNode("Hello") << "\n";
////  std::cout << g.IsNode("Hello1") << "\n";
//  g.IsConnected("are1", "Hello1");
//  g.InsertNode("aa");
//  std::vector<std::string> vector = g.GetNodes();
//  for (const auto& item : vector) {
//    std::cout << "Node: " << item << "\n";
//  }
//
  std::vector<std::string> vector2 = g.GetConnected("Hello");
  for (const auto& item : vector2) {
    std::cout << "Connected: " << item << "\n";
  }
  g.Iterator_Traverse();
//  std::cout << g;

  std::cout << "\n================================================\n";
  std::vector<int> myvector;
  for (int i=7; i<10; i++) myvector.push_back(i);
  myvector.push_back(155);

  typedef std::vector<int>::iterator iter_type;

  std::reverse_iterator<iter_type> rev_begin{myvector.end()};
  std::reverse_iterator<iter_type> rev_end{myvector.begin()};

  std::reverse_iterator<iter_type> rev_iterator = rev_begin;
  std::cout << *rev_iterator << '\n';
  std::cout << *rev_end << '\n';
  std::cout << *myvector.end() << '\n';
  std::cout << *myvector.begin() << '\n';
  while ( rev_iterator != rev_end )
    std::cout << *rev_iterator++ << ' ';
  std::cout << '\n';

  while ( rev_iterator != rev_begin )
    std::cout << *(--rev_iterator) << ' ';
  std::cout << '\n';




//////
//  std::vector<double> vector3 = g.GetWeights("Hello", "how");
//  for (const auto& item : vector3) {
//    std::cout << "Weight: " << item << "\n";
//  }

//  std::vector<std::string> v{"Hello", "how", "are", "you", "Hello", "Hello", "Hello"};
//  gdwg::Graph<std::string, double> a{v.begin(),v.end()};
//  b.InsertEdge("Hello", "how", 10);
//  b.InsertEdge("Hello", "how", 8);
//  b.InsertEdge("Hello", "are", 9);
//  b.InsertEdge("Hello", "how", 7);
//  b.InsertEdge("Hello", "how", 8);
//  b.InsertEdge("how", "Hello", 8);
//  b.Traverse_Graph();

//
//  std::string s1{"Hello"};
//  std::string s2{"how"};
//  std::string s3{"are"};
//  auto e1 = std::make_tuple(s1, s2, 5.4);
//  auto e2 = std::make_tuple(s2, s3, 7.6);
//  auto e = std::vector<std::tuple<std::string, std::string, double>>{e1, e2};
//  gdwg::Graph<std::string, double> b{e.begin(), e.end()};
//  b.Traverse_Graph();
//  std::cout << "-----------------------------\n";
//  gdwg::Graph<std::string, double> copy_b(b);
//  copy_b.Traverse_Graph();

//  gdwg::Graph<char, std::string> c{'a', 'b', 'x', 'y'};
//  c.Traverse_Graph();
//  gdwg::Graph<char, std::string> copy_c{c};
//  copy.Traverse_Graph();


//  std::shared_ptr<std::string> sharedPtr = std::make_shared<std::string>("123");
//  std::cout << "sharedPtr value: " << *sharedPtr.get() << "\n";
//  std::cout << "sharedPtr.size: " << sharedPtr.use_count() << "\n";
//  std::shared_ptr<std::string> sharedPtr2 = sharedPtr;
//  std::cout << "sharedPtr.size: " << sharedPtr2.use_count() << "\n";
//  std::shared_ptr<std::string> sharedPtr3 = sharedPtr;
//  std::cout << "sharedPtr.size: " << sharedPtr3.use_count() << "\n";
//  std::weak_ptr<std::string> weak_ptr = sharedPtr;
//  std::weak_ptr<std::string> weak_ptr2 = weak_ptr;
//  std::cout << "weak_ptr.size*****: " << weak_ptr.use_count() << "\n";
//  std::cout << "sharedPtr2.size: " << weak_ptr2.use_count() << "\n";
//  std::shared_ptr<std::string> sharedPtr4;
//  sharedPtr4 = weak_ptr.lock();
//  std::weak_ptr<std::string> weak_ptr3 = weak_ptr.lock();
//  std::cout << "sharedPtr.size++++++++: " << sharedPtr4.use_count() << "\n";
//  sharedPtr4.reset();
//  sharedPtr.reset();
//  std::cout << "sharedPtr.size: " << sharedPtr2.use_count() << "\n";
//  std::cout << "sharedPtr.size: " << sharedPtr2.get() << "\n";
//  sharedPtr2.reset();
//  std::cout << "weak_ptr.size: " << weak_ptr.use_count() << "\n";
//  std::cout << "weak_ptr.lock().get(): " << weak_ptr.lock() << "\n";
//  std::cout << "weak_ptr.expired(): " << weak_ptr.expired() << "\n";
//  sharedPtr3.reset();
//  std::cout << "weak_ptr.val: " << weak_ptr.lock().get() << "\n";
//  std::cout << "weak_ptr.expired(): " << weak_ptr.expired() << "\n";
//  std::string string = "321";
//  std::unique_ptr<std::string> uniquePtr = std::make_unique<std::string>(string);
//  std::cout << "uniquePtr.get(): " << *(uniquePtr.get()) << "\n";
}

