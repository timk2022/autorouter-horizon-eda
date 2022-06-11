#include "pathing.hpp"
#include "matplotlibcpp.h"

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

  // std::vector<std::vector<int>> x_tmp, y_tmp;

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
      new_node.prev_node_index = UINT32_MAX;
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

  plot_path_starts(paths, obstacles);
}