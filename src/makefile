AR			:= ar
ARFLAGS		:= rcv
CXX			:= g++ -std=c++11
CPPFLAGS	:= -O2 -fopenmp -lpthread
OMPFLAGS := -fopenmp -lpthread
DBGFLAGS	:= -g -D _DEBUG_ON -fopenmp -lpthread
LIBS		:= -Ilib
LIBFLAGS	:= -ltm_usage -Llib


# optimised version
all	: ../bin/net_open_finder
	@echo -n""
../bin/net_open_finder	: main.o graph.o path.o circuit.o io.o greedy.o ../lib/libtm_usage.a
	$(CXX) $(OMPFLAGS) $^ -o $@

main.o		: main.cpp Circuit.h ../lib/tm_usage.h
	$(CXX) $(CPPFLAGS) $(LIBS) -c $< -o $@

circuit.o	: Circuit.cpp Circuit.h Graph.h
	$(CXX) $(CPPFLAGS) -c $< -o $@

graph.o	: Graph.cpp Graph.h Circuit.h
	$(CXX) $(CPPFLAGS) -c $< -o $@

path.o	: path.cpp Circuit.h
	$(CXX) $(CPPFLAGS) -c $< -o $@

io.o		: io.cpp Circuit.h
	$(CXX) $(CPPFLAGS) -c $< -o $@
	
greedy.o		: Greedy.cpp Circuit.h Graph.h 
	$(CXX) $(CPPFLAGS) -c $< -o $@


#debug version
dbg	: ../bin/net_open_finder_dbg
	@echo -n""
../bin/net_open_finder_dbg	: main_dbg.o graph_dbg.o path_dbg.o circuit_dbg.o io_dbg.o greedy_dbg.o ../lib/libtm_usage.a
	$(CXX) $(OMPFLAGS) $^ -o $@ 

main_dbg.o		: main.cpp Circuit.h ../lib/tm_usage.h
	$(CXX) $(DBGFLAGS) $(LIBS) -c $< -o $@

path_dbg.o	: path.cpp Circuit.h
	$(CXX) $(DBGFLAGS) -c $< -o $@

circuit_dbg.o	: Circuit.cpp Circuit.h
	$(CXX) $(DBGFLAGS) -c $< -o $@

graph_dbg.o	: Graph.cpp Graph.h
	$(CXX) $(DBGFLAGS) -c $< -o $@

io_dbg.o		: io.cpp Circuit.h
	$(CXX) $(DBGFLAGS) -c $< -o $@
	
greedy_dbg.o		: Greedy.cpp Circuit.h Graph.h 
	$(CXX) $(DBGFLAGS) -c $< -o $@

#library
../lib: ../lib/libtm_usage.a

../lib/libtm_usage.a: tm_usage.o
	$(AR) $(ARFLAGS) $@ $<
tm_usage.o: ../lib/tm_usage.cpp ../lib/tm_usage.h
	$(CXX) -c $<


clean	:
	rm -rf *.o ../lib/*.a ../lib/*.o ../bin/net_open_finder ../bin/net_open_finder_dbg ../outputs/*
