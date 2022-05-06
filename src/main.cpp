#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <random>
#include <iostream>
#include <ctime>
#include <iomanip>
// using json library https://github.com/nlohmann/json
#include <nlohmann/json.hpp>

#include "utils.hpp"

// for convenience
using json = nlohmann::json;




int main(void){
    component_group_t * components; 
    components = load_top_block("pcb-project/autorouter-testing/top_block.json");
    print_component_group(components);
    

    
}