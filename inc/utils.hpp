#pragma once
#include "uuid.hpp"
#include <functional>
#include <string>
#include <vector>

// using json library https://github.com/nlohmann/json
#include <nlohmann/json.hpp>

// typedef struct Component;

class Component;
// class Vec3;
// class Vec3_int;
struct Vec3 {
  double x, y, z;
  Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
  Vec3(const Vec3 &v) : x(v.x), y(v.y), z(v.z) {}

  Vec3() {}

  Vec3 operator+(const Vec3 &v) const {
    return Vec3(x + v.x, y + v.y, z + v.z);
  }
  Vec3 operator-(const Vec3 &v) const {
    return Vec3(x - v.x, y - v.y, z - v.z);
  }
  Vec3 operator*(float d) const { return Vec3(x * d, y * d, z * d); }
  Vec3 operator/(float d) const { return Vec3(x / d, y / d, z / d); }
  Vec3 max(const Vec3 &v) {
    return Vec3((x > v.x) ? x : v.x, (y > v.y) ? y : v.y, (z > v.z) ? z : v.z);
  }
  Vec3 normalize() const {
    double mg = sqrt(x * x + y * y + z * z);
    return Vec3(x / mg, y / mg, z / mg);
  }

  void print() const { printf("x: %f, y: %f, z: %f\n", x, y, z); }
};

struct Vec3_int {
  int x, y, z;
  Vec3_int(int x, int y, int z) : x(x), y(y), z(z) {}
  Vec3_int(const Vec3_int &v) : x(v.x), y(v.y), z(v.z) {}
  Vec3_int() {}

  Vec3_int operator+(const Vec3_int &v) const {
    return Vec3_int(x + v.x, y + v.y, z + v.z);
  }
  Vec3_int operator-(const Vec3_int &v) const {
    return Vec3_int(x - v.x, y - v.y, z - v.z);
  }
  bool operator ==(const Vec3_int &v) const {
    return (x==v.x && y==v.y && z ==v.z);
  }
  bool operator !=(const Vec3_int &v) const {
    return !(x==v.x && y==v.y && z ==v.z);
  }

  Vec3_int operator*(float d) const { return Vec3_int(x * d, y * d, z * d); }
  Vec3_int operator/(float d) const { return Vec3_int(x / d, y / d, z / d); }
  Vec3_int max(const Vec3_int &v) {
    return Vec3_int((x > v.x) ? x : v.x, (y > v.y) ? y : v.y,
                    (z > v.z) ? z : v.z);
  }

  int dot(const Vec3_int &v) { return (x * v.x + y * v.y + z * v.z); }
  double dot(const Vec3 &v) {
    return ((double)x * v.x + (double)y * v.y + (double)z * v.z);
  }

  double distance_xy(const Vec3_int &v) {
    double dist_2 = pow(x - v.x, 2) + pow(y - v.y, 2);
    if (dist_2 > 0) {
      return sqrt(dist_2);
    }
    // change this?
    return 0;
  }
  double distance(const Vec3_int &v) {
    double dist_2 = pow(x - v.x, 2) + pow(y - v.y, 2) + pow(z - v.z, 2);
    if (dist_2 > 0) {
      return sqrt(dist_2);
    }
    // change this?
    return 0;
  }

  Vec3_int rotate(double angle) {
    Vec3 r_0 = Vec3(cos(angle), -sin(angle), 0);
    Vec3 r_1 = Vec3(sin(angle), cos(angle), 0);
    return Vec3_int(x * r_0.x + y * r_0.y, x * r_1.x + y * r_1.y, 0);
  }

  void print() const { printf("x: %i, y: %i, z: %i\n", x, y, z); }
};

struct connection_t {
  UUID gate;
  UUID pin;
  UUID comp_id;
  Component *comp_pointer;
  // unsigned long long gate_pin_hashed;
  UUID net;

  UUID pad_id;
  UUID package_id;
  struct Vec3_int pad_offset;
  double pad_angle;
  // unsigned long long net_hashed;
  // connection_t(UUID gate, UUID pin, UUID comp_id, Component * comp_pointer,
  // UUID net) :
  //     gate(gate), pin(pin), comp_id(comp_id), comp_pointer(comp_pointer),
  //     net(net) {}

  connection_t() {}

  connection_t(const connection_t &c)
      : gate(c.gate), pin(c.pin), comp_id(c.comp_id), net(c.net),
        comp_pointer(c.comp_pointer),

        package_id(package_id),

        pad_id(c.pad_id), pad_offset(c.pad_offset), pad_angle(c.pad_angle)

  {}

  connection_t operator=(const connection_t &c) {
    if (this != &c) {
      gate = c.gate;
      pin = c.pin;
      comp_id = c.comp_id;
      net = c.net;
      comp_pointer = c.comp_pointer;
      pad_id = c.pad_id;
      package_id = c.package_id;
    }
    return *this;
  }
  // ~connection_t(){

  // }
};

// todo: add courtyard as bounds

struct polygon {
  std::vector<std::pair<Vec3_int, Vec3_int>> vertices;

  // line_0 connects vertex 0 and vertex 1
  // if it is curved, it is marked as a curve
  // and the curve center is set in verticies.second
  enum line_t { POLYGON_STRAIGHT, POLYGON_CURVE };
  std::vector<line_t> line_type;

  UUID polygon_id;

  polygon() {}
  polygon(const polygon &p)
      : vertices(p.vertices), line_type(p.line_type), polygon_id(p.polygon_id) {
  }
  polygon operator=(const polygon &p) {
    if (this != &p) {
      vertices = p.vertices;
      line_type = p.line_type;
      polygon_id = p.polygon_id;
    }
    return *this;
  }
  ~polygon() {
    vertices.clear();
    line_type.clear();
  }
};

class Component {
public:
  UUID component_id;
  UUID board_comp_id;

  UUID entity_id;
  UUID part_id;
  UUID group;
  UUID tag;

  bool mirrored;
  struct Vec3_int pos_offset;
  double angle;
  bool is_used;
  bool is_fixed;

  // package object information
  // used for spacing calcs
  polygon courtyard;
  std::vector<polygon> pads;

  std::vector<connection_t> conn_arr;

  Component() {}

  Component(const Component &c)
      : component_id(c.component_id), board_comp_id(c.board_comp_id),
        entity_id(c.entity_id), part_id(c.part_id), group(c.group), tag(c.tag),
        conn_arr(std::vector<connection_t>(c.conn_arr)), pos_offset(pos_offset),
        angle(angle), mirrored(mirrored), is_used(is_used), is_fixed(is_fixed) {
  }

  Component operator=(const Component &c) {
    if (this != &c) {
      component_id = c.component_id;
      board_comp_id = c.board_comp_id;
      entity_id = c.entity_id;
      part_id = c.part_id;
      group = c.group;
      tag = c.tag;
      conn_arr = std::vector<connection_t>(c.conn_arr);
      pos_offset = c.pos_offset;
      angle = c.angle;
      mirrored = c.mirrored;
      is_used = c.is_used;
      is_fixed = c.is_fixed;
    }
    return *this;
  }

  ~Component() { conn_arr.clear(); }
};

struct component_group_t {
  nlohmann::json j;
  std::vector<Component> comp_arr;

  component_group_t() {}
  component_group_t(const component_group_t &c)
      : j(j), comp_arr(std::vector<Component>(comp_arr)) {}

  ~component_group_t() { comp_arr.clear(); }
  // component_group_t(nlohmann::json j, std::vector<Component> comp_arr,
  // uint32_t comp_arr_len) :
  //     j(j), comp_arr(comp_arr), comp_arr_len(comp_arr_len) {}
};

struct connection_group_t {
  std::vector<connection_t> conn_arr;
  uint32_t conn_arr_len;

  connection_group_t(std::vector<connection_t> conn_arr, uint32_t conn_arr_len)
      : conn_arr(conn_arr), conn_arr_len(conn_arr_len) {}

  ~connection_group_t() { conn_arr.clear(); }
};

struct net_t {
  UUID net_id;
  UUID net_class_id;

  std::string net_name;

  enum net_class_t { default_class };
  net_class_t net_class;

  // todo: add net_class_properties? (i.e. impedance sensitive, RF,
  // digital/analog, etc.)
  bool is_power;
  bool is_used;

  std::vector<connection_t> linked_conns_arr;
  uint32_t linked_conns_arr_len;

  net_t() {}

  net_t(UUID net_id, UUID net_class_id, net_class_t net_class, bool is_power,
        bool is_used, std::vector<connection_t> linked_conns_arr,
        uint32_t linked_conns_arr_len)
      : net_id(net_id), net_class_id(net_class_id), net_class(net_class),
        is_power(is_power), is_used(is_used),
        linked_conns_arr(linked_conns_arr) {}

  net_t operator=(const net_t &n) {
    if (this != &n) {
      net_id = n.net_id;
      net_class_id = n.net_class_id;
      net_class = n.net_class;

      net_name = n.net_name;

      is_power = n.is_power;
      is_used = n.is_used;
      linked_conns_arr = std::vector<connection_t>(n.linked_conns_arr);
      linked_conns_arr_len = n.linked_conns_arr_len;
    }
    return *this;
  }

  ~net_t() { linked_conns_arr.clear(); }
};

struct net_group_t {
  std::vector<net_t> nets;
  uint32_t num_nets;
};

struct component_group_t *load_top_block(const std::string &filename);
void print_component_group(struct component_group_t *component_arr);
void print_net_list(struct net_group_t *net_list);

net_group_t *net_generation(component_group_t *components);
void board_load_and_parse(component_group_t *comp_group, const std::string &filename);
double CLOCK(void);