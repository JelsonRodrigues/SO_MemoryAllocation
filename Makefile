CXX = g++
CXXFLAGS = -fopenmp
LFLAGS = -lX11 -lGL -lpthread -lpng -lstdc++fs -std=c++17 -O3 -mavx2 
WFLAGS = -luser32 -lgdi32 -lopengl32 -lgdiplus -lShlwapi -ldwmapi -lstdc++fs -static -std=c++17 -O3 -mavx2 
WINPATH = src
FILE = main

all: windows run_windows

linux: $(WINPATH)/$(FILE).cpp
	$(CXX) $(CXXFLAGS) $(WINPATH)/$(FILE).cpp $(LFLAGS) -o bin/$(FILE)

windows: $(WINPATH)/$(FILE).cpp
	$(CXX) $(CXXFLAGS) $(WINPATH)/$(FILE).cpp $(WFLAGS) -o bin/$(FILE)

run: bin/$(FILE)
	./bin/$(FILE)

run_windows: bin/$(FILE).exe
	./bin/$(FILE).exe

install:
	sudo apt-get install build-essential libglu1-mesa-dev libpng-dev libmpich12

clean:
	rm ./bin
	rm -rf *.o
