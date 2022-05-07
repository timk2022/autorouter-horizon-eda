#include "utils.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>
#include <random>
#include <iostream>
#include <ctime>
#include <iomanip>

// for convenience
using json = nlohmann::json;

inline bool file_exists(const std::string& filename)
{
  return(access(filename.c_str(), F_OK) != -1);
}

json json_load(const std::string& filename){
    if(file_exists(filename)){
        std::ifstream i(filename.c_str());
        json j;
        i >> j;
        return j;
    }
    std::cout << filename << std::endl; 
    return nullptr;

}

uint32_t num_conns_in_comp_top_block(json j, std::string& component_id){
    uint32_t count = 0;
    for (auto conn = j["components"][component_id]["connections"].begin(); 
                conn != j["components"][component_id]["connections"].end(); conn++){
        count++;
    }
    return count;
}

uint32_t num_components_in_top_block(json j){
    uint32_t count = 0;
    
    for(auto comp = j["components"].begin(); comp != j["components"].end(); comp++){
        count++;
    }
    return count;
}

void print_component_group(struct component_group_t * component_arr){
    std:: cout << "=================COMPONENT GROUP DEBUG OUTPUT===================\n";
    for (int i = 0; i < component_arr->num_components; i++){
        std:: cout << "================= component number: " << i <<" ===================\n";
        std::cout << "component id: " << std::string(component_arr->components[i].component_id) << std::endl;
        std::cout << "part id: " << std::string(component_arr->components[i].part_id) << std::endl;
        std::cout << "entity id: " << std::string(component_arr->components[i].entity_id) << std::endl;
        std::cout << "group id: " << std::string(component_arr->components[i].group) << std::endl;
        std::cout << "tag id: " << std::string(component_arr->components[i].tag) << std::endl;
        
        for (int j = 0; j < component_arr->components[i].num_connections; j++){
            std:: cout << "----------------- connection number: " << j <<" ----------------------\n";
            std::cout << "conn gate: " << std::string(component_arr->components[i].connections[j].gate) << std::endl;
            std::cout << "conn pin: " << std::string(component_arr->components[i].connections[j].pin) << std::endl;
            std::cout << "conn net: " << std::string(component_arr->components[i].connections[j].net) << std::endl;
        } 
    }
}


component_group_t * load_top_block(const std::string& filename){
    json j = json_load(filename);
    struct component_group_t * component_arr;
    uint32_t num_components = num_components_in_top_block(j);

    component_arr = (struct component_group_t *)malloc(sizeof(struct component_group_t));

    component_arr->components = (Component *)malloc(sizeof(Component) * num_components);
    component_arr->num_components = num_components;
    component_arr->j = j;

    uint32_t component_index = 0;
    uint32_t connection_index = 0;
    UUID uu;
    
    Component::connection * conns;
    
    for(auto comp = j["components"].begin(); comp != j["components"].end(); comp++){
        std::string component_id = comp.key();
        uint32_t num_connections = num_conns_in_comp_top_block(j,component_id);

        uu.to_uuid(component_id);
        component_arr->components[component_index].component_id = uu;
        
        uu.to_uuid(comp.value()["entity"]);
        component_arr->components[component_index].entity_id = uu;
        uu.to_uuid(comp.value()["part"]);
        component_arr->components[component_index].part_id = uu;
        
        uu.to_uuid(comp.value()["group"]);
        component_arr->components[component_index].group = uu;
        uu.to_uuid(comp.value()["tag"]);
        component_arr->components[component_index].tag = uu;
    
        conns = (Component::connection *)malloc(sizeof(Component::connection)* num_connections);
        component_arr->components[component_index].connections = conns;
        component_arr->components[component_index].num_connections = num_connections;

        for (auto conn = j["components"][component_id]["connections"].begin(); 
                  conn != j["components"][component_id]["connections"].end(); conn++){
            std::string gate_uuid_str;
            gate_uuid_str = conn.key().substr(0,conn.key().find("/"));
            std::string pin_uuid_str;
            pin_uuid_str = conn.key().substr(conn.key().find("/")+1);

            uu.to_uuid(gate_uuid_str);
            conns[connection_index].gate = uu;
            uu.to_uuid(pin_uuid_str);
            conns[connection_index].pin = uu;
            uu.to_uuid(conn.value()["net"]);
            conns[connection_index].net = uu;
            connection_index++;
        }
        connection_index = 0;
        component_index++;
    }
    return component_arr;
}


net_group_t * get_all_nets(component_group_t * components){
    uint32_t total = 0;
    json j = components->j;
    // get total number of nets
    for(auto comp = j["nets"].begin(); comp != j["nets"].end(); comp++){
        total++;
    }
    
    // allocate nets
    net_group_t * all_nets = (net_group_t *)malloc(sizeof(net_group_t)); 
    all_nets->nets = (net_t *)malloc(sizeof(net_t)*total);
    all_nets->num_nets = total;
    
    UUID uu_tmp;
    uint32_t index = 0;
    for(auto comp = j["nets"].begin(); comp != j["nets"].end(); comp++){
        std::string tmp_str = comp.key();
        uu_tmp.to_uuid(tmp_str);
        all_nets->nets[index].net_id = uu_tmp;

        tmp_str = comp.value()["net_class"];
        uu_tmp.to_uuid(tmp_str);
        all_nets->nets[index].net_class_id = uu_tmp;

        //fixme: segfaulting around here, likely memory allocation error
        tmp_str = comp.value()["name"];
        all_nets->nets[index].net_name = tmp_str;
        
        all_nets->nets[index].is_power = comp.value()["is_power"];

        index++;
    }
    
    for (uint32_t i = 0; i < all_nets->num_nets; i++){
        bool net_used = false;
        uint32_t num_linked_components = 0;
        for(uint32_t j = 0; j < components->num_components; j++){
            for(uint32_t k = 0; k < components->components[j].num_connections; k++){
                if (components->components[j].connections[k].net == all_nets->nets[i].net_id){
                    net_used = true;
                    num_linked_components++;
                }
            }
        }
        all_nets->nets[i].is_used = net_used;
        all_nets->nets[i].num_linked_components = num_linked_components;
    }

    for(uint32_t i = 0; i < all_nets->num_nets; i++){
        all_nets->nets[i].linked_components = (Component *)malloc(sizeof(Component)*all_nets->nets[i].num_linked_components);
        uint32_t linked_index = 0;
        for(uint32_t j = 0; j < components->num_components; j++){
            for(uint32_t k = 0; k < components->components[j].num_connections; k++){
                if (components->components[j].connections[k].net == all_nets->nets[i].net_id){
                    std::copy(
                        &(components->components[j]),
                        &(components->components[j]) + sizeof(Component),
                        &(all_nets->nets[i].linked_components[linked_index])
                    );
                    all_nets->nets[i].linked_components[linked_index].num_connections = 0;
                    std::copy(
                        &(components->components[j].connections[k]),
                        &(components->components[j].connections[k]) + sizeof(Component::connection),
                        &(all_nets->nets[i].linked_components[linked_index].connections[0])
                    );
                    linked_index++;
                }
            }
        }
    }
    return all_nets;
}


net_group_t * net_generation(component_group_t * components){
    net_group_t * nets = (net_group_t *)malloc(sizeof(net_group_t));
    net_group_t * all_nets = get_all_nets(components); 

    for (uint32_t i = 0; i<all_nets->num_nets; i++ ){
        // check if net is used
        // if (all_nets.nets[i].is_used){
        //     for (uint32_t j = 0; j < components->num_components)
        // }        
    }



}