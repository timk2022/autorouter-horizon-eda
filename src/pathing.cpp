#include "pathing.hpp"
#include "matplotlibcpp.h"


#define PI 3.14159265
#define TAU (2*PI)
#define MAX_ANGLE (UINT16_MAX)

namespace plt = matplotlibcpp;

void plot_path_starts(path_group_t * paths){
    // plt::figure_size(1200,780);
    

    std::vector<uint32_t> x, y;

    std::cout << paths->path_arr.size() << std::endl;
    for (auto i = 0; i < paths->path_arr.size(); i++){
        // x.push_back(paths->path_arr[i].start.pos.x);
        // y.push_back(paths->path_arr[i].start.pos.y);    
        std:: cout << "===================" << std::endl;
            paths->path_arr[i].start.pos.print();
        std:: cout << "===================" << std::endl;
        for (auto j = 0; j < paths->path_arr[i].ends.size(); j++){
            // x.push_back(paths->path_arr[i].ends[j].pos.x);
            // y.push_back(paths->path_arr[i].ends[j].pos.y);    
            // paths->path_arr[i].ends[j].pos.print();
        }
    }    
    plt::figure_size(1200, 780);
    plt::scatter(x,y);
    plt::save("tmp.png");
}


void path_from_netlist(net_group_t * net_list){
    // rumbling 
    path_group_t * paths = new path_group_t;


    paths->path_arr.reserve(4);

    std::vector<int> x, y;

    std::cout << sizeof(Path) <<std::endl;
    // initializing pathing
    for(auto i = 0; i<net_list->nets.size(); i++){
        Path new_path;
        new_path.ends.reserve(2);
        for (auto j = 0; j < net_list->nets[i].linked_conns_arr.size(); j++){
            Vec3_int comp_center = net_list->nets[i].linked_conns_arr[j].comp_pointer->pos_offset;
            int comp_angle = TAU * net_list->nets[i].linked_conns_arr[j].comp_pointer->angle / MAX_ANGLE;

            Vec3_int pad_center = net_list->nets[i].linked_conns_arr[j].pad_offset;

            // x.push_back((comp_center+pad_center).x);
            // y.push_back((comp_center+pad_center).y);

            // rotation matrix
            // Vec3 r_0 = Vec3(cos(comp_angle), -sin(comp_angle),0);
            // Vec3 r_1  = Vec3(sin(comp_angle), cos(comp_angle),0);

            // Vec3_int pad_center_new = Vec3_int(pad_center.dot(r_0), pad_center.dot(r_1), 0) + comp_center;

            Vec3_int pad_center_new = pad_center + comp_center;


            node new_node;
            new_node.pos = pad_center_new;
            new_node.prev_node_index= UINT32_MAX;
            if (j == 0){
                new_path.start = new_node;
                new_path.start.pos.print();
            } else {
                new_path.ends.push_back(new_node);
                new_path.ends[j].pos.print();
            }
        }
        paths->path_arr[i] = new_path;
    }
    // plt::figure_size(1200, 780);
    // plt::scatter(x,y);
    // plt::save("tmp.png");
    plot_path_starts(paths);
}