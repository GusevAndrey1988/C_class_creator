CXX := clang++ 
OUT := CC_create

all:
	$(CXX) $(LIBS) C_class_creator.cpp -o $(OUT)
