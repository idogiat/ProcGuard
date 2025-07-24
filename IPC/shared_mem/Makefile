CXX = g++
CXXFLAGS = -std=c++17 -pthread -lrt -Wall -Wextra

all: writer reader

writer: writer.cpp
	$(CXX) $(CXXFLAGS) writer.cpp -o writer

reader: reader.cpp
	$(CXX) $(CXXFLAGS) reader.cpp -o reader

clean:
	rm -f writer reader
