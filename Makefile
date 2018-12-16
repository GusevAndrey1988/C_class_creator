CXX := g++
LIBS := -lstdc++fs

all:
	$(CXX) $(LIBS) C_class_creator.cpp -o C_class_creator.exe
