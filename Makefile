CXX = g++
CXXFLAGS = -std=c++11 -O2
MAIN = MyBot
SRCS = C++/MyBot.cpp

# The object files corresponding to the source files
OBJS = $(SRCS:.cpp=.o)

all: build

build: $(MAIN)

$(MAIN): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(MAIN) $(OBJS)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<  -o $@

# Remove program and object files
clean:
	rm -f $(MAIN) $(OBJS) *~
	find . -name '*.hlt' -delete
	find . -name '*.log' -delete

# Run the program
run:
	python3 ./run.py --cmd "./$(MAIN)" --round 1
	python3 ./run.py --cmd "./$(MAIN)" --round 2

run1:
	python3 ./run.py --cmd "./$(MAIN)" --round 1

run2:
	python3 ./run.py --cmd "./$(MAIN)" --round 2

# Phony targets for clean and all to run properly
.PHONY: all clean run
