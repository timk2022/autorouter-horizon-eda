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
    for (int i = 0; i < component_arr->comp_arr.size(); i++){
        std:: cout << "================= component number: " << i <<" ===================\n";
        std::cout << "component id: " << std::string(component_arr->comp_arr[i].component_id) << std::endl;
        std::cout << "part id: " << std::string(component_arr->comp_arr[i].part_id) << std::endl;
        std::cout << "entity id: " << std::string(component_arr->comp_arr[i].entity_id) << std::endl;
        std::cout << "group id: " << std::string(component_arr->comp_arr[i].group) << std::endl;
        std::cout << "tag id: " << std::string(component_arr->comp_arr[i].tag) << std::endl;
        
        for (int j = 0; j < component_arr->comp_arr[i].conn_arr.size(); j++){
            std:: cout << "----------------- connection number: " << j <<" ----------------------\n";
            std::cout << "conn gate: " << std::string(component_arr->comp_arr[i].conn_arr[j].gate) << std::endl;
            std::cout << "conn pin: " << std::string(component_arr->comp_arr[i].conn_arr[j].pin) << std::endl;
            std::cout << "conn net: " << std::string(component_arr->comp_arr[i].conn_arr[j].net) << std::endl;
        } 
    }
}

void print_net_list(struct net_group_t * net_list){
    std:: cout << "=================NET LIST DEBUG OUTPUT===================\n";
    for(uint32_t i = 0; i < net_list->nets.size();i++){
        std::cout << "================= net number: " << i <<" ===================\n";
        std::cout << "net id: " << std::string(net_list->nets[i].net_id) << std::endl; 
        for (uint32_t j = 0; j < net_list->nets[i].linked_conns_arr.size(); j++){
            std::cout << "----------------- linked component number: " << j << "-------------------\n";   
            std::cout << "component id: " << std::string(net_list->nets[i].linked_conns_arr[j].comp_id) << std::endl;
            std::cout << "net id: " << std::string(net_list->nets[i].linked_conns_arr[j].net) << std::endl;
            std::cout << "gate id: " << std::string(net_list->nets[i].linked_conns_arr[j].gate) << std::endl;
            std::cout << "pin id: " << std::string(net_list->nets[i].linked_conns_arr[j].pin) << std::endl;
            
            std::cout << "part id: " << std::string(net_list->nets[i].linked_conns_arr[j].comp_pointer->part_id) << std::endl;

        }
    }
}


component_group_t load_top_block(const std::string& filename){
    json loaded_json = json_load(filename);
    uint32_t num_components = num_components_in_top_block(loaded_json);


    component_group_t components;// = (component_group_t *)malloc(sizeof(component_group_t));

    components.j = loaded_json;
    // components.comp_arr_len = num_components;
    std::vector<Component> comp_arr_init (num_components);
    components.comp_arr = comp_arr_init;
   
    uint32_t conn_arr_index = 0;
    uint32_t comp_arr_index = 0;

    for(auto comp = loaded_json["components"].begin(); comp != loaded_json["components"].end(); comp++){
        std::string component_id = comp.key();
        uint32_t num_connections = num_conns_in_comp_top_block(loaded_json,component_id);

        Component new_comp;


        new_comp.component_id = str_to_uuid(component_id);        
        new_comp.entity_id = str_to_uuid(comp.value()["entity"]);
        new_comp.part_id = str_to_uuid(comp.value()["part"]);
        new_comp.group = str_to_uuid(comp.value()["group"]);
        new_comp.tag = str_to_uuid(comp.value()["tag"]); 

        std::vector<connection_t> new_conn_arr(num_connections);

        new_comp.conn_arr = new_conn_arr;
        conn_arr_index = 0;

        // populate components
        for (auto conn = loaded_json["components"][component_id]["connections"].begin(); 
                  conn != loaded_json["components"][component_id]["connections"].end(); conn++){
            
            connection_t conn_tmp;
            
            std::string gate_uuid_str;
            gate_uuid_str = conn.key().substr(0,conn.key().find("/"));
            std::string pin_uuid_str;
            pin_uuid_str = conn.key().substr(conn.key().find("/")+1);
            
            conn_tmp.gate = str_to_uuid(gate_uuid_str);
            conn_tmp.pin = str_to_uuid(pin_uuid_str);
            conn_tmp.net = str_to_uuid(conn.value()["net"]);            

            conn_tmp.comp_id = new_comp.component_id;
            conn_tmp.comp_pointer = &(components.comp_arr[comp_arr_index]); 



            new_comp.conn_arr[conn_arr_index++] = conn_tmp;
        }
        components.comp_arr[comp_arr_index++] = new_comp;

        
    }
    // get component pointers
    for (uint32_t i = 0; i <components.comp_arr.size(); i++){
        for (uint32_t j = 0; j < components.comp_arr[i].conn_arr.size(); j++){
            components.comp_arr[i].conn_arr[j].comp_pointer = &(components.comp_arr[i]);
        }
    }
    return components;
}

net_group_t net_generation(component_group_t * components){
    uint32_t num_nets = 0;
    json json_data = components->j;
    // get total number of nets
    for(auto comp = json_data["nets"].begin(); comp != json_data["nets"].end(); comp++){
        num_nets++;
    }

    net_group_t net_list;
    net_list.num_nets = num_nets;
    std::vector<net_t> net_arr_init (num_nets);
    net_list.nets = net_arr_init;

    net_list.nets = std::vector<net_t> (num_nets);

    uint32_t net_index = 0;
    //parse nets 
    for(auto comp = json_data["nets"].begin(); comp != json_data["nets"].end(); comp++){
        net_list.nets[net_index].net_id = str_to_uuid(comp.key());
        net_list.nets[net_index].net_class_id = str_to_uuid(comp.value()["net_class"]);
        net_list.nets[net_index].net_name = comp.value()["name"]; 
        net_list.nets[net_index].is_power = comp.value()["is_power"];        

        net_index++;
    }
    net_index = 0;


    // count number of linked components for memory allocation
    for (uint32_t i = 0; i < net_list.nets.size(); i++){
        bool net_used = false;
        uint32_t num_linked_components = 0;
        for(uint32_t j = 0; j < components->comp_arr.size(); j++){
            for(uint32_t k = 0; k < components->comp_arr[j].conn_arr.size(); k++){
                if (components->comp_arr[j].conn_arr[k].net == net_list.nets[i].net_id){
                    net_used = true;
                    num_linked_components++;
                }
            }
        }
        net_list.nets[i].is_used = net_used;
        net_list.nets[i].linked_conns_arr_len = num_linked_components;
    }

    // start populating nets
    for (uint32_t i = 0; i <net_list.nets.size(); i++){
        std::vector<connection_t> tmp_conn_arr (net_list.nets[i].linked_conns_arr_len);
        net_list.nets[i].linked_conns_arr = tmp_conn_arr;

        uint32_t linked_index = 0;
        for(uint32_t j = 0; j < components->comp_arr.size(); j++){
            for(uint32_t k = 0; k < components->comp_arr[j].conn_arr.size(); k++){
                if(components->comp_arr[j].conn_arr[k].net == net_list.nets[i].net_id){
                    net_list.nets[i].linked_conns_arr[linked_index++] = components->comp_arr[j].conn_arr[k]; 
                    // net_list.nets[i].linked_conns_arr[linked_index++].comp_id = components->comp_arr[j].conn_arr[k].comp_id; 
                    
                }
            }
        }


    }    

    std::string pool_base_path = "pcb-project/autorouter-testing/pool/";

    // get pad offsets
    for (uint32_t i = 0; i < net_list.nets.size(); i++){
        for (uint32_t j = 0; j < net_list.nets[i].linked_conns_arr.size(); j++){

            std::string part_filename = pool_base_path + "/parts/cache/" + 
                std::string(net_list.nets[i].linked_conns_arr[j].comp_pointer->part_id) 
                + "/package.json";
            
            json part_file = json_load(part_filename);

            UUID package_id = str_to_uuid(part_file["package"]); 


            std::string package_filename = pool_base_path + "/packages/cache/" + 
                std::string(net_list.nets[i].linked_conns_arr[j].comp_pointer->part_id) 
                + "/package.json";
            
            json package_file = json_load(part_filename);


        }
    }




    return net_list;
}