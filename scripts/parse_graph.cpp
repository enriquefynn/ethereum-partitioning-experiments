#include <cassert>
#include <fstream>
#include <iostream>
using namespace std;

enum {
  CREATE_TYPE,
  CALL_TYPE,
  CALLCODE_TYPE,
  DELEGATECALL_TYPE,
  STATICCALL_TYPE,
  PRECOMPILED_TYPE,
  OPSELFDESTRUCT_TYPE
};

int main(int argc, char **argv) {
  char header;
  int n_transactions;
  string tx_value;

  int tx_author, tx_failed, tx_type, tx_type_n, tx_types_size, block_number,
      new_timestamp, has_from, from_vertex, to_vertex, has_value, weight;

  ifstream calls_file(argv[1]);
  calls_file >> header >> n_transactions;
  for (int tx = 0; tx < n_transactions; ++tx) {
    int to_vertex;
    calls_file >> to_vertex >> tx_value;

    cout << to_vertex << ' ' << tx_value << endl;
    // This is a vertex without an edge.
  }

  while (calls_file >> header >> block_number >> new_timestamp >>
         n_transactions) {
    for (int tx = 0; tx < n_transactions; ++tx) {
      calls_file >> header >> tx_author >> tx_failed >> tx_types_size;
      assert(header == 'T');
      for (int tx_n_type = 0; tx_n_type < tx_types_size; ++tx_n_type) {
        calls_file >> tx_type >> tx_type_n;
        for (int tx_call = 0; tx_call < tx_type_n; ++tx_call) {
          calls_file >> has_from;
          if (has_from == 1) {
            from_vertex = tx_author;
            calls_file >> to_vertex;
          } else if (has_from == 2) {
            calls_file >> from_vertex >> to_vertex;
          } else
            assert(false);
          if (has_value)
            calls_file >> tx_value;
          calls_file >> weight;
          if (tx_type == OPSELFDESTRUCT_TYPE && !tx_failed) {
            // Delete from_vertex
            // TODO
          }
          // TODO
          cout << from_vertex << ' ' << to_vertex << ' ' << weight << endl;
        }
      }
    }
  }
}