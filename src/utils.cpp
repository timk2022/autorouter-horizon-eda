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
        component_arr->comp_arr[i].pos_offset.print();
        std::cout << "component angle: " << component_arr->comp_arr[i].angle << std::endl;
        std::cout << std::endl;
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
            std::cout << "package id: " << std::string(net_list->nets[i].linked_conns_arr[j].package_id) <<std::endl;
            std::cout << "pad id: " << std::string(net_list->nets[i].linked_conns_arr[j].pad_id) <<std::endl;
            std::cout << "part id: " << std::string(net_list->nets[i].linked_conns_arr[j].comp_pointer->part_id) << std::endl;
            
            std::cout << "pad angle: " << net_list->nets[i].linked_conns_arr[j].pad_angle << std::endl;
            net_list->nets[i].linked_conns_arr[j].pad_offset.print();
        }
    }
    std:: cout << "=================NET LIST DEBUG END===================\n";
}




component_group_t * load_top_block(const std::string& filename){
    json loaded_json = json_load(filename);
    uint32_t num_components = num_components_in_top_block(loaded_json);


    component_group_t * components = new component_group_t;

    components->j = loaded_json;
    components->comp_arr.reserve(num_components);
   
    uint32_t conn_arr_index = 0;
    uint32_t comp_arr_index = 0;

    for(auto comp = loaded_json["components"].begin(); comp != loaded_json["components"].end(); comp++){
        std::string component_id = comp.key();
        uint32_t num_connections = num_conns_in_comp_top_block(loaded_json,component_id);

        Component new_comp;


        new_comp.component_id = str_to_uuid(component_id);        
        new_comp.entity_id = str_to_uuid(comp.value()["entity"]);
        new_comp.part_id = str_to_uuid(comp.value()["part"]);\
        new_comp.group = str_to_uuid(comp.value()["group"]);
        new_comp.tag = str_to_uuid(comp.value()["tag"]); 


        new_comp.conn_arr.reserve(num_connections); //= new_conn_arr;
        conn_arr_index = 0;

        // populate components
        for (auto conn = loaded_json["components"][component_id]["connections"].begin(); 
                  conn != loaded_json["components"][component_id]["connections"].end(); conn++){
            
            connection_t conn_tmp;
            
            std::string gate_uuid_str;
            gate_uuid_str = conn.key().substr(0,conn.key().find("/"));
            std::string pin_uuid_str;
            pin_uuid_str = conn.key().substr(conn.key().find("/" )+1);
            
            conn_tmp.gate = str_to_uuid(gate_uuid_str);
            conn_tmp.pin = str_to_uuid(pin_uuid_str);
            conn_tmp.net = str_to_uuid(conn.value()["net"]);            

            conn_tmp.comp_id = new_comp.component_id;
            conn_tmp.comp_pointer = &(components->comp_arr[comp_arr_index]); 



            new_comp.conn_arr.push_back(conn_tmp);
        }
        components->comp_arr.push_back(new_comp);
        

        
    }
    // get component pointers
    for (auto i = 0; i <components->comp_arr.size(); i++){
        for (auto j = 0; j < components->comp_arr[i].conn_arr.size(); j++){
            components->comp_arr[i].conn_arr[j].comp_pointer = &(components->comp_arr[i]);
        }
    }
    return components;
}

net_group_t * net_generation(component_group_t * components){
    uint32_t num_nets = 0;
    json json_data = components->j;
    // get total number of nets
    for(auto comp = json_data["nets"].begin(); comp != json_data["nets"].end(); comp++){
        num_nets++;
    }

    net_group_t * net_list = new net_group_t;
    net_list->num_nets = num_nets;
    net_list->nets.reserve(num_nets);


    //parse nets 
    for(auto comp = json_data["nets"].begin(); comp != json_data["nets"].end(); comp++){
        net_t tmp_net;
        tmp_net.net_id = str_to_uuid(comp.key());
        tmp_net.net_class_id = str_to_uuid(comp.value()["net_class"]);
        tmp_net.net_name = comp.value()["name"]; 
        tmp_net.is_power = comp.value()["is_power"];        
        net_list->nets.push_back(tmp_net);
    }


    // count number of linked components for memory allocation
    for (auto i = 0; i < net_list->nets.size(); i++){
        uint32_t num_linked_components = 0;
        net_list->nets[i].is_used = false;
        for(auto j = 0; j < components->comp_arr.size(); j++){
            for(auto k = 0; k < components->comp_arr[j].conn_arr.size(); k++){
                if (components->comp_arr[j].conn_arr[k].net == net_list->nets[i].net_id){
                    net_list->nets[i].is_used = true;
                    num_linked_components++;
                }
            }
        }
        net_list->nets[i].linked_conns_arr_len = num_linked_components;
    }

    // start populating nets
    for (auto i = 0; i <net_list->nets.size(); i++){
        net_list->nets[i].linked_conns_arr.reserve(net_list->nets[i].linked_conns_arr_len);

        uint32_t linked_index = 0;
        for(uint32_t j = 0; j < components->comp_arr.size(); j++){
            for(uint32_t k = 0; k < components->comp_arr[j].conn_arr.size(); k++){
                if(components->comp_arr[j].conn_arr[k].net == net_list->nets[i].net_id){
                    // net_list->nets[i].linked_conns_arr[linked_index++] = components->comp_arr[j].conn_arr[k]; 
                    net_list->nets[i].linked_conns_arr.push_back(components->comp_arr[j].conn_arr[k]); 
                    // net_list->nets[i].linked_conns_arr[linked_index++].comp_id = components->comp_arr[j].conn_arr[k].comp_id; 
                    
                }
            }
        }


    }    

    // TODO: make this more portable
    std::string pool_base_path = "pcb-project/autorouter-testing/pool";

    // get pad offsets
    for (auto i = 0; i < net_list->nets.size(); i++){
        for (auto j = 0; j < net_list->nets[i].linked_conns_arr.size(); j++){

            std::string part_filename = pool_base_path + "/parts/cache/" + 
                std::string(net_list->nets[i].linked_conns_arr[j].comp_pointer->part_id) 
                + ".json";
            
            json part_file = json_load(part_filename);
            
            if (part_file.size() == 0){
                std::cout << part_filename << std::endl;

                break;
            }

            std::string package_id_str;
            if (part_file.contains("base") && !part_file.contains("package")){
                std::string base_part_id = part_file["base"];

                std::string base_part_filename = pool_base_path + "/parts/cache/" + base_part_id + ".json";
                // delete &part_file;
                
                part_file = json_load(base_part_filename);
                // part_file = tmp_part_file;

            }

            // pad mapping 
            for (auto pad_map = part_file["pad_map"].begin(); pad_map != part_file["pad_map"].end(); pad_map++){
                if(net_list->nets[i].linked_conns_arr[j].gate == str_to_uuid(pad_map.value()["gate"]) &&
                   net_list->nets[i].linked_conns_arr[j].pin == str_to_uuid(pad_map.value()["pin"])){
                    net_list->nets[i].linked_conns_arr[j].pad_id = str_to_uuid(pad_map.key());
                    break;
                }
            }




            
            package_id_str = std::string(part_file["package"]);

            UUID package_id = str_to_uuid(package_id_str); 
            net_list->nets[i].linked_conns_arr[j].package_id = package_id;

            std::string package_filename = pool_base_path + "/packages/cache/" + 
                package_id_str 
                + "/package.json";
            
            json package_file = json_load(package_filename);

            //TODO: add keepout support 
            // courtyard mapping
            if (package_file.contains("polygons")){
                for (auto polygon_map = package_file["polygons"].begin(); polygon_map != package_file["polygons"].end(); polygon_map++){
                    if (polygon_map.value()["parameter_class"] == "courtyard"){
                        auto comp_pointer= net_list->nets[i].linked_conns_arr[j].comp_pointer;
                        comp_pointer->courtyard.polygon_id = str_to_uuid(polygon_map.key());

                        for(auto vert = polygon_map.value()["vertices"].begin(); vert != polygon_map.value()["vertices"].end(); vert++){
                            if (vert.value()["type"] == "line"){
                                comp_pointer->courtyard.line_type.push_back(polygon::POLYGON_STRAIGHT);
                            } else if (vert.value()["type"] == "curve"){
                                comp_pointer->courtyard.line_type.push_back(polygon::POLYGON_CURVE);
                            }
                            comp_pointer->courtyard.vertices.push_back(std::make_pair(
                                                                        Vec3_int(vert.value()["position"][0],vert.value()["position"][1],0),
                                                                        Vec3_int(vert.value()["arc_center"][0],vert.value()["arc_center"][1],0)
                                                                        ));
                            // todo: add curve support
                        }
                    }
                }
            } else {
                std::cout << "no polygons found in file for part " << net_list->nets[i].linked_conns_arr[j].comp_pointer->part_id << std::endl;
            }

            for (auto pad = package_file["pads"].begin(); pad != package_file["pads"].end(); pad++){
                if(net_list->nets[i].linked_conns_arr[j].pad_id == str_to_uuid(pad.key())){
                    net_list->nets[i].linked_conns_arr[j].pad_angle = (double)pad.value()["placement"]["angle"];
                    net_list->nets[i].linked_conns_arr[j].pad_offset.x = (double)pad.value()["placement"]["shift"][0];
                    net_list->nets[i].linked_conns_arr[j].pad_offset.y = (double)pad.value()["placement"]["shift"][1];
                    net_list->nets[i].linked_conns_arr[j].pad_offset.z = 0; 

                    auto comp_pointer = net_list->nets[i].linked_conns_arr[j].comp_pointer;
                    polygon new_pad;
                    new_pad.polygon_id = str_to_uuid(pad.key());
                    int height = pad.value()["parameter_set"]["pad_height"];
                    int width = pad.value()["parameter_set"]["pad_width"];
                    int curve_radius = pad.value()["parameter_set"]["corner_radius"];

                    
                    // todo: add curve support
                    // adding rectangle                    
                    // todo: adjust for non zero center
                    new_pad.vertices.push_back(std::make_pair(Vec3_int(-width/2, -height/2,0), Vec3_int(0,0,0)));
                    new_pad.line_type.push_back(polygon::POLYGON_STRAIGHT);
                    new_pad.vertices.push_back(std::make_pair(Vec3_int(width/2, -height/2,0), Vec3_int(0,0,0)));
                    new_pad.line_type.push_back(polygon::POLYGON_STRAIGHT);
                    new_pad.vertices.push_back(std::make_pair(Vec3_int(width/2, height/2,0), Vec3_int(0,0,0)));
                    new_pad.line_type.push_back(polygon::POLYGON_STRAIGHT);
                    new_pad.vertices.push_back(std::make_pair(Vec3_int(-width/2, height/2,0), Vec3_int(0,0,0)));
                    new_pad.line_type.push_back(polygon::POLYGON_STRAIGHT);

                    comp_pointer->pads.push_back(new_pad);


                }
            }
        }
    }

    return net_list;
}

void board_load_and_parse(component_group_t * comp_group, const std::string& filename){
    json board_file = json_load(filename);

    // get placed packages
    // TODO: add self placement
    for(auto comp = board_file["packages"].begin(); comp != board_file["packages"].end(); comp++){
        for (uint32_t i = 0; i < comp_group->comp_arr.size(); i++){
            // std::cout << comp.key() << std::endl;
            comp_group->comp_arr[i].is_used = false;
            if (comp_group->comp_arr[i].component_id == str_to_uuid(comp.value()["component"])){
                comp_group->comp_arr[i].board_comp_id = str_to_uuid(comp.key());
                comp_group->comp_arr[i].pos_offset.x = comp.value()["placement"]["shift"][0];
                comp_group->comp_arr[i].pos_offset.y = comp.value()["placement"]["shift"][1];
                comp_group->comp_arr[i].pos_offset.z = 0;
                comp_group->comp_arr[i].angle = comp.value()["placement"]["angle"]; 
                comp_group->comp_arr[i].mirrored = comp.value()["placement"]["mirror"]; 
                comp_group->comp_arr[i].is_used = true;
                comp_group->comp_arr[i].is_fixed = comp.value()["fixed"];
                break;
            }
        }
    }


}