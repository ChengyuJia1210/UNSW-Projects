
/*

  == Explanation and rational of testing ==

    1. test all the constructors. use isnode method to determine if the nodes already in the graph;
  if there are edges , use getconnected method to test if edges have been inserted to the right
  positions; for copy constructor and move constructor, only difference is to test if the old graph
  still contains the nodes and edges or not; copy and move assignment follow the similar approach.
  2. for the methods testing, try to use the examples of multiple edges connected to different nodes
  with both directions; it can generally come up with more complete testing to cover most of the
  cases.
  3. for merge replace, implement some other methods such as getweights to test in order to list out
  all the edges connected and the corresponding weights
  4. for const iterator, test out all the end iterators; for the reverse iterators simply just
  follow the similar approach like iterator

*/

#include <string>
#include "assignments/dg/graph.h"
#include "catch.h"


// constructors tests
// #######################################################################################
SCENARIO("initialize a graph with a bunch of nodes") {
  GIVEN("vector of int type nodes") {
    std::vector<int> v{2, 33, 4, 1, 9};
    WHEN("take the begin and end of the vector to initialize the graph") {
      gdwg::Graph<int, double> g{v.begin(), v.end()};
      THEN("the graph should be constructed with all the nodes from vector") {
        REQUIRE(g.IsNode(2) == true);
        REQUIRE(g.IsNode(33) == true);
        REQUIRE(g.IsNode(4) == true);
        REQUIRE(g.IsNode(1) == true);
        REQUIRE(g.IsNode(9) == true);
        REQUIRE(g.IsNode(12) == false);
      }
    }
  }
}

SCENARIO("initialize a graph with nodes and edges") {
  GIVEN("some nodes with string type and edges based on tuple of nodes") {
    std::string s1{"a"};
    std::string s2{"b"};
    std::string s3{"c"};
    auto e1 = std::make_tuple(s1, s2, 3);
    auto e2 = std::make_tuple(s2, s3, 3);
    auto e = std::vector<std::tuple<std::string, std::string, double>>{e1, e2};
    WHEN("take the begin and end iterator of the vector e") {
      gdwg::Graph<std::string, double> g{e.begin(), e.end()};
      THEN("nodes should be in the graph") {
        REQUIRE(g.IsNode("a") == true);
        REQUIRE(g.IsNode("b") == true);
        REQUIRE(g.IsNode("c") == true);
      }
      THEN("certain edges should be connected, s1 and s3 should not be connected") {
        REQUIRE(g.IsConnected(s1, s2) == true);
        REQUIRE(g.IsConnected(s2, s3) == true);
        REQUIRE(g.IsConnected(s1, s3) == false);
      }
    }
  }
}

SCENARIO("use initialize list to construct a graph") {
  GIVEN("a graph with some nodes") {
    gdwg::Graph<char, std::string> g{'a', 'b', 'x', 'y'};
    THEN("all nodes should be in the graph") {
      REQUIRE(g.IsNode('a') == true);
      REQUIRE(g.IsNode('b') == true);
      REQUIRE(g.IsNode('x') == true);
      REQUIRE(g.IsNode('y') == true);
      REQUIRE(g.IsNode('d') == false);
    }
  }
}

// copy constructor
SCENARIO("copy constructor to copy a graph from existed graph") {
  GIVEN("a initialize list graph ") {
    gdwg::Graph<char, std::string> g{'a'};
    WHEN("construct a graph by copying g") {
      gdwg::Graph<char, std::string> gCopy{g};
      THEN("node a is also in the graph gCopy") { REQUIRE(gCopy.IsNode('a') == true); }
    }
  }
}

// move constructor

SCENARIO("move constructor to move a graph and delete the original graph") {
  GIVEN("a graph with nodes and edges") {
    gdwg::Graph<std::string, double> g;
    g.InsertNode("a");
    g.InsertNode("b");
    g.InsertEdge("a", "b", 1);
    g.InsertEdge("b", "a", 2);

    WHEN("move constructor is executed") {
      gdwg::Graph<std::string, double> g2{std::move(g)};
      THEN(" new graph will contains all the nodes old graph has") {
        REQUIRE(g2.IsNode("a") == true);
        REQUIRE(g2.IsNode("b") == true);
      }
      THEN("new graph will contain all the edges old graph has") {
        REQUIRE(g2.IsConnected("a", "b") == true);
        REQUIRE(g2.IsConnected("b", "a") == true);
      }
    }
  }
}

// operations testing
// #######################################################################################

// copy assignment
SCENARIO("copy assignment overload ") {
  GIVEN("a graph with nodes and edges") {
    gdwg::Graph<std::string, double> g1;
    g1.InsertNode("a");
    g1.InsertNode("b");
    g1.InsertEdge("a", "b", 1);
    g1.InsertEdge("b", "a", 2);
    gdwg::Graph<std::string, double> g2;
    WHEN("g1 is copied to g2") {
      g2 = g1;
      THEN("g2 should have the same nodes edges as g1") {
        REQUIRE(g2.IsNode("a") == true);
        REQUIRE(g2.IsNode("b") == true);
        REQUIRE(g2.IsConnected("a", "b") == true);
        REQUIRE(g2.IsConnected("b", "a") == true);
      }
      THEN(" g1 still remains the same nodes and edges without being deleted") {
        REQUIRE(g1.IsNode("a") == true);
        REQUIRE(g1.IsNode("b") == true);
        REQUIRE(g1.IsConnected("a", "b") == true);
        REQUIRE(g1.IsConnected("b", "a") == true);
      }
    }
  }
}

// move assignment
SCENARIO("original gragh being moved assigned to a new graph") {
  GIVEN("a graph with nodes and edges") {
    gdwg::Graph<std::string, double> g1;
    g1.InsertNode("a");
    g1.InsertNode("b");
    g1.InsertEdge("a", "b", 1);
    g1.InsertEdge("b", "a", 2);
    gdwg::Graph<std::string, double> g2;
    WHEN("g1 is copied to g2") {
      g2 = std::move(g1);
      THEN("g2 should have the same nodes edges as g1") {
        REQUIRE(g2.IsNode("a") == true);
        REQUIRE(g2.IsNode("b") == true);
        REQUIRE(g2.IsConnected("a", "b") == true);
        REQUIRE(g2.IsConnected("b", "a") == true);
      }
      THEN(" g1 still remains the same nodes and edges without being deleted") {
        REQUIRE(g1.IsNode("a") == false);
        REQUIRE(g1.IsNode("b") == false);
      }
    }
  }
}

// Methods testing
// #######################################################################################

SCENARIO("insert a node to a existed graph") {
  GIVEN("a node with string type and a graph with some nodes in it ") {
    std::vector<std::string> v{"Hello", "how", "are", "you"};
    gdwg::Graph<std::string, double> g{v.begin(), v.end()};
    std::string s{"la"};
    std::string existed{"Hello"};
    WHEN("if insert a node that  is not existed in the graph") {
      THEN("node should be inserted and return true ") { REQUIRE(g.InsertNode(s) == true); }
    }
    WHEN("the node is already existed in the graph") {
      THEN(" node should not be inserted and return a false") {
        REQUIRE(g.InsertNode(existed) == false);
      }
    }
  }
}

SCENARIO("insert an edge to a graph") {
  GIVEN("an existed graph with some nodes and edges ") {
    std::string s1{"a"};
    std::string s2{"b"};
    std::string s3{"c"};
    auto e1 = std::make_tuple(s1, s2, 3);
    auto e2 = std::make_tuple(s2, s3, 3);
    auto e = std::vector<std::tuple<std::string, std::string, double>>{e1, e2};
    gdwg::Graph<std::string, double> g{e.begin(), e.end()};
    WHEN(
        "the new edge is not in the graph, the edge should be added to the graph and return true") {
      REQUIRE(g.InsertEdge("a", "c", 4) == true);
    }
    WHEN("the edge is existed with same nodes and weight , the graph should not accept insertion "
         "and return false") {
      REQUIRE(g.InsertEdge("a", "b", 3) == false);
    }
    WHEN("the nodes are the same but with different weight should treat as new edge and return "
         "true") {
      REQUIRE(g.InsertEdge("a", "b", 4) == true);
    }
    WHEN("either src node or dest node is not in the graph") {
      REQUIRE_THROWS_WITH(
          g.InsertEdge("d", "e", 2),
          "Cannot call Graph::InsertEdge when either src or dst node does not exist");
    }
  }
}

SCENARIO("delete the existed node from the graph") {
  GIVEN("a graph with some nodes") {
    std::vector<std::string> v{"Hello", "how", "are", "you"};
    gdwg::Graph<std::string, double> g{v.begin(), v.end()};
    WHEN("a given node that exists in the graph ") {
      std::string d{"Hello"};
      THEN("the node should be deleted from the graph and return true") {
        REQUIRE(g.DeleteNode(d) == true);
        REQUIRE(g.IsNode(d) == false);
      }
    }
    WHEN("a given node does not exist in the graph") {
      std::string d{"Nope"};
      THEN("it should do nothing to the graph and return false") {
        REQUIRE(g.DeleteNode(d) == false);
      }
    }
  }
}

SCENARIO("replace a node in the graph with a given node") {
  GIVEN("a node that will be replaced and a graph with some ndoes") {
    std::vector<std::string> v{"Hello", "how", "are", "you"};
    gdwg::Graph<std::string, double> g{v.begin(), v.end()};
    std::string r{"replace"};
    WHEN("the node that will be replaced is not in the graph") {
      THEN("it should throw a runtime error") {
        REQUIRE_THROWS_WITH(g.Replace("Hi", r),
                            "Cannot call Graph::Replace on a node that doesn't exist");
      }
    }
    WHEN("old data exists in the graph and new data is not in the graph") {
      THEN("new node should replace the old node and return true") {
        REQUIRE(g.Replace("how", r) == true);
        REQUIRE(g.IsNode(r) == true);
      }
    }

    WHEN("new data already exists in the graph") {
      THEN("should let the graph remain the same and return false") {
        REQUIRE(g.Replace("how", "Hello") == false);
        REQUIRE(g.IsNode("how") == true);
      }
    }
  }
}

SCENARIO("replace the node with an existed node and merge the edges ") {
  GIVEN("a graph with some nodes with edges connected") {
    std::string s1{"a"};
    std::string s2{"b"};
    std::string s3{"c"};
    std::string s4{"d"};
    auto e1 = std::make_tuple(s1, s2, 3);
    auto e2 = std::make_tuple(s2, s3, 3);
    auto e3 = std::make_tuple(s1, s3, 5);
    auto e4 = std::make_tuple(s3, s2, 2);
    auto e5 = std::make_tuple(s1, s4, 5);
    auto e = std::vector<std::tuple<std::string, std::string, double>>{e1, e2, e3, e4, e5};
    gdwg::Graph<std::string, double> g{e.begin(), e.end()};
    WHEN("either node is not found in the graph") {
      THEN("throw a runtime error") {
        REQUIRE_THROWS_WITH(
            g.MergeReplace("d", "e"),
            "Cannot call Graph::MergeReplace on old or new data if they don't exist in the graph");
      }
    }
    WHEN("old data and new data exist in the graph") {
      THEN("new data node will replace the old data node and apart from duplicate edges all "
           "incoming and out coming edges of old data become new data node's") {
        g.MergeReplace("c", "d");
        REQUIRE(g.IsNode("c") == false);  // node c is replaced it is gone
        //                REQUIRE(g.GetWeights("a","d") == std::vector<double> {5}); // only one
        //                weight in the set means no duplicates
        REQUIRE(g.IsConnected("b", "d") == true);  // after merge b and d should connect together
      }
    }
  }
}

SCENARIO("clear all the nodes in the graph") {
  GIVEN("a graph with some nodes and edges") {
    std::string s1{"a"};
    std::string s2{"b"};
    std::string s3{"c"};
    auto e1 = std::make_tuple(s1, s2, 3);
    auto e2 = std::make_tuple(s2, s3, 3);
    auto e3 = std::make_tuple(s1, s3, 5);
    auto e = std::vector<std::tuple<std::string, std::string, double>>{e1, e2, e3};
    gdwg::Graph<std::string, double> g{e.begin(), e.end()};
    WHEN("execute clear method") {
      g.Clear();
      THEN("all nodes and edges should be cleared ") {
        REQUIRE(g.GetNodes() == std::vector<std::string>{});
      }
      THEN("new nodes and edges can be added to the graph again") {
        g.InsertNode("new");
        g.InsertNode("new2");
        g.InsertEdge("new", "new2", 2);
        REQUIRE(g.IsNode("new") == true);
        REQUIRE(g.IsNode("new2") == true);
        REQUIRE(g.IsConnected("new", "new2") == true);
      }
    }
  }
}

SCENARIO("Get all the nodes from the graph in increasing order") {
  GIVEN("a graph with some nodes") {
    std::vector<double> v{2, 3, 1};
    gdwg::Graph<double, double> g{v.begin(), v.end()};
    THEN("when execute method getnodes") { REQUIRE(g.GetNodes() == std::vector<double>{1, 2, 3}); }
  }
}

SCENARIO("return all nodes connected from src node") {
  GIVEN("a graph with some ndoes") {
    std::string s1{"a"};
    std::string s2{"b"};
    std::string s3{"c"};
    std::string s4{"d"};
    auto e1 = std::make_tuple(s1, s2, 3);
    auto e2 = std::make_tuple(s2, s3, 3);
    auto e3 = std::make_tuple(s1, s3, 5);
    auto e4 = std::make_tuple(s3, s2, 2);
    auto e5 = std::make_tuple(s1, s4, 5);
    auto e = std::vector<std::tuple<std::string, std::string, double>>{e1, e2, e3, e4, e5};
    gdwg::Graph<std::string, double> g{e.begin(), e.end()};
    WHEN("src node is not in the graph") {
      THEN("should throw a runtime error") {
        REQUIRE_THROWS_WITH(g.GetConnected("e"),
                            "Cannot call Graph::GetConnected if src doesn't exist in the graph");
      }
    }
    WHEN("src node is given and has edges connected to it ") {
      THEN("return all the nodes as vector where connected to the src node") {
        std::vector<std::string> nodes_v = {s2, s3, s4};
        REQUIRE(g.GetConnected(s1) == nodes_v);
      }
    }
    WHEN("src node is not connected any other nodes in the graph") {
      std::vector<std::string> empty = {};
      REQUIRE(g.GetConnected(s4) == empty);
    }
  }
}

SCENARIO("get all the weights between two nodes") {
  GIVEN("a graph with some nodes and edges") {
    std::string s1{"a"};
    std::string s2{"b"};
    std::string s3{"c"};
    std::string s4{"d"};
    auto e1 = std::make_tuple(s1, s2, 3);
    auto e2 = std::make_tuple(s2, s3, 3);
    auto e3 = std::make_tuple(s2, s3, 5);
    auto e4 = std::make_tuple(s3, s2, 2);
    auto e5 = std::make_tuple(s1, s4, 5);
    auto e = std::vector<std::tuple<std::string, std::string, double>>{e1, e2, e3, e4, e5};
    gdwg::Graph<std::string, double> g{e.begin(), e.end()};
    WHEN("neither src nor dst node is in the graph") {
      THEN("throw a runtime error") {
        REQUIRE_THROWS_WITH(
            g.GetWeights("e", "f"),
            "Cannot call Graph::GetWeights if src or dst node don't exist in the graph");
      }
    }
    WHEN("given src and dst nodes are in the graph") {
      THEN("return the a vector contains all the weights between two nodes in increasing order") {
        std::vector<double> w = {3, 5};
        REQUIRE(g.GetWeights(s2, s3) == w);
      }
    }
  }
}

SCENARIO("delete edge with specific weight between src and dst nodes") {
  GIVEN("a graph with some nodes and edges ") {
    std::string s1{"a"};
    std::string s2{"b"};
    std::string s3{"c"};
    std::string s4{"d"};
    auto e1 = std::make_tuple(s1, s2, 3);
    auto e2 = std::make_tuple(s2, s3, 3);
    auto e3 = std::make_tuple(s1, s3, 5);
    auto e4 = std::make_tuple(s3, s2, 2);
    auto e5 = std::make_tuple(s1, s4, 5);
    auto e = std::vector<std::tuple<std::string, std::string, double>>{e1, e2, e3, e4, e5};
    gdwg::Graph<std::string, double> g{e.begin(), e.end()};
    WHEN("edge exists ") {
      THEN("return true to show has been removed") { REQUIRE(g.erase(s1, s2, 3) == true); }
    }
    WHEN("edge with specific weight does not exist between src and dst node") {
      THEN("return false ") { REQUIRE(g.erase(s1, s2, 12) == false); }
    }
  }
}

// const iterator testing
// #####################################################################

SCENARIO("find the iterator of specific edge") {
  GIVEN(" a graph with some nodes and edges") {
    std::string s1{"a"};
    std::string s2{"b"};
    std::string s3{"c"};
    std::string s4{"d"};
    auto e1 = std::make_tuple(s1, s2, 3);
    auto e2 = std::make_tuple(s2, s3, 3);
    auto e3 = std::make_tuple(s1, s3, 5);
    auto e4 = std::make_tuple(s3, s2, 2);
    auto e5 = std::make_tuple(s1, s4, 5);
    auto e = std::vector<std::tuple<std::string, std::string, double>>{e1, e2, e3, e4, e5};
    gdwg::Graph<std::string, double> g{e.begin(), e.end()};
    WHEN("the edge exists in the graph") {
      THEN("return iterator of the found edge") { REQUIRE(*(g.find(s1, s3, 5)) == e3); }
    }
    WHEN(" current iterator is the end ") {
      THEN("return the end iterator cend") {
        for (auto it = g.cbegin(); it != g.cend(); ++it) {
          if (it == g.cend()) {
            const auto & [ from, to, weight ] = *it;
            REQUIRE(from == s1);
            REQUIRE(to == s4);
            REQUIRE(weight == 5);
          }
        }
      }
    }
  }
}

SCENARIO("remove the location where iterator points to ") {
  GIVEN("a graph with nodes and edges ") {
    gdwg::Graph<int, double> g;
    g.InsertNode(1);
    g.InsertNode(2);
    g.InsertNode(3);
    g.InsertNode(4);
    g.InsertEdge(1, 2, 4);
    g.InsertEdge(1, 3, 2);
    g.InsertEdge(2, 2, 4);
    g.InsertEdge(1, 2, 5);
    g.InsertEdge(3, 2, 3);
    g.InsertEdge(4, 1, 2);
    // 1,2,4 ; 1,2,5 ; 1,3,2 ; 2,2,4 ; 3,2,3 ; 4,1,2
    WHEN("iterator position is removed ") {
      THEN("return the next iterator after the curr iterator") {
        auto it = g.find(1, 2, 5);
        auto it2 = g.find(1, 3, 2);
        REQUIRE(g.erase(it) == it2);
      }
    }
    WHEN("iterator postion is at the end ") {
      THEN("return the end element without going to the next element") {
        for (auto it = g.cbegin(); it != g.cend(); ++it) {
          if (it == g.cend()) {
            const auto & [ from, to, weight ] = *it;
            REQUIRE(from == 4);
            REQUIRE(to == 1);
            REQUIRE(weight == 2);
          }
        }
      }
    }
  }
}

// cbegin and cend
SCENARIO("test the iterator position for cbegin and cend") {
  GIVEN("a graph with nodes and edges ") {
    gdwg::Graph<int, double> g;
    g.InsertNode(1);
    g.InsertNode(2);
    g.InsertNode(3);
    g.InsertNode(4);
    g.InsertEdge(1, 2, 4);
    g.InsertEdge(1, 3, 2);
    g.InsertEdge(2, 2, 4);
    g.InsertEdge(1, 2, 5);
    g.InsertEdge(3, 2, 3);
    g.InsertEdge(4, 1, 2);
    // 1,2,4 ; 1,2,5 ; 1,3,2 ; 2,2,4 ; 3,2,3 ; 4,1,2

    for (auto it = g.cbegin(); it != g.cend(); ++it) {
      WHEN("call cbegin") {
        if (it == g.cbegin()) {
          const auto & [ from, to, weight ] = *it;
          THEN("return the first iterator 1, 2, 4") {
            REQUIRE(from == 1);
            REQUIRE(to == 2);
            REQUIRE(weight == 4);
          }
        }
      }
      WHEN("call cend") {
        if (it == g.cend()) {
          const auto & [ from, to, weight ] = *it;
          THEN("return the last iterator 4, 1, 2") {
            REQUIRE(from == 4);
            REQUIRE(to == 1);
            REQUIRE(weight == 2);
          }
        }
      }
    }
  }
}

// crbegin and crend
SCENARIO("test the reverse iterator position for crbegin and crend") {
  GIVEN("a graph with nodes and edges ") {
    gdwg::Graph<int, double> g;
    g.InsertNode(1);
    g.InsertNode(2);
    g.InsertNode(3);
    g.InsertNode(4);
    g.InsertEdge(1, 2, 4);
    g.InsertEdge(1, 3, 2);
    g.InsertEdge(2, 2, 4);
    g.InsertEdge(1, 2, 5);
    g.InsertEdge(3, 2, 3);
    g.InsertEdge(4, 1, 2);
    // 1,2,4 ; 1,2,5 ; 1,3,2 ; 2,2,4 ; 3,2,3 ; 4,1,2 reverse

    for (auto it = g.crbegin(); it != g.crend(); ++it) {
      WHEN("call cbegin") {
        if (it == g.crbegin()) {
          const auto & [ from, to, weight ] = *it;
          THEN("return the last iterator 4, 1, 2") {
            REQUIRE(from == 4);
            REQUIRE(to == 1);
            REQUIRE(weight == 2);
          }
        }
      }
      WHEN("call cend") {
        if (it == g.crend()) {
          const auto & [ from, to, weight ] = *it;
          THEN("return the last iterator 1, 2, 4") {
            REQUIRE(from == 1);
            REQUIRE(to == 2);
            REQUIRE(weight == 4);
          }
        }
      }
    }
  }
}

// begin and end
SCENARIO("test the iterator position begin and end") {
  GIVEN("a graph with nodes and edges ") {
    gdwg::Graph<int, double> g;
    g.InsertNode(1);
    g.InsertNode(2);
    g.InsertNode(3);
    g.InsertNode(4);
    g.InsertEdge(1, 2, 4);
    g.InsertEdge(1, 3, 2);
    g.InsertEdge(2, 2, 4);
    g.InsertEdge(1, 2, 5);
    g.InsertEdge(3, 2, 3);
    g.InsertEdge(4, 1, 2);
    // 1,2,4 ; 1,2,5 ; 1,3,2 ; 2,2,4 ; 3,2,3 ; 4,1,2

    for (auto it = g.begin(); it != g.end(); ++it) {
      WHEN("call cbegin") {
        if (it == g.begin()) {
          const auto & [ from, to, weight ] = *it;
          THEN("return the first iterator 1, 2, 4") {
            REQUIRE(from == 1);
            REQUIRE(to == 2);
            REQUIRE(weight == 4);
          }
        }
      }
      WHEN("call cend") {
        if (it == g.end()) {
          const auto & [ from, to, weight ] = *it;
          THEN("return the last iterator 4, 1, 2") {
            REQUIRE(from == 4);
            REQUIRE(to == 1);
            REQUIRE(weight == 2);
          }
        }
      }
    }
  }
}

// rbegin and rend
SCENARIO("test the reverse iterator position rbegin and rend") {
  GIVEN("a graph with nodes and edges ") {
    gdwg::Graph<int, double> g;
    g.InsertNode(1);
    g.InsertNode(2);
    g.InsertNode(3);
    g.InsertNode(4);
    g.InsertEdge(1, 2, 4);
    g.InsertEdge(1, 3, 2);
    g.InsertEdge(2, 2, 4);
    g.InsertEdge(1, 2, 5);
    g.InsertEdge(3, 2, 3);
    g.InsertEdge(4, 1, 2);
    // 1,2,4 ; 1,2,5 ; 1,3,2 ; 2,2,4 ; 3,2,3 ; 4,1,2 reverse
    auto b = g.rbegin();
    auto e = g.rend();
    for (auto it = b; it != e; ++it) {
      WHEN("call rbegin") {
        if (it == b) {
          const auto & [ from, to, weight ] = *it;
          THEN("return the last iterator 4, 1, 2") {
            REQUIRE(from == 4);
            REQUIRE(to == 1);
            REQUIRE(weight == 2);
          }
        }
      }
      WHEN("call rend") {
        if (it == e) {
          const auto & [ from, to, weight ] = *it;
          THEN("return the last iterator 1, 2, 4") {
            REQUIRE(from == 1);
            REQUIRE(to == 2);
            REQUIRE(weight == 4);
          }
        }
      }
    }
  }
}
