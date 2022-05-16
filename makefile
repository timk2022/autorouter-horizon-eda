SRC = ./src
INC = ./inc
OBJ = ./obj
CC = nvcc

# card code reference: https://arnon.dk/matching-sm-architectures-arch-and-gencode-for-various-nvidia-cards/
CFLAGS = -g -std=c++11  -O0 --gpu-architecture=compute_75 --gpu-code=sm_75
LIBRARIES =  -luuid -lcuda -I ~/Documents/json/include/

SOURCES := $(wildcard $(SRC)/*.c*) 
INCLUDES := $(wildcard $(INC)/*.cuh)
OBJECTS := $(patsubst $(SRC)/%.cu, $(OBJ)/%.o, $(SOURCES))

$(OBJ)/%.o: $(SRC)/%.cu
	$(CC)  -c $< -o $@ $(CFLAGS) -dc $(LIBRARIES)

$(OBJ)/%.o: $(SRC)/%.cpp
	$(CC)  -c $< -o $@ $(CFLAGS) $(LIBRARIES)

autorouter: $(OBJECTS) 
	$(CC) -o $@ $^  $(CFLAGS) -I$(INC)  $(LIBRARIES)


.PHONY: clean
# *~ core $(INCDIR)/*~
clean:
	rm -f $(OBJ)/*.o
	rm autorouter