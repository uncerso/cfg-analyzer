FLAGS = --std=c++17
BUILD_FOLDER = Build
GS_SOURCE = ../../Source/GS/*.cpp
GS_NAME = linearGraphMaker
STATES_SOURCE = ../../Source/States/*.cpp
STATES_NAME = statesMaker
RECOGNIZER_SOURCE = ../../Source/Recognizer/*.cpp
RECOGNIZER_NAME = recognizer

all:
	make GS
	make States
	make Recognizer

GS:
	mkdir -p $(BUILD_FOLDER)
	g++ -O3 $(FLAGS) $(GS_SOURCE) -o $(BUILD_FOLDER)/$(GS_NAME)
GS_debug:
	mkdir -p $(BUILD_FOLDER)
	g++ -g $(FLAGS) $(GS_SOURCE) -o $(BUILD_FOLDER)/$(GS_NAME)
States:
	mkdir -p $(BUILD_FOLDER)
	g++ -O3 $(FLAGS) $(STATES_SOURCE) -o $(BUILD_FOLDER)/$(STATES_NAME)
States_debug:
	mkdir -p $(BUILD_FOLDER)
	g++ -g $(FLAGS) $(STATES_SOURCE) -o $(BUILD_FOLDER)/$(STATES_NAME)
Recognizer:
	mkdir -p $(BUILD_FOLDER)
	g++ -O3 $(FLAGS) $(RECOGNIZER_SOURCE) -o $(BUILD_FOLDER)/$(RECOGNIZER_NAME)
Recognizer_debug:
	mkdir -p $(BUILD_FOLDER)
	g++ -g $(FLAGS) $(RECOGNIZER_SOURCE) -o $(BUILD_FOLDER)/$(RECOGNIZER_NAME)