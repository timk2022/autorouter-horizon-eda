#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

#include "pathing.hpp"
#include "utils.hpp"

int main(void) {
  component_group_t *components;
  components = load_top_block("pcb-project/autorouter-testing/top_block.json");

  // print_component_group(components);
  board_load_and_parse(components, "pcb-project/autorouter-testing/board.json");
  // print_component_group(components);

  net_group_t *net_groups;
  net_groups = net_generation(components);
  print_net_list(net_groups);
  path_from_netlist(net_groups, components);
}