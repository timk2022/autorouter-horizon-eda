#pragma once
#include "utils.hpp"

#include <iostream>
#include <vector>
// todo: add type and component pointer?
class Obstacle {
public:
  // treating each obstacle as being bounded by a rectangle
  Vec3_int center;

  enum obstacle_type { PAD, COURTYARD };
  obstacle_type obstacle_type;

  // todo: add support for nested polygons  (ex: donut)
  polygon vert;
  Vec3_int side_half_lengths;

  Obstacle() {}

  Obstacle(const Obstacle &o)
      : obstacle_type(o.obstacle_type), center(o.center), vert(o.vert),
        side_half_lengths(o.side_half_lengths) {}

  Obstacle operator=(const Obstacle &o) {
    if (this != &o) {
      obstacle_type = o.obstacle_type;
      vert = o.vert;
      center = o.center;
      side_half_lengths = o.side_half_lengths;
    }
    return *this;
  }

  // return true if given point is within bounds
  bool intersects(Vec3_int point) {
    // fixme: bug if polygon overlaps with origin (need to check from farther
    // away too)
    Vec3_int source_vector = Vec3_int(0, 0, 0);
    // draw vector from origin to point, if it intersects an odd
    // number of lines, the point is inside the polygon
    // otherwise it is outside
    if (point.z == center.z) {
      if (!cached) {
        for (auto i = vert.vertices.begin(); i != vert.vertices.end(); i++) {
          if (i->first.x > x_max) {
            x_max = i->first.x;
          } else if (i->first.x < x_min) {
            x_min = i->first.x;
          }
          if (i->first.y > y_max) {
            y_max = i->first.y;
          } else if (i->first.y < y_min) {
            y_min = i->first.y;
          }
        }

        cached = true;
      }
      int intersections = 0;
      if (point.x >= x_min && point.x <= x_max && point.y >= y_min &&
          point.y <= y_max) {
        // https://math.stackexchange.com/questions/149622/finding-out-whether-two-line-segments-intersect-each-other
        // since one of the points is at 0,0 -> can ignore min(c,d)
        for (auto i = 1; i < vert.vertices.size(); i++) {
          Vec3_int a = vert.vertices[i].first;
          Vec3_int b = vert.vertices[i - 1].first;
          int h_c = h(point, a, b);
          int h_d = h(source_vector, a, b);
          int g_a = g(a, point, source_vector);
          int g_b = g(b, point, source_vector);
          if (h_c * h_d < 0 && g_a * g_b < 0) {
            intersections++;
          } else if (h_c == 0 && h_d == 0) {
            // check if two collinear segments intersect on line
            if (min(point.x, source_vector.x) <= max(a.x, b.x) &&
                max(point.x, source_vector.x) >= min(a.x, b.x) &&
                min(point.y, source_vector.y) <= max(a.y, b.y) &&
                max(point.y, source_vector.y) >= min(a.y, b.y)) {
              intersections++;
            }
          }
        }
        return intersections % 2;
      }
    }
    return false;
  }

  std::pair<std::vector<int>, std::vector<int>> get_vectors(void) {
    std::pair<std::vector<int>, std::vector<int>> vecs;

    for (auto i = vert.vertices.begin(); i != vert.vertices.end(); i++) {
      vecs.first.push_back(i->first.x + center.x);
      vecs.second.push_back(i->first.y + center.y);
    }

    return vecs;
  }

private:
  int x_min = INT32_MAX;
  int x_max = INT32_MIN;
  int y_min = INT32_MAX;
  int y_max = INT32_MIN;
  bool cached = false;

  int min(int a, int b) {
    if (a > b) {
      return b;
    }
    return a;
  }

  int max(int a, int b) {
    if (a > b) {
      return a;
    }
    return b;
  }

  int g(Vec3_int P, Vec3_int C, Vec3_int D) { return h(P, C, D); }

  int h(Vec3_int P, Vec3_int A, Vec3_int B) {
    return ((B - A).x * (P - A).y - (B - A).y * (P - A).x);
  }
};

struct obstacle_group_t {
  std::vector<Obstacle> obs_arr;
  uint32_t num_obs;

  obstacle_group_t() {}

  obstacle_group_t operator=(const obstacle_group_t &o) {
    if (this != &o) {
      obs_arr = o.obs_arr;
      num_obs = o.num_obs;
    }
    return *this;
  }

  ~obstacle_group_t() { obs_arr.clear(); }
};

struct node;

struct node {
  node * prev_node_pointer;
  Vec3_int pos;
  Vec3_int parent_node;
  // costs
  int g;
  int f;
  int h;
  //todo: change definition here
  // node parent_node;
  
  node get_shifted_pos(int x, int y){
    node new_node;
    new_node.pos = pos + Vec3_int(x,y,0);
    return new_node;
  }

  node() {}
  node(const node &n) : prev_node_pointer(n.prev_node_pointer), pos(n.pos), parent_node(n.parent_node), g(n.g), f(n.f), h(n.h) {}
  bool operator ==(const node &n) {
    return (pos == n.pos); 
  }
  bool operator != (const node &n){
    return !(pos == n.pos);
  }
  
  node operator=(const node &n) {
    if (this != &n) {
      prev_node_pointer = n.prev_node_pointer;
      parent_node = n.parent_node;
      pos = n.pos;
      g = n.g;
      f = n.f;
      h = n.h;
    }
    return *this;
  }

  // ~node (){
  //     if (this1)
  // }
};
class Path {
public:
  node start;
  std::vector<node> ends;
  uint32_t num_ends;

  std::vector<node> node_path;
  uint32_t num_nodes;

  // todo:: add support for multiple end points
  int get_cost(node point, node end){
    return (abs(end.pos.x - point.pos.x) + abs(end.pos.y - point.pos.y));
  }
  int get_cost(Vec3_int point_pos, Vec3_int end_pos){
    return (abs(end_pos.x - point_pos.x) + abs(end_pos.y - point_pos.y));
  }

  std::vector<node> get_neighbors(node n){
    std::vector<node> neighbors;
    neighbors.reserve(8);
    for(int x = -grid_spacing; x <= grid_spacing; x += grid_spacing){
      for (int y = -grid_spacing; y <= grid_spacing; y += grid_spacing){
        if (!(x == 0 && y == 0)){
          node tmp = n.get_shifted_pos(x,y);
          // tmp.prev_node_pointer = n;
          if (n != start){
            // check if previous node is defined (todo: figure out better behavior here) 
            // if ( n->prev_node_pointer == nullptr){
                // neighbors.push_back(tmp);
            // } else{
              if(tmp.pos != n.parent_node){
                tmp.parent_node= n.pos;
              // if (tmp != *n->prev_node_pointer){
                neighbors.push_back(tmp);
              }
            // }
          } else {
            tmp.parent_node = start.pos;
            neighbors.push_back(tmp);
          }
          
        // }
        }
      }
    }
    return neighbors;

    // for (auto i = node_tree.begin(); i != node_tree.end(); i++){

    // }
  }


  Path() {}

  Path(const Path &p)
      : start(p.start), ends(p.ends), num_ends(p.num_ends),
        node_path(p.node_path), num_nodes(num_nodes) {}

  Path operator=(const Path &p) {
    if (this != &p) {
      start = p.start;
      // ends = p.ends;
      ends = p.ends;
      num_ends = p.num_ends;
      // node_tree = p.node_tree;
      node_path = p.node_path;
      num_nodes = p.num_nodes;
    }
    return *this;
  }


  ~Path() {
    ends.clear();
    node_path.clear();
  }
  private:
    int grid_spacing = 10000;

};

struct path_group_t {
  std::vector<Path> path_arr;
  uint32_t num_paths;

  path_group_t() {}

  ~path_group_t() { path_arr.clear(); }
};

// class Priority_Queue{
//   public:
//     std::priority_queue<int> queue;
//     std::unordered_multimap<int, node> map;
//     std::unordered_map<node, int> reverse_map;

//     void add_to_queue(int c, node n){
//       queue.push(c);
//       map.insert(std::make_pair(c,n));
//       reverse_map.insert(std::make_pair(n,c));
//     }

//     bool node_exists(node n){
//       return (reverse_map.find(n) != reverse_map.end());
//     }

//     bool is_empty(void){
//       return queue.empty();
//     }

//     void pop(void){
//       int cost = queue.top();
//       queue.pop();
//       auto it = map.find(cost);
//       reverse_map.erase(it->second);
//     }

//     Priority_Queue(){}
//     ~Priority_Queue(){
//       map.clear();
//     }
// }
class Priority_Queue{
  public:
    // defining each node with node and its priority (smaller cost -> higher priority) in the queue
    std::vector<std::pair<node, int>> nodes;

    Priority_Queue(){}

    bool is_empty(void){
      return nodes.size() == 0;
    }

    void add_to_queue(node new_node, int cost){
      // if (nodes.size() == 0){
      //   return;
      // }
      for(auto i = nodes.begin(); i != nodes.end(); i++){
        if (i->second > cost){
          nodes.insert(i, std::make_pair(new_node,cost));
          return;
        }
      }
      nodes.push_back(std::make_pair(new_node,cost));
      return;
    };

    //todo: improve here, removing elements beside vector.end is slow
    void pop(){
      nodes.erase(nodes.begin());
    }

    std::pair<node,int> top(){
      return nodes[0];
    }

    // check if node is in queue and replace if it is cheaper
    bool node_in_queue(node key){
      for(auto i = nodes.begin(); i != nodes.end(); i++){
        if (i->first == key){
          if (i->second > key.h){
            std::cout << "cheaper node found" << std::endl;
            nodes.erase(i);
            add_to_queue(key, key.f);
          }
          return true;
        }
      }
      return false;
    };



};

void path_from_netlist(net_group_t *net_list, component_group_t *components);