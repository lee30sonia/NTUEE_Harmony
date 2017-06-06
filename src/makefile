AR			:= ar
ARFLAGS		:= rcv
CXX			:= g++
CPPFLAGS	:= -O2
DBGFLAGS	:= -g -D_DEBUG_ON_
LIBS		:= -Ilib
LIBFLAGS	:= -ltm_usage -Llib


# optimised version
all	: ../bin/net_open_finder
	@echo -n""
../bin/net_open_finder	: main.o greedy.o graph.o circuit.o io.o ../lib/libtm_usage.a
	$(CXX) $^ -o $@

main.o		: main.cpp Circuit.h ../lib/tm_usage.h
	$(CXX) $(CPPFLAGS) $(LIBS) -c $< -o $@

greedy.o	: greedy.cpp Circuit.h
	$(CXX) $(CPPFLAGS) -c $< -o $@

circuit.o	: Circuit.cpp Circuit.h
	$(CXX) $(CPPFLAGS) -c $< -o $@

graph.o	: Graph.cpp Graph.h
	$(CXX) $(CPPFLAGS) -c $< -o $@

io.o		: io.cpp Circuit.h
	$(CXX) $(CPPFLAGS) -c $< -o $@


#debug version
dbg	: ../bin/net_open_finder_dbg
	@echo -n""
../bin/net_open_finder_dbg	: main_dbg.o greedy_dbg.o graph_dbg.o circuit_dbg.o io_dbg.o ../lib
	$(CXX) $^ -o $@ 

main_dbg.o		: main.cpp Circuit.h
	$(CXX) $(DBGFLAGS) $(LIBS) -c $< -o $@

greedy_dbg.o	: greedy.cpp Circuit.h
	$(CXX) $(DBGFLAGS) -c $< -o $@

circuit_dbg.o	: Circuit.cpp Circuit.h
	$(CXX) $(DBGFLAGS) -c $< -o $@

graph_dbg.o	: Graph.cpp Graph.h
	$(CXX) $(DBGFLAGS) -c $< -o $@

io_dbg.o		: io.cpp Circuit.h
	$(CXX) $(DBGFLAGS) -c $< -o $@

#library
../lib: ../lib/libtm_usage.a

../lib/libtm_usage.a: tm_usage.o
	$(AR) $(ARFLAGS) $@ $<
tm_usage.o: ../lib/tm_usage.cpp ../lib/tm_usage.h
	$(CXX) -c $<


clean	:
	rm -rf *.o ../lib/*.a ../lib/*.o ../bin/net_open_finder
