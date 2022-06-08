SRC = ./src
INC = ./inc
OBJ = ./obj
CC = nvcc

# card code reference: https://arnon.dk/matching-sm-architectures-arch-and-gencode-for-various-nvidia-cards/
CFLAGS = -g -std=c++17 -O0 --gpu-architecture=compute_75 --gpu-code=sm_75 
LIBRARIES =  -luuid -lcuda -I ~/Documents/json/include/ -I /usr/include/ -I /usr/include/python3.8 -lpython3.8 -I ~/Documents/matplotlib-cpp#-I ~/Documents/morphologica/include  -I /usr/include/freetype2 -I ~/Documents/morphologica 
# 
SOURCES := $(wildcard $(SRC)/*.c*) 
INCLUDES := $(wildcard $(INC)/*.cuh)
OBJECTS := $(patsubst $(SRC)/%.cpp, $(OBJ)/%.o, $(SOURCES))

$(OBJ)/%.o: $(SRC)/%.cu
	$(CC)  -c $< -o $@ $(CFLAGS) -dc $(LIBRARIES)

$(OBJ)/%.o: $(SRC)/%.cpp
	$(CC)  -c $< -o $@ $(CFLAGS) -I$(INC) $(LIBRARIES)

autorouter: $(OBJECTS) 
	$(CC) -o $@ $^  $(CFLAGS) -I$(INC)  $(LIBRARIES)


.PHONY: clean
# *~ core $(INCDIR)/*~
clean:
	rm -f $(OBJ)/*.o
	rm autorouter