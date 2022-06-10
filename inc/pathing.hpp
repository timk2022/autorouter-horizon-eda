#pragma once
#include "utils.hpp"

#include <vector>
//todo: add type and component pointer?
class Obstacle {
    public:
        // treating each obstacle as being bounded by a rectangle
        Vec3_int center;
        // todo: add support for nested polygons  (ex: donut)
        polygon vert; 
        Vec3_int side_half_lengths;

        Obstacle(){}

        Obstacle(const Obstacle& o) :
            center(o.center),
            vert(o.vert),
            side_half_lengths(o.side_half_lengths)
        {}
        

        Obstacle operator = (const Obstacle& o){
            if (this != &o){
                vert = o.vert;
                center = o.center;
                side_half_lengths= o.side_half_lengths;
            }            
            return *this;
        }

        // return true if given point is within bounds
        bool intersects(Vec3_int point) {
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
                if (point.x >= x_min && point.x <= x_max && point.y >= y_min && point.y <= y_max){

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