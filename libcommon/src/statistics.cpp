#include <statistics.h>

std::ostream &operator<<(std::ostream &os, const Operation &op) {
  os << op.op_code << ' ';
  switch (op.op_code) {
  case Operation::CREATE: {
    os << op.group_id;
    break;
  }
  case Operation::MERGE: {
    os << op.merge_from << ' ' << op.merge_to;
    break;
  }
  }
  return os;
}

void Statistics::log(uint32_t timestamp) {
  if (m_log_graph_size) {
    log_graph_size(timestamp, boost::num_vertices(m_graph),
                   boost::num_edges(m_graph));
  }
  if (m_log_graph_cc) {
    log_connected_components(timestamp);
  }
}

void Statistics::log_connected_components(uint32_t timestamp) {
  if (m_operations.empty())
    return;
  m_output_graph_cc << timestamp << ' ' << m_operations.size() << std::endl;
  for (const auto &op : m_operations) {
    m_output_graph_cc << op;
    if (op.op_code == Operation::MERGE)
      m_output_graph_cc << ' ' << m_cc_size[op.merge_to];
    m_output_graph_cc << std::endl;
  }
  m_operations.clear();
};

uint32_t Statistics::p_find(uint32_t node) {
  if (m_parent[node] == node)
    return node;
  else {
    m_parent[node] = p_find(m_parent[node]);
    return m_parent[node];
  }
}

void Statistics::p_union(uint32_t from, uint32_t to) {
  if (from == to) {
    return;
  }
  auto from_p = p_find(from);
  auto to_p = p_find(to);

  if (from_p == to_p) {
    return;
  }
  if (m_rank[from_p] < m_rank[to_p]) {
    m_parent[from_p] = to_p;
    m_cc_size[to_p] += m_cc_size[from_p];
    m_operations.push_back(Operation(from_p, to_p));
    m_cc_size.erase(from_p);
    m_rank[to_p] += m_rank[from_p];
  } else if (m_rank[from_p] > m_rank[to_p]) {
    m_parent[to_p] = from_p;
    m_cc_size[from_p] += m_cc_size[to_p];
    m_operations.push_back(Operation(to_p, from_p));
    m_cc_size.erase(to_p);
    m_rank[from_p] += m_rank[to_p];
  } else {
    m_parent[from_p] = to_p;
    ++m_rank[to_p];
    m_cc_size[to_p] += m_cc_size[from_p];
    m_operations.push_back(Operation(from_p, to_p));
    m_cc_size.erase(from_p);
    m_rank[to_p] += m_rank[from_p];
  }
}

void Statistics::add_edges(
    const std::vector<std::tuple<Edge, Utils::EDGE_PROP>> &edges) {
  for (const auto &edge : edges) {
    using P = Utils::TUPLE_PROP;
    if (std::get<P::PROP>(edge) == Utils::INVALID)
      continue;
    uint32_t from =
        Utils::get_id(boost::source(std::get<P::EDGE>(edge), m_graph), m_graph);
    uint32_t to =
        Utils::get_id(boost::target(std::get<P::EDGE>(edge), m_graph), m_graph);
    // Calculate edge-cuts
    if (std::get<P::PROP>(edge) == Utils::NOT_FOUND) {
      if (m_partitioner.m_partitioning.at(from) !=
          m_partitioner.m_partitioning.at(to))
        ++m_edges_cut;
    }
    // Calculate Connected components
    if (m_log_graph_cc) {
      auto max_idx = std::max(from, to);
      for (int i = m_rank.size(); i < max_idx + 1; ++i) {
        m_cc_size[i] = 1;
        m_operations.push_back(Operation(i));
        m_rank.push_back(0);
        m_parent.push_back(i);
      }
      p_union(from, to);
    }
  }
  // std::cout << "Degree[6]: " << m_rank[6] << std::endl << "Degree[4]: " <<
  // m_rank[4] << std::endl;
}
