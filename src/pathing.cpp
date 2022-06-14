#include "pathing.hpp"
#include "matplotlibcpp.h"
#include <queue>

#define PI 3.14159265
#define TAU (2 * PI)
#define MAX_ANGLE (UINT16_MAX + 1)

namespace plt = matplotlibcpp;

void plot_path_starts(path_group_t *paths, obstacle_group_t *obstacles) {
  std::cout << "============== PLOTTING PATHS ===============" << std::endl;
  plt::figure_size(1200, 780);
  plt::xlim(0, 6000000);
  plt::ylim(0, 6000000);
  std::vector<int> x, y;

  std::vector<int> x_ends, y_ends;

  for (auto i = paths->path_arr.begin(); i != paths->path_arr.end(); i++) {
    x.push_back(i->start.pos.x);
    y.push_back(i->start.pos.y);

    for (auto j = i->ends.begin(); j != i->ends.end(); j++) {
      x_ends.push_back(j->pos.x);
      y_ends.push_back(j->pos.y);
    }
  }

  plt::scatter(x, y);
  plt::scatter(x_ends, y_ends);

  std::vector<int> x_path, y_path;
  // std::vector<std::vector<int>> x_tmp, y_tmp;
  for (auto i = paths->path_arr.begin(); i != paths->path_arr.end(); i++) {
    for(auto j = i->node_path.begin(); j != i->node_path.end(); j++){

      x_path.push_back(j->pos.x);
      y_path.push_back(j->pos.y);
    }
    plt::plot(x_path, y_path);
  }
  
  for (auto i = obstacles->obs_arr.begin(); i != obstacles->obs_arr.end();
       i++) {
    std::vector<int> x_tmp, y_tmp;
    std::pair<std::vector<int>, std::vector<int>> tmp_vecs;
    tmp_vecs = i->get_vectors();
    plt::plot(tmp_vecs.first, tmp_vecs.second);
  }

  // plt::save("tmp2.png");
  plt::show();
}


void retrace_path(Path *path, node n, std::vector<node> * mem){
  // node * next = n.prev_node_pointer;
  // node next = *n.prev_node_pointer;
  for(auto i = mem->begin(); i!= mem->end(); i++){
    path->node_path.push_back(*i);
  }
  // while(*next != path->start || next->prev_node_pointer == nullptr){
  //   path->node_path.push_back(*next);
  //   next = next->prev_node_pointer;
  // }
  // path->node_path.push_back(path->start);
  std::reverse(path->node_path.begin(), path->node_path.end());
}

// returns true if path connects to end goal, false otherwise
bool a_star(Path * path, obstacle_group_t * obs){

  Priority_Queue * queue = new Priority_Queue;
  node end_node = path->ends[0];

  path->start.g = 0;
  path->start.h = path->get_cost(path->start,end_node);
  path->start.f = path->start.g + path->start.h;

  queue->add_to_queue(path->start, path->start.h);
  std::vector<node> closed_list;

  std::vector<node> mem;

  while(!queue->is_empty()){
    node current = queue->top().first;
    queue->pop();

    mem.push_back(current);
    node * current_addr = &mem.back();
    
    if(current == path->ends[0]){
      retrace_path(path, current, &mem);
      return true;
    }
    // closed_list.push_back(current);

    std::vector<node> neighbors = path->get_neighbors(current);
    int max_cost = INT32_MAX;
    for(auto n = neighbors.begin(); n != neighbors.end(); n++){
      n->g = current.g + path->get_cost(*n, current);
      n->h = path->get_cost(*n, end_node);
      n->f = n->g + n->h;
      n->prev_node_pointer = current_addr;
      if (n->g < max_cost || std::find(closed_list.begin(), closed_list.end(), *n) == closed_list.end()){
        max_cost = n->g;
        if (*n == path->ends[0]){
          retrace_path(path, *n, &mem);
          return true;
        }
        if (!(queue->node_in_queue(*n))){
          queue->add_to_queue(*n, n->h);
        } 
        closed_list.push_back(*n);
        // if (std::find(closed_list.begin(), closed_list.end(), *n) == closed_list.end()){
        //   // mem.push_back(*n);
        //   closed_list.push_back(*n);

      // }
        
        
      }
    }
    

  }
  return false;
}

void pathing(path_group_t *paths, obstacle_group_t * obs){
  for(int i = 0; i < paths->path_arr.size(); i++){
  // for(auto i = paths->path_arr.begin(); i != paths->path_arr.end(); i++){
    if(paths->path_arr[i].ends.size() > 0){
      if (a_star(&(paths->path_arr[i]), obs)){
        std::cout << "path found" << std::endl;
      }
    }
  }

}

// convert netlist into lighter data structures
void path_from_netlist(net_group_t *net_list, component_group_t *components) {
  // rumbling
  path_group_t *paths = new path_group_t;

  paths->num_paths = net_list->nets.size();
  paths->path_arr.reserve(net_list->nets.size());

  // initializing pathing
  for (auto i = net_list->nets.begin(); i != net_list->nets.end(); i++) {
    Path new_path;
    new_path.num_ends = 0;
    new_path.ends.reserve(i->linked_conns_arr.size());
    for (auto j = i->linked_conns_arr.begin(); j != i->linked_conns_arr.end();
         j++) {
      Vec3_int comp_center = j->comp_pointer->pos_offset;
      double comp_angle = TAU * j->comp_pointer->angle / MAX_ANGLE;

      Vec3_int pad_center = j->pad_offset;

      Vec3_int pad_center_new = pad_center.rotate(comp_angle) + comp_center;

      node new_node;
      new_node.pos = pad_center_new;
      new_node.prev_node_pointer = nullptr;
      if (j == i->linked_conns_arr.begin()) {
        new_path.start = new_node;
      } else {
        new_path.ends.push_back(new_node);
        new_path.num_ends++;
      }
    }
    paths->path_arr.push_back(new_path);
  }

  obstacle_group_t *obstacles = new obstacle_group_t;

  obstacles->num_obs = components->comp_arr.size();
  obstacles->obs_arr.reserve(obstacles->num_obs);

  for (auto i = components->comp_arr.begin(); i != components->comp_arr.end();
       i++) {
    Obstacle new_obstacle;
    double comp_angle = TAU * i->angle / MAX_ANGLE;

    for (auto j = i->courtyard.vertices.begin();
         j != i->courtyard.vertices.end(); j++) {
      j->first = j->first.rotate(comp_angle);
    }
    new_obstacle.center = i->pos_offset;
    new_obstacle.vert = i->courtyard;
    obstacles->obs_arr.push_back(new_obstacle);
  }

  for (auto i = net_list->nets.begin(); i != net_list->nets.end(); i++) {
    for (auto j = i->linked_conns_arr.begin(); j != i->linked_conns_arr.end();
         j++) {
      Obstacle new_obstacle;
      double comp_angle = TAU * j->comp_pointer->angle / MAX_ANGLE;

      new_obstacle.center =
          j->pad_offset.rotate(comp_angle) + j->comp_pointer->pos_offset;
      UUID pad_id = j->pad_id;
      for (auto k = j->comp_pointer->pads.begin();
           k != j->comp_pointer->pads.end(); k++) {
        if (k->polygon_id == pad_id) {
          for (auto x = k->vertices.begin(); x != k->vertices.end(); x++) {
            // new_obstacle.vert.vertices.push_back(std::make_pair(x->first.rotate(comp_anglex->second))
            x->first = x->first.rotate(comp_angle);
          }
          new_obstacle.vert.vertices = k->vertices;
          new_obstacle.vert.line_type = k->line_type;
          // new_obstacle.vert= k;/
          break;
        }
      }
      obstacles->obs_arr.push_back(new_obstacle);
    }
  }
  pathing(paths, obstacles);
  plot_path_starts(paths, obstacles);
}