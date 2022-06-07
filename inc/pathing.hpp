#pragma once
#include "utils.hpp"

class Obstacle {
    public:
        // treating each obstacle as being bounded by a rectangle
        Vec3_int center;
        Vec3_int side_half_lengths;

        Obstacle(){}

        Obstacle(const Obstacle& o) :
            center(o.center),
            side_half_lengths(o.side_half_lengths)
        {}
        

        Obstacle operator = (const Obstacle& o){
            if (this != &o){
                center = o.center;
                side_half_lengths= o.side_half_lengths;
            }            
            return *this;
        }

        // return true if given point is within bounds
        bool intersects(Vec3_int point) {
            if (point.z == center.z){
                if(point.x >= (center.x - side_half_lengths.x) && point.x <= (center.x + side_half_lengths.x)){
                    if(point.y >= (center.y - side_half_lengths.y) && point.y <= (center.y + side_half_lengths.y)){
                        return true;
                    }
                }
            }
            return false;
        }

};

struct obstacle_group_t {
    std::vector<Obstacle> obs_arr;
    uint32_t num_obs;
};

class Path {
    public:
        struct node {
            uint32_t prev_node_index;
            Vec3_int pos;
            node () {}
            node(const node& n) :
                prev_node_index(prev_node_index),
                pos(pos)
            {}
            node operator = (const node& n){
                if (this != &n){
                    prev_node_index = n.prev_node_index;
                    pos = n.pos;
                }
                return *this;
            }
        };

        node start;
        std::vector<node> ends;
        uint32_t num_ends;

        std::vector<node> node_tree;
        uint32_t num_nodes;


};

struct path_group_t {
    std::vector<Path> path_arr;
    uint32_t num_paths;
};