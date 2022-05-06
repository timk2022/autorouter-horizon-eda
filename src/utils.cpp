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