#include <statistics.h>

void Statistics::log(uint32_t timestamp) {
  if (m_log_graph_size) {
    log_graph_size(timestamp, boost::num_vertices(m_graph),
                   boost::num_edges(m_graph));
  }
  if (m_log_graph_cc) {
    log_connected_components(timestamp);
  }
}

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
  auto from_r = p_find(from);
  auto to_r = p_find(to);
  if (from_r == to_r)
    return;
  if (m_rank[from_r] < m_rank[to_r])
    m_parent[from_r] = to_r;
  else if (m_rank[to_r] < m_rank[from_r])
    m_parent[to_r] = from_r;
  else {
    m_parent[from_r] = to_r;
    ++m_rank[to_r];
  }
  --m_number_of_cc;
}

void Statistics::add_edges(
    const std::vector<std::pair<uint32_t, uint32_t>> &edges) {

  // if (!m_log_graph_cc)
  //   return;
  // auto idx_from = m_graph[from].m_vertex_id;
  // auto idx_to = m_graph[to].m_vertex_id;
  // auto max_idx = std::max(idx_from, idx_to);
  // for (int i = m_rank.size(); i < max_idx + 1; ++i) {
  //   ++m_number_of_cc;
  //   m_rank.push_back(0);
  //   m_parent.push_back(i);
  // }
  // p_union(idx_from, idx_to);
}
