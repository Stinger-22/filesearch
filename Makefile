CXX = g++
CXXFLAGS = -Wall -std=c++17

fsapp: build/fsapp.o fslib
	@echo "Compiling filesearch app which uses static library."
	$(CXX) $(CXXFLAGS) -o fsapp build/fsapp.o filesearcher.a

fslib: build/filesearcher.o
	@echo "Compiling static library."
	ar rvs filesearcher.a build/filesearcher.o

build/filesearcher.o: src/filesearcher.cpp src/filesearcher.h
	$(CXX) -c src/filesearcher.cpp -o build/filesearcher.o

build/fsapp.o: src/fsapp.cpp
	$(CXX) -c src/fsapp.cpp -o build/fsapp.o
