#pragma once
#include <functional>
#include <string>
#include <vector>

#include "uuid.hpp"

// using json library https://github.com/nlohmann/json
#include <nlohmann/json.hpp>

// typedef struct Component;

class Component;

struct connection_t{
    UUID gate;
    UUID pin;
    UUID comp_id;
    Component * comp_pointer;
    // unsigned long long gate_pin_hashed;
    UUID net;
    // unsigned long long net_hashed;
    // connection_t(UUID gate, UUID pin, UUID comp_id, Component * comp_pointer, UUID net) :
    //     gate(gate), pin(pin), comp_id(comp_id), comp_pointer(comp_pointer), net(net) {}
    
    connection_t() {}

    connection_t(const connection_t& c) :
        gate(c.gate),
        pin(c.pin),
        comp_id(c.comp_id),
        net(c.net)
        {}

    connection_t operator = (const connection_t& c){
        if(this != &c){
            gate = c.gate;
            pin = c.pin;
            comp_id = c.comp_id;
            net = c.net;
        }  
        return *this;
    }
};

class Component{
    public:
        UUID component_id;
        UUID entity_id;
        UUID part_id;
        UUID group;
        UUID tag;

        std::vector<connection_t> conn_arr;

        Component() {}

        Component(const Component& c) :
            component_id(c.component_id),
            entity_id(c.entity_id),
            part_id(c.part_id),
            group(c.group),
            tag(c.tag),
            conn_arr(std::vector<connection_t>(c.conn_arr))
            {}


        Component operator = (const Component& c) {
            if (this != &c){
                component_id = c.component_id;
                entity_id = c.entity_id;
                part_id = c.part_id;
                group = c.group;
                tag = c.tag;
                conn_arr = std::vector<connection_t>(c.conn_arr);
            }
            return *this;
        }

        ~Component(){
            conn_arr.clear();
        }

};

struct component_group_t {
    nlohmann::json j; 
    std::vector<Component> comp_arr;

    component_group_t() {}
    component_group_t(const component_group_t& c) :
        j(j),
        comp_arr(std::vector<Component>(comp_arr))
        {}
    // component_group_t(nlohmann::json j, std::vector<Component> comp_arr, uint32_t comp_arr_len) :
    //     j(j), comp_arr(comp_arr), comp_arr_len(comp_arr_len) {}
};



struct connection_group_t{
    std::vector<connection_t> conn_arr;
    uint32_t conn_arr_len;

    connection_group_t(std::vector<connection_t> conn_arr, uint32_t conn_arr_len) :
       conn_arr(conn_arr), conn_arr_len(conn_arr_len) {} 
};

struct net_t {
    UUID net_id;
    UUID net_class_id;

    std::string net_name;

    enum net_class_t{default_class};
    net_class_t net_class; 

    //todo: add net_class_properties? (i.e. impedance sensitive, RF, digital/analog, etc.)
    bool is_power;
    bool is_used;
    
    std::vector<connection_t> linked_conns_arr;
    uint32_t linked_conns_arr_len;

    net_t () {}

    net_t (UUID net_id, UUID net_class_id, net_class_t net_class, bool is_power,
            bool is_used, std::vector<connection_t> linked_conns_arr, 
            uint32_t linked_conns_arr_len) :
        net_id(net_id), net_class_id(net_class_id), net_class(net_class), is_power(is_power),
        is_used(is_used), linked_conns_arr(linked_conns_arr) {}

    net_t operator = (const net_t& n) {
        if (this != &n){
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
};

struct net_group_t {
    std::vector<net_t> nets;
    uint32_t num_nets;
};








struct component_group_t load_top_block(const std::string& filename);
void print_component_group(struct component_group_t * component_arr);

net_group_t * net_generation(component_group_t * components);