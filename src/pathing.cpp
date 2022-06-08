#include "pathing.hpp"
#include "matplotlibcpp.h"


#define PI 3.14159265
#define TAU (2*PI)
#define MAX_ANGLE (UINT16_MAX+1)

namespace plt = matplotlibcpp;

void plot_path_starts(path_group_t * paths){
    // plt::figure_size(1200,780);
    
    std::cout << "============== PLOTTING PATHS ===============" << std::endl;

    std::vector<int> x, y;

    for (auto i = paths->path_arr.begin(); i != paths->path_arr.end(); i++){// paths->path_arr.size(); i++){
        x.push_back(i->start.pos.x);
        y.push_back(i->start.pos.y);

        for (auto j = i->ends.begin(); j != i->ends.end(); j++){
            x.push_back(j->pos.x);
            y.push_back(j->pos.y);
    
        }
    }    
    plt::figure_size(1200, 780);
    plt::scatter(x,y);
    // plt::save("tmp2.png");
    plt::show();
}


void path_from_netlist(net_group_t * net_list){
    // rumbling 
    path_group_t * paths = new path_group_t;

    paths->num_paths = net_list->nets.size();
    paths->path_arr.reserve(net_list->nets.size());

    int counter = 0;
    // initializing pathing
    for(auto i = net_list->nets.begin(); i != net_list->nets.end(); i++){
        Path new_path;
        new_path.num_ends = 0;
        new_path.ends.reserve(i->linked_conns_arr.size());
        for (auto j = i->linked_conns_arr.begin(); j != i->linked_conns_arr.end(); j++){
            Vec3_int comp_center = j->comp_pointer->pos_offset; 
            double comp_angle = TAU * j->comp_pointer->angle / MAX_ANGLE;  
            
            Vec3_int pad_center = j->pad_offset;


            // rotation matrix
            Vec3 r_0 = Vec3(cos(comp_angle), -sin(comp_angle),0);
            Vec3 r_1  = Vec3(sin(comp_angle), cos(comp_angle),0);

            std::cout << comp_angle <<std::endl;

            Vec3_int pad_center_new = Vec3_int(pad_center.dot(r_0), pad_center.dot(r_1), 0) + comp_center;
            
            node new_node;
            new_node.pos = pad_center_new;
            new_node.prev_node_index= UINT32_MAX;
            if (j == i->linked_conns_arr.begin()){
                new_path.start = new_node;
            } else {
                new_path.ends.push_back(new_node);
                new_path.num_ends++;
            }
        }
        paths->path_arr.push_back(new_path);
    }
    plot_path_starts(paths);
}