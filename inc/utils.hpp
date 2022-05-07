#pragma once
#include <functional>
#include <string>

#include "uuid.hpp"

// using json library https://github.com/nlohmann/json
#include <nlohmann/json.hpp>



class Component{
    public:
        UUID component_id;
        // std::hash<unsigned long long> component_id_hashed;

        struct connection{
            UUID gate;
            UUID pin;
            // unsigned long long gate_pin_hashed;
            UUID net;
            // unsigned long long net_hashed;
        };
        struct connection * connections;
        uint32_t num_connections;

        UUID entity_id;
        UUID part_id;
        
        UUID group;
        UUID tag;

        Component(){
            component_id = "";
            connections = nullptr;
            num_connections = 0;
            entity_id = "";
            part_id = "";
            group = "";
            tag = "";
        }
};

struct component_group_t {
    nlohmann::json j; 
    Component * components;
    uint32_t num_components; 
};


struct net_t {
    UUID net_id;
    UUID net_class_id;

    std::string net_name;

    enum net_class_type{default_class};
    //todo: add net_class_properties? (i.e. impedance sensitive, RF, digital/analog, etc.)
    bool is_power;
    Component * linked_components;
    uint32_t num_linked_components;

    bool is_used;

};

struct net_group_t {
    net_t * nets;
    uint32_t num_nets;
};


struct component_group_t * load_top_block(const std::string& filename);
void print_component_group(struct component_group_t * component_arr);

net_group_t * net_generation(component_group_t * components);