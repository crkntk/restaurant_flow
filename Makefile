#Name: Carlos Reyes REDID: 131068259
# CXX Make variable for compiler
CXX=g++
# -std=c++11  C/C++ variant to use, e.g. C++ 2011
# -Wall       show the necessary warning files
# -g3         include information for symbolic debugger e.g. gdb 
CXXFLAGS=-std=c++11 -Wall -g3 -pthread -c

# object files
OBJS = log.o main.o consumer.o monitor.o producer.o

# Program name
PROGRAM = dineseating

# Rules format:
# target : dependency1 dependency2 ... dependencyN
#     Command to make target, uses default rules if not specified

# First target is the one executed if you just type make
# make target specifies a specific target
# $^ is an example of a special variable.  It substitutes all dependencies
$(PROGRAM) : $(OBJS)
	$(CXX) -o $(PROGRAM) $^ -pthread



producer.o : seating.h monitor.h producer.h producer.cpp
	$(CXX) $(CXXFLAGS) producer.cpp

consumer.o : seating.h monitor.h consumer.h consumer.cpp
	$(CXX) $(CXXFLAGS) consumer.cpp

monitor.o : seating.h monitor.h monitor.cpp
	$(CXX) $(CXXFLAGS) monitor.cpp

seating.o : seating.h
	$(CXX) $(CXXFLAGS)

log.o : log.h log.cpp
	$(CXX) $(CXXFLAGS) log.cpp

main.o : monitor.h producer.h consumer.h main.cpp
	$(CXX) $(CXXFLAGS) main.cpp


	
clean :
	rm -f *.o $(PROGRAM)