#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <string>
#include <random>
#include <iostream>
#include <ctime>
#include <iomanip>
// using json library https://github.com/nlohmann/json
#include <nlohmann/json.hpp>

// for convenience
using json = nlohmann::json;

typedef std::mt19937 RNG;
uint32_t seed;
RNG rng;



std::uniform_int_distribution<uint32_t> uint_dist;
void rand_init(void){
    rng.seed(time(0));
}

uint32_t rand_num_gen(void){
    return uint_dist(rng);
}

template <typename T>
std::string int_to_hex(T i){
    std::stringstream stream;
    stream << std::setfill ('0') << std::setw(sizeof(T)) << std::hex << i;
    return stream.str();
}

// generates uuid in the format used by horizon eda
std::string uuid_gen(void){
    uint32_t block_0 = rand_num_gen() & 0xffffffff;
    uint16_t block_1 = rand_num_gen() & 0xffff;
    uint16_t block_2 = rand_num_gen() & 0xffff;
    uint16_t block_3 = rand_num_gen() & 0xffff;
    long block_4 = rand_num_gen() & 0xffffffffffff;
    std::string uuid = int_to_hex(block_0) + "-"
                        + int_to_hex(block_1) + "-"
                        + int_to_hex(block_2) + "-" 
                        + int_to_hex(block_3) + "-" 
                        + int_to_hex(block_4);
    return uuid;
} 

int main(void){
    rand_init();
    uuid_gen();

    std::ifstream i("pcb-project/top_block.json");
    json j;
    i >> j;
}