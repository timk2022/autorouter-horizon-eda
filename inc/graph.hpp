#pragma once

#include <string>

class Node {
public:
  enum node_type_enum { null_type, junction, pin, port, bus_ripper };

  std::string node_id;
  node_type_enum node_type;
  Node **connections;
  uint32_t num_connections;

  Node() {
    node_id = "";
    node_type = null_type;
    connections = nullptr;
    num_connections = 0;
  }
};

class Graph {
public:
};