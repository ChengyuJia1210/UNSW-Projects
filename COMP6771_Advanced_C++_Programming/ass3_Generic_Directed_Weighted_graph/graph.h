#ifndef ASSIGNMENTS_DG_GRAPH_H_
#define ASSIGNMENTS_DG_GRAPH_H_

#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <tuple>
#include <typeinfo>
#include <unordered_set>
#include <utility>
#include <vector>

namespace gdwg {
template <typename N, typename E>
class Graph;
}  // namespace gdwg

template <typename N, typename E>
class gdwg::Graph {
 public:
  class Node;
  class Edge;
  using Node_weak_ptr = std::weak_ptr<Node>;
  using Weight_share_ptr = std::shared_ptr<E>;

  // =======================> Compare Helper Structs <=======================
  // this struct will be used when the nodes insert into map to make sure
  // that the inertion is according to the node value increasing order
  struct NodeCompare {
    bool operator()(const Node_weak_ptr& a, const Node_weak_ptr& b) const {
      return a.lock().get()->node_val < b.lock().get()->node_val;
    }
  };

  // this struct jused to compare the node value
  // when calling the GetConnected() function
  // to make sure the nodes are ordered
  struct Node_val_Compare {
    bool operator()(const N& a, const N& b) const { return a < b; }
  };

  // this struct jused to compare the edge weight
  // when calling the GetWeights() function
  // to make sure the weights are ordered
  struct Weight_val_Compare {
    bool operator()(const E& a, const E& b) const { return a < b; }
  };

  // this struct will be used when the nodes insert into map nest set to make sure
  // that the inertion is according to the dst node value by increasing order first,
  // if it's' the same dst node, then compare the weight by increasing order.
  struct EdgeCompare {
    // make edge class comparable.
    // first compare node_val if all equal compare weight.
    bool operator()(const std::shared_ptr<Edge>& a, const std::shared_ptr<Edge>& b) const {
      if (!is_node_equal(a.get()->node.lock().get()->node_val,
                         b.get()->node.lock().get()->node_val)) {
        return a.get()->node.lock().get()->node_val < b.get()->node.lock().get()->node_val;
      } else {
        return *(a.get()->weight.get()) < *(b.get()->weight.get());
      }
    }
  };

  // =======================> Iterator Nest Classes <=======================
  class const_iterator {
   public:
    using iterator_category = std::bidirectional_iterator_tag;
    using val_type = std::tuple<N, N, E>;
    using reference = std::tuple<const N&, const N&, const E&>;
    using pointer = N*;
    using difference_type = int;

    reference operator*() const {
      return {outer_->first.get()->node_val, (*inner_).get()->node.lock().get()->node_val,
              *(*inner_).get()->weight};
    }

    const_iterator operator++(int) {
      auto copy{*this};
      ++(*this);
      return copy;
    }

    const_iterator& operator++() {
      ++inner_;
      // if the inner_ iterator equal to set's end() iterator,
      // then try to increase the outer map's iterator
      if (inner_ == set_end_) {
        // after increased the outer_ then check
        // if the outer_ is not equal to map's end() iterator,
        // if it's false, jump out of while loop,
        // else then check its second element, which is set, empty or not
        // if empty , jump out of while loop,
        // else increase iterator outer_
        do {
          ++outer_;
        } while (outer_ != map_end_ && outer_->second.empty());
        // if the outer_ is not equal to map's end() iterator,
        // then try to assign this map's this current iterator's set begin() and end() iterator
        if (outer_ != map_end_) {
          inner_ = outer_->second.begin();
          set_end_ = outer_->second.end();
        }
      }
      return *this;
    }

    friend bool operator==(const const_iterator& lhs, const const_iterator& rhs) {
      // need to check the map_end_ because comparison of default constructed iterators is
      // undefined.
      return lhs.outer_ == rhs.outer_ && (lhs.outer_ == lhs.map_end_ || lhs.inner_ == rhs.inner_);
    }
    friend bool operator!=(const const_iterator& lhs, const const_iterator& rhs) {
      return !(lhs == rhs);
    }

   private:
    typename std::map<std::shared_ptr<Node>,
                      std::set<std::shared_ptr<Edge>, EdgeCompare>,
                      NodeCompare>::iterator outer_;
    const typename std::map<std::shared_ptr<Node>,
                            std::set<std::shared_ptr<Edge>, EdgeCompare>,
                            NodeCompare>::iterator map_end_;
    typename std::set<std::shared_ptr<Edge>, EdgeCompare>::iterator inner_;
    typename std::set<std::shared_ptr<Edge>, EdgeCompare>::iterator set_end_;

    friend class Graph;
    const_iterator(const decltype(outer_)& outer,
                   const decltype(map_end_)& map_end,
                   const decltype(inner_)& inner,
                   const decltype(set_end_)& set_end)
      : outer_{outer}, map_end_{map_end}, inner_{inner}, set_end_{set_end} {}
  };

  class const_reverse_iterator {
   public:
    using iterator_category = std::bidirectional_iterator_tag;
    using val_type = std::tuple<N, N, E>;
    using reference = std::tuple<const N&, const N&, const E&>;
    using pointer = N*;
    using difference_type = int;

    reference operator*() const {
      return {map_rbegin_->first.get()->node_val, (*set_rbegin_).get()->node.lock().get()->node_val,
              *(*set_rbegin_).get()->weight};
    }

    const_reverse_iterator operator++(int) {
      auto copy{*this};
      ++(*this);
      return copy;
    }

    const_reverse_iterator& operator++() {
      ++set_rbegin_;
      // if the set_rbegin_ iterator equal to set's end() iterator,
      // then try to increase the outer map's iterator
      if (set_rbegin_ == set_rend_) {
        // after increased the outer_ then check
        // if the map_rbegin_ is not equal to map's end() iterator,
        // if it's false, jump out of while loop,
        // else then check its second element, which is set, empty or not
        // if empty , jump out of while loop,
        // else increase iterator map_rbegin_
        do {
          ++map_rbegin_;
        } while (map_rbegin_ != map_rend_ && map_rbegin_->second.empty());
        // if the map_rbegin_ is not equal to map's end() iterator,
        // then try to assign this map's this current iterator's set begin() and end() iterator
        if (map_rbegin_ != map_rend_) {
          set_rbegin_ = map_rbegin_->second.rbegin();
          set_rend_ = map_rbegin_->second.rend();
        }
      }
      return *this;
    }

    friend bool operator==(const const_reverse_iterator& lhs, const const_reverse_iterator& rhs) {
      // We need to check the map_rend_ because comparison of default constructed iterators is
      // undefined.
      return lhs.map_rbegin_ == rhs.map_rbegin_ &&
             (lhs.map_rbegin_ == lhs.map_rend_ || lhs.set_rbegin_ == rhs.set_rbegin_);
    }

    friend bool operator!=(const const_reverse_iterator& lhs, const const_reverse_iterator& rhs) {
      return !(lhs == rhs);
    }

   private:
    // 6pm/05/08, add map's rbegin and rend as well
    typename std::map<std::shared_ptr<Node>,
                      std::set<std::shared_ptr<Edge>, EdgeCompare>,
                      NodeCompare>::reverse_iterator map_rbegin_;
    const typename std::map<std::shared_ptr<Node>,
                            std::set<std::shared_ptr<Edge>, EdgeCompare>,
                            NodeCompare>::reverse_iterator map_rend_;
    typename std::set<std::shared_ptr<Edge>, EdgeCompare>::reverse_iterator set_rbegin_;
    typename std::set<std::shared_ptr<Edge>, EdgeCompare>::reverse_iterator set_rend_;

    friend class Graph;
    // 6pm/05/08, add map's rbegin and rend as well
    const_reverse_iterator(const decltype(map_rbegin_)& map_rbegin,
                           const decltype(map_rend_)& map_rend,
                           const decltype(set_rbegin_)& set_rbegin,
                           const decltype(set_rend_)& set_rend)
      : map_rbegin_{map_rbegin}, map_rend_{map_rend}, set_rbegin_{set_rbegin}, set_rend_{set_rend} {
    }
  };

  const_iterator cbegin();
  const_iterator cend();
  const_iterator begin() { return cbegin(); }
  const_iterator end() { return cend(); }
  const_iterator find(const N&, const N&, const E&);
  const_iterator erase(const_iterator it);
  const_reverse_iterator crbegin();
  const_reverse_iterator crend();
  const_reverse_iterator rbegin() { return crbegin(); }
  const_reverse_iterator rend() { return crend(); }
  const_reverse_iterator find2(const N& src, const N& dst, const E& w);

  // =======================> Constructors <=======================
  // Default Constructor
  Graph() = default;
  Graph(typename std::vector<N>::const_iterator begin, typename std::vector<N>::const_iterator end);
  Graph(typename std::vector<std::tuple<N, N, E>>::const_iterator,
        typename std::vector<std::tuple<N, N, E>>::const_iterator);
  Graph(std::initializer_list<N>);
  explicit Graph(const gdwg::Graph<N, E>&);
  // Move Constructor
  explicit Graph(gdwg::Graph<N, E>&& g) noexcept : graph_{std::move(g.graph_)},
                                                   has_set_value_{g.has_set_value_} {
    g.has_set_value_ = false;
  }

  // =======================> Destructor <=======================
  ~Graph() = default;

  // =======================> Friend Operators <=======================
  friend bool operator==(const gdwg::Graph<N, E>& outer_g, const gdwg::Graph<N, E>& outer_b) {
    // first check graph size, if sizes are different then have no necessary  to check the nodes and
    // edge
    if (outer_b.graph_.size() != outer_g.graph_.size()) {
      return false;
    }
    // set two vectors to store the node values of two graphs
    std::vector<N> inner_vector, outer_vector;
    for (const auto& map_inner : outer_b.graph_) {
      inner_vector.push_back(map_inner.first.get()->node_val);
    }
    for (const auto& map_outer : outer_g.graph_) {
      outer_vector.push_back(map_outer.first.get()->node_val);
    }
    // then compare two vector, if they are equal then compare the edge next
    if (outer_vector != inner_vector)
      return false;
    // cause two graphs have same nodes, so find out the edge then compare the corresponding edges
    for (const auto& item : inner_vector) {
      auto inner_set = outer_b.graph_.find(outer_b.find_node(item))->second;
      auto outer_set = outer_g.graph_.find(outer_g.find_node(item))->second;
      // if the graphs are same, then the second part of the map, which have same key, will have
      // same size
      // otherwise return false
      if (inner_set.size() != outer_set.size()) {
        return false;
      }
      // insert the dst nodes and edges into a map, then use map's conveivence to compare the edges
      std::map<N, E> inner_map_norm, outer_map_norm;
      for (const auto& setIn : inner_set) {
        inner_map_norm.insert(
            std::make_pair(setIn.get()->node.lock().get()->node_val, *(setIn.get()->weight.get())));
      }
      for (const auto& setIn : outer_set) {
        outer_map_norm.insert(
            std::make_pair(setIn.get()->node.lock().get()->node_val, *(setIn.get()->weight.get())));
      }
      typename std::map<N, E>::const_iterator i, j;
      for (i = inner_map_norm.begin(), j = outer_map_norm.begin(); i != inner_map_norm.end();
           ++i, ++j) {
        if (*i != *j) {
          return false;
        }
      }
    }
    return true;
  }

  friend bool operator!=(const gdwg::Graph<N, E>& outer_a, const gdwg::Graph<N, E>& outer_b) {
    return !(outer_a == outer_b);
  }

  friend std::ostream& operator<<(std::ostream& os, const gdwg::Graph<N, E>& g) {
    if (g.graph_.size() == 0) {
      os << "";
      return os;
    }
    for (const auto& item : g.graph_) {
      std::shared_ptr<Node> key_node = std::get<0>(item);
      N src_node_value = key_node.get()->node_val;
      std::set<std::shared_ptr<Edge>, EdgeCompare> set = item.second;
      os << src_node_value << " (\n";
      for (const auto& item1 : set) {
        if (!item1.get()->node.expired()) {
          E weight = *(item1.get()->weight.get());
          Node_weak_ptr dst_node_value = item1.get()->node;
          os << "  " << dst_node_value.lock().get()->node_val << " | " << weight << "\n";
        }
      }
      os << ")\n";
    }
    return os;
  }

  // =======================> operators <=======================
  // Copy Assignment
  gdwg::Graph<N, E>& operator=(const gdwg::Graph<N, E>& graph) {
    Clear();
    // first loop is insert whole nodes into the map
    for (const auto& item : graph.graph_) {
      N new_node_val = item.first.get()->node_val;
      InsertNode(new_node_val);
    }
    // if map's second element set has content then do the loop
    if (graph.has_set_value_) {
      has_set_value_ = true;
      // second loop is insert the dst node and weight into the src nodes' set
      for (const auto& item : graph.graph_) {
        N new_node_val = item.first.get()->node_val;
        auto map_key_src = find_node(new_node_val);
        // insert each dst node and weight into corresponding set
        for (const auto& item2 : item.second) {
          N dst_node_value = item2.get()->node.lock().get()->node_val;
          E weight = *item2.get()->weight.get();
          auto map_key_dst = find_node(dst_node_value);
          Edge edge{map_key_dst, weight};
          graph_.find(map_key_src)->second.insert(std::make_shared<Edge>(edge));
        }
      }
    }
    return *this;
  }

  // Move Assignment
  gdwg::Graph<N, E>& operator=(gdwg::Graph<N, E>&& g) {
    graph_ = std::move(g.graph_);
    has_set_value_ = g.has_set_value_;
    g.has_set_value_ = false;
    return *this;
  }

  // =======================> Methods <=======================
  bool InsertNode(const N& val);
  bool InsertEdge(const N& src, const N& dst, const E& w);
  bool DeleteNode(const N&);
  bool Replace(const N& oldData, const N& newData);
  void MergeReplace(const N& oldData, const N& newData);
  void Clear();
  bool IsNode(const N& val);
  bool IsConnected(const N& src, const N& dst);
  bool erase(const N& src, const N& dst, const E& w);
  std::vector<N> GetNodes();
  std::vector<N> GetConnected(const N& src);
  std::vector<E> GetWeights(const N& src, const N& dst);

  // This function is used to help check node in the graph or not
  // if in the graph, then return the share pointer of this node, return nullptr otherwise
  std::shared_ptr<Node> find_node(N n) const {
    for (const auto& item : graph_) {
      if (n == item.first.get()->node_val) {
        return item.first;
      }
    }
    return nullptr;
  }

  static bool is_node_equal(N& a, N& b) { return a == b; }

  // if the edge exist then return true
  // The purpose of this function is used to avoid insert redundent edge
  bool check_edge_exist(std::shared_ptr<Node> src_node_sp,
                        std::shared_ptr<Node> dst_node_sp,
                        E edge_weight) {
    // according key to find the particular set
    std::set<std::shared_ptr<Edge>, EdgeCompare> set = graph_.find(src_node_sp)->second;
    for (const auto& set_item : set) {
      // if the set's first element(shared_ptr<Node>) is same as the dst_node_sp
      // then validate the weight value is equal or not
      // if equal means this edge is exist.
      if (set_item.get()->node.lock() == dst_node_sp &&
          *(set_item.get()->weight.get()) == edge_weight) {
        return true;
      }
    }
    return false;
  }

 private:
  std::map<std::shared_ptr<Node>, std::set<std::shared_ptr<Edge>, EdgeCompare>, NodeCompare> graph_;
  // this flag can help program to check the graph only have node or have edges as well
  // if has edge then change flag to true, false otherwise
  bool has_set_value_ = false;
};  // class

#include "assignments/dg/graph.tpp"

#endif  // ASSIGNMENTS_DG_GRAPH_H_
