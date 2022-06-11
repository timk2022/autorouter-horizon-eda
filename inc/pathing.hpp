#pragma once
#include "utils.hpp"

#include <vector>
//todo: add type and component pointer?
class Obstacle {
    public:
        // treating each obstacle as being bounded by a rectangle
        Vec3_int center;

        enum obstacle_type{PAD, COURTYARD};
        obstacle_type obstacle_type;

        // todo: add support for nested polygons  (ex: donut)
        polygon vert; 
        Vec3_int side_half_lengths;

        Obstacle(){}

        Obstacle(const Obstacle& o) :
            obstacle_type(o.obstacle_type),
            center(o.center),
            vert(o.vert),
            side_half_lengths(o.side_half_lengths)
        {}
        

        Obstacle operator = (const Obstacle& o){
            if (this != &o){
                obstacle_type = o.obstacle_type;
                vert = o.vert;
                center = o.center;
                side_half_lengths= o.side_half_lengths;
            }            
            return *this;
        }

        // return true if given point is within bounds
        bool intersects(Vec3_int point) {
            //fixme: bug if polygon overlaps with origin (need to check from farther away too)
            Vec3_int source_vector = Vec3_int(0,0,0);
            // draw vector from origin to point, if it intersects an odd
            // number of lines, the point is inside the polygon
            // otherwise it is outside
            if (point.z == center.z){
                if (!cached){
                    for (auto i = vert.vertices.begin(); i != vert.vertices.end(); i++){
                        if(i->first.x > x_max){
                            x_max = i->first.x;
                        } else if (i->first.x < x_min){
                            x_min = i->first.x;
                        }
                        if(i->first.y > y_max){
                            y_max = i->first.y;
                        } else if (i->first.y < y_min){
                            y_min = i->first.y;
                        }
                    }
                    
                    cached = true;
                }
                int intersections = 0;
                if (point.x >= x_min && point.x <= x_max && point.y >= y_min && point.y <= y_max){
                    //https://math.stackexchange.com/questions/149622/finding-out-whether-two-line-segments-intersect-each-other
                    // since one of the points is at 0,0 -> can ignore min(c,d)
                    for(auto i = 1; i < vert.vertices.size(); i++){   
                        Vec3_int a = vert.vertices[i].first;
                        Vec3_int b = vert.vertices[i-1].first;
                        int h_c = h(point, a, b);
                        int h_d = h(source_vector, a, b);
                        int g_a = g(a, point, source_vector);
                        int g_b = g(b, point, source_vector);
                        if(h_c*h_d < 0 && g_a*g_b < 0){
                            intersections++;
                        } else if (h_c == 0 && h_d == 0){
                            // check if two collinear segments intersect on line
                            if(min(point.x, source_vector.x) <= max(a.x, b.x) &&
                               max(point.x,source_vector.x) >= min(a.x,b.x) &&
                               min(point.y, source_vector.y) <= max(a.y,b.y) &&
                               max(point.y, source_vector.y) >= min(a.y,b.y))
                            {
                                   intersections++;
                            }
                        }
                    }
                    return intersections%2;
                }
            }
            return false;
        }

        std::pair<std::vector<int>, std::vector<int>> get_vectors(void){
            std::pair<std::vector<int>, std::vector<int>> vecs;

            for (auto i = vert.vertices.begin(); i != vert.vertices.end(); i++){
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

        int min(int a, int b){
            if (a > b){
                return b;
            }
            return a;
        }

        int max(int a, int b){
            if(a>b){
                return a;
            }
            return b;
        }

        int g(Vec3_int P, Vec3_int C, Vec3_int D){
            return h(P,C,D);
        }

        int h(Vec3_int P, Vec3_int A, Vec3_int B){
            return ((B-A).x*(P-A).y - (B-A).y * (P-A).x);
        }
};

struct obstacle_group_t {
    std::vector<Obstacle> obs_arr;
    uint32_t num_obs;

    obstacle_group_t(){}

    obstacle_group_t operator = (const obstacle_group_t& o){
        if(this != &o){
            obs_arr = o.obs_arr;
            num_obs = o.num_obs;
        }
        return *this;
    }

    ~obstacle_group_t(){
        obs_arr.clear();
    }

};
 struct node {
    uint32_t prev_node_index;
    Vec3_int pos;
    node () {}
    node(const node& n) :
        prev_node_index(n.prev_node_index),
        pos(n.pos)
    {}
    node operator = (const node& n){
        if (this != &n){
            prev_node_index = n.prev_node_index;
            pos = n.pos;
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

        std::vector<node> node_tree;
        uint32_t num_nodes;

        Path (){}

        Path (const Path& p) :
            start(p.start), 
            ends(p.ends),
            num_ends(p.num_ends),
            node_tree(p.node_tree),
            num_nodes(num_nodes)
        {}


        Path operator = (const Path& p){
            if (this != &p){
                start = p.start;
                // ends = p.ends;
                ends = p.ends;
                num_ends = p.num_ends;
                // node_tree = p.node_tree;
                node_tree = p.node_tree;
                num_nodes = p.num_nodes;
            }
            return *this;
        }

        ~Path(){
            ends.clear();
            node_tree.clear();
        }

};

struct path_group_t {
    std::vector<Path> path_arr;
    uint32_t num_paths;

    path_group_t (){}

    ~path_group_t(){
        path_arr.clear();
    }
};

void path_from_netlist(net_group_t * net_list, component_group_t * components);