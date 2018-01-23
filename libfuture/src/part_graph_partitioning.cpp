#include <cassert>
#include <iomanip>
#include <metis.h>

#include <log.h>
#include <part_graph_partitioning.h>
#include <partitioner.h>
#include <utils.h>

uint32_t Part_graph_partitioner::partition(int32_t nparts) {
  // Get actual partitioning of existing graph in current graph
  for (auto v_it = boost::vertices(m_cur_graph); v_it.first != v_it.second;
       ++v_it.first) {
    auto v_id = Utils::get_id(*v_it.first, m_cur_graph);
    m_cur_partitioning[v_id] = m_partitioning[v_id];
  }

  uint32_t moves = METIS_partitioner::partition(m_cur_graph, m_cur_id_to_vertex,
                                                m_cur_partitioning, nparts);

  for (const auto v_part : m_cur_partitioning)
    m_partitioning[v_part.first] = v_part.second;
  m_cur_graph.clear();
  m_cur_partitioning.clear();
  m_cur_id_to_vertex.clear();
  return moves;
}

void Part_graph_partitioner::added_edge(uint32_t from, uint32_t to) {
  auto v_fr = m_cur_id_to_vertex.find(from);
  Vertex fr_desc, to_desc;
  fr_desc = to_desc = nullptr;
  if (v_fr == std::end(m_cur_id_to_vertex)) {
    fr_desc = boost::add_vertex(m_cur_graph);
    m_cur_graph[fr_desc] = VertexProperty(from);
    m_cur_id_to_vertex.insert(v_fr, {from, fr_desc});
  } else
    fr_desc = (*v_fr).second;

  auto v_to = m_cur_id_to_vertex.find(to);
  if (v_to == std::end(m_cur_id_to_vertex)) {
    to_desc = boost::add_vertex(m_cur_graph);
    m_cur_graph[to_desc] = VertexProperty(to);
    m_cur_id_to_vertex.insert(v_to, {to, to_desc});
  } else
    to_desc = (*v_to).second;

  auto weights_map = get(boost::edge_weight, m_cur_graph);
  Edge edge;
  bool edge_found;
  tie(edge, edge_found) = boost::edge(fr_desc, to_desc, m_cur_graph);
  if (edge_found) {
    auto current_weight = get(weights_map, edge);
    put(weights_map, edge, current_weight + 1);
  } else {
    tie(edge, edge_found) = boost::add_edge(fr_desc, to_desc, m_cur_graph);
    put(weights_map, edge, 1);
  }
  // Update vertex weight
  ++m_cur_graph[boost::source(edge, m_cur_graph)].m_vertex_weight;
  ++m_cur_graph[boost::target(edge, m_cur_graph)].m_vertex_weight;
}