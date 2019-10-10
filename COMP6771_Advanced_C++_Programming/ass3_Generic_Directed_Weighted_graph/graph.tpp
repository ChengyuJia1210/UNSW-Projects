// The node contain the node value, and will be made as shared_ptr
// to insert into a map as a key
template <typename N, typename E>
class gdwg::Graph<N, E>::Node {
 public:
  N node_val;
  Node() = default;
  Node(N n) { node_val = n; }
};

template <typename N, typename E>
// This Edge data structure has dst node's weak_ptr and weight's shared_ptr
// which will be made as shared_ptr to insert into a set
class gdwg::Graph<N, E>::Edge {
 public:
  // 8:30pm/30/08, make the weight as weak_ptr
  // to make sure the input weight has unique memery address
  Weight_share_ptr weight;
  Node_weak_ptr node;
  Edge() = delete;
  Edge(Node_weak_ptr n, E w) {
    weight = std::make_shared<E>(w);
    node = n;
  }
};

template <typename N, typename E>
gdwg::Graph<N, E>::Graph(typename std::vector<N>::const_iterator begin,
                         typename std::vector<N>::const_iterator end) {
  std::vector<N> vector(begin, end);
  for (const auto& item : vector) {
    InsertNode(item);
  }
}

template <typename N, typename E>
gdwg::Graph<N, E>::Graph(typename std::vector<std::tuple<N, N, E>>::const_iterator begin,
                         typename std::vector<std::tuple<N, N, E>>::const_iterator end) {
  std::vector<std::tuple<N, N, E>> vector{begin, end};
  for (const auto& item : vector) {
    InsertNode(std::get<0>(item));
    InsertNode(std::get<1>(item));
  }
  for (const auto& item1 : vector) {
    InsertEdge(std::get<0>(item1), std::get<1>(item1), std::get<2>(item1));
  }
}

template <typename N, typename E>
gdwg::Graph<N, E>::Graph(std::initializer_list<N> initializer_list) {
  for (const auto& item : initializer_list) {
    InsertNode(item);
  }
}

// Copy Constructor
template <typename N, typename E>
gdwg::Graph<N, E>::Graph(const gdwg::Graph<N, E>& graph) {
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
}

// Insert node into the graph, if the graph's size changed
// which means insert a new node successfully then return ture,
// otherwise the graph has this node aready then return flase.
template <typename N, typename E>
bool gdwg::Graph<N, E>::InsertNode(const N& val) {
  size_t beforeInsertSize = graph_.size();
  std::set<std::shared_ptr<Edge>, EdgeCompare> set;
  graph_.insert(std::make_pair(std::make_shared<Node>(Node{val}), set));
  size_t afterInsertSize = graph_.size();
  return beforeInsertSize < afterInsertSize ? true : false;
}

template <typename N, typename E>
bool gdwg::Graph<N, E>::InsertEdge(const N& src, const N& dst, const E& w) {
  auto map_key_src = find_node(src);  // return empty means map has no such node
  auto map_key_dst = find_node(dst);
  if (map_key_src == nullptr || map_key_dst == nullptr) {
    throw std::runtime_error{
        "Cannot call Graph::InsertEdge when either src or dst node does not exist"};
  }
  // if edge exist then return false
  if (check_edge_exist(map_key_src, map_key_dst, w)) {
    return false;
  }
  // make dst node and weight as an Edge structure then insert into corresponding src node's set
  Edge edge{map_key_dst, w};
  graph_.find(map_key_src)->second.insert(std::make_shared<Edge>(edge));
  has_set_value_ = true;  // insert edge so the map's second value must not empty
  return true;
}

// If the node no(or the graph is empty) in the graph
// then return false, otherwise erase that node and return true
template <typename N, typename E>
bool gdwg::Graph<N, E>::DeleteNode(const N& n) {
  std::shared_ptr<Node> sp_node = find_node(n);
  if (sp_node != nullptr) {
    graph_.erase(sp_node);
    return true;
  }
  if (graph_.size() == 0) {
    has_set_value_ = false;
  }
  return false;
}

// clear map, simple
template <typename N, typename E>
void gdwg::Graph<N, E>::Clear() {
  graph_.clear();
  has_set_value_ = false;
}

// Try to find that node in the graph,
// if the return is a nullptr,
// means graph has no such node, return false
// otherwise return true.
template <typename N, typename E>
bool gdwg::Graph<N, E>::IsNode(const N& val) {
  std::shared_ptr<Node> sp_node = find_node(val);
  if (sp_node != nullptr) {
    return true;
  }
  return false;
}

// First make sure two nodes are existing in the graph
// then according to src node to find it's corresponding set
// then fetch the dst node, if the dst node in the set return true,
// return false otherwise
template <typename N, typename E>
bool gdwg::Graph<N, E>::IsConnected(const N& src, const N& dst) {
  auto sp_src_node = find_node(src);
  auto sp_dst_node = find_node(dst);
  if (sp_src_node == nullptr || sp_dst_node == nullptr) {
    throw std::runtime_error(
        "Cannot call Graph::IsConnected if src or dst node don't exist in the graph");
  }
  for (const auto& item : graph_.find(sp_src_node)->second) {
    if (item.get()->node.lock().get()->node_val == dst) {
      return true;
    }
  }
  return false;
}

// iterate the whole graoh find the keys in the map
template <typename N, typename E>
std::vector<N> gdwg::Graph<N, E>::GetNodes() {
  std::vector<N> vector;
  for (const auto& item : graph_) {
    vector.push_back(item.first.get()->node_val);
  }
  return vector;
}

// According to the src node value to find it's correspond set
// the iterate that set to find it's dst nodes
template <typename N, typename E>
std::vector<N> gdwg::Graph<N, E>::GetConnected(const N& src) {
  std::shared_ptr<Node> node = find_node(src);
  if (node == nullptr) {
    throw std::out_of_range("Cannot call Graph::GetConnected if src doesn't exist in the graph");
  }
  std::vector<N> vector;
  // this set is used to sort the dst nodes, then insert those ordered nodes into vector
  std::set<N, Node_val_Compare> set;
  const auto& item = graph_.find(node)->second;
  if (has_set_value_) {
    for (const auto& set_item : item) {
      set.insert(set_item.get()->node.lock().get()->node_val);
    }
    for (const auto& set_item2 : set) {
      vector.push_back(set_item2);
    }
  }
  return vector;
}

//
template <typename N, typename E>
std::vector<E> gdwg::Graph<N, E>::GetWeights(const N& src, const N& dst) {
  auto sp_src_node = find_node(src);
  auto sp_dst_node = find_node(dst);
  if (sp_src_node == nullptr || sp_dst_node == nullptr) {
    throw std::out_of_range(
        "Cannot call Graph::GetWeights if src or dst node don't exist in the graph");
  }

  std::vector<E> vector;
  std::set<E, Weight_val_Compare> set;
  auto map_set = graph_.find(sp_src_node)->second;
  for (const auto& set_item : map_set) {
    // if the set's first element(shared_ptr<Node>) is same as the dst_node_sp
    // then validate the weight value is equal or not
    // if equal means this edge is exist.
    if (set_item.get()->node.lock() == sp_dst_node) {
      set.insert(*(set_item.get()->weight.get()));
    }
  }
  for (const auto& set_item2 : set) {
    vector.push_back(set_item2);
  }
  return vector;
}

// find the old_data's node in the map key
// then replace this node's value as new_data
// without change the memery address,
// and becauset the dst nodes in the set are using weak_ptr
// so dst node in the set will changed without any manipulation
template <typename N, typename E>
bool gdwg::Graph<N, E>::Replace(const N& old_data, const N& new_data) {
  std::shared_ptr<Node> old_node_ptr = find_node(old_data);
  std::shared_ptr<Node> new_node_ptr = find_node(new_data);
  if (old_node_ptr == nullptr) {
    throw std::runtime_error("Cannot call Graph::Replace on a node that doesn't exist");
  }
  if (new_node_ptr != nullptr) {
    return false;
  }
  old_node_ptr.get()->node_val = new_data;
  return true;
}

// Merge to nodes, find old_data node and new_data node's shared_ptr,
// then move old_data node's set into the new_data node's set
// second step, insert whole element in the new_data node's set into a new set
// to filter the duplicates, then use this new set override the new_data node's old set
// finally Delete old_data node
template <typename N, typename E>
void gdwg::Graph<N, E>::MergeReplace(const N& old_data, const N& new_data) {
  std::shared_ptr<Node> old_node_ptr = find_node(old_data);
  std::shared_ptr<Node> new_node_ptr = find_node(new_data);
  if (old_node_ptr == nullptr || new_node_ptr == nullptr) {
    throw std::runtime_error(
        "Cannot call Graph::MergeReplace on old or new data if they don't exist in the graph");
  }
  if (old_data == new_data) {
    return;
  }
  for (const auto& item : graph_.find(old_node_ptr)->second) {
    graph_.find(new_node_ptr)->second.insert(item);
  }
  for (const auto& map : graph_) {
    if (map.first.get()->node_val != old_data) {
      std::set<std::shared_ptr<Edge>, EdgeCompare> new_set;
      for (const auto& set_item : map.second) {
        if (set_item.get()->node.lock().get()->node_val == old_data) {
          set_item.get()->node = new_node_ptr;
        }
        new_set.insert(set_item);
      }
      graph_.find(map.first)->second = new_set;
    }
  }
  this->DeleteNode(old_data);
}

template <typename N, typename E>
bool gdwg::Graph<N, E>::erase(const N& src, const N& dst, const E& w) {
  auto src_node_sp = find_node(src);
  auto dst_node_sp = find_node(dst);
  if (src_node_sp == nullptr || dst_node_sp == nullptr) {
    return false;
  }
  for (const auto& set_item : graph_.find(src_node_sp)->second) {
    if (set_item.get()->node.lock().get()->node_val == dst &&
        *(set_item.get()->weight.get()) == w) {
      graph_.find(src_node_sp)->second.erase(set_item);
      return true;
    }
  }
  return false;
}

// =================================> const_iterator <=================================
template <typename N, typename E>
typename gdwg::Graph<N, E>::const_iterator gdwg::Graph<N, E>::cbegin() {
  // find the graph's first iterator which it's nest set is not empty
  auto first = std::find_if(
      graph_.begin(), graph_.end(),
      [](const std::pair<std::shared_ptr<Node>, std::set<std::shared_ptr<Edge>, EdgeCompare>>& s) {
        return !s.second.empty();
      });
  // if first equal to graph_end() then return cend
  if (first != graph_.end()) {
    return {first, graph_.end(), first->second.begin(), first->second.end()};
  }
  return cend();
}

template <typename N, typename E>
typename gdwg::Graph<N, E>::const_iterator gdwg::Graph<N, E>::cend() {
  return {graph_.end(), graph_.end(), {}, {}};
}

template <typename N, typename E>
typename gdwg::Graph<N, E>::const_reverse_iterator gdwg::Graph<N, E>::crbegin() {
  // find the graph's first iterator which it's nest set is not empty
  auto first = std::find_if(
      graph_.rbegin(), graph_.rend(),
      [](const std::pair<std::shared_ptr<Node>, std::set<std::shared_ptr<Edge>, EdgeCompare>>& s) {
        return !s.second.empty();
      });
  if (first != graph_.rend()) {
    return {first, graph_.rend(), first->second.rbegin(), first->second.rend()};
  }
  return crend();
}

template <typename N, typename E>
typename gdwg::Graph<N, E>::const_reverse_iterator gdwg::Graph<N, E>::crend() {
  std::set<std::shared_ptr<Edge>, EdgeCompare> set;
  return {graph_.rend(), graph_.rend(), {}, {}};
}

// iterate the how graph, then try to find the iterator
// which it's src node val, dst node val and weight val are equal to input src, dst and w
// if found the iterator then return this iterator
// return cend() otehrwise
template <typename N, typename E>
typename gdwg::Graph<N, E>::const_iterator
gdwg::Graph<N, E>::find(const N& src, const N& dst, const E& w) {
  for (auto iterator = this->cbegin(); iterator != this->cend(); ++iterator) {
    const auto& key = iterator.outer_->first.get()->node_val;
    const auto& set_node_val = (*iterator.inner_).get()->node.lock().get()->node_val;
    const auto& set_weight = *(*iterator.inner_).get()->weight;
    if (key == src && set_node_val == dst && set_weight == w) {
      return iterator;
    }
  }
  return cend();
}

// erase the require iterator and return the next iterator whatever it's and end() or other iterator
template <typename N, typename E>
typename gdwg::Graph<N, E>::const_iterator gdwg::Graph<N, E>::erase(const_iterator it) {
  if (it.inner_ != it.set_end_) {
    it.inner_ = it.outer_->second.erase(it.inner_);
    return it;
  }
  return this->end();
}
