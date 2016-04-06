#.SUFFIXES:.cc .o

CXX=g++
SRCS=main.cc ai.cc
OBJS=$(SRCS:.cc=.o)
CFLAGS=-c -Wall -std=c++11 -DDEBUG -g -O0
LINK=-lgraphics -lgdi32 -limm32 -lmsimg32 \
	 -lole32 -loleaut32 -lwinmm -luuid
	 
gobang: $(OBJS)
	$(CXX) -o $@ $^ $(LINK) 
	@echo *************** Compile completed! ***************

main.o: main.cc gobang.h
	$(CXX) -o $@ $(CFLAGS) main.cc 
ai.o: ai.cc gobang.h
	$(CXX) -o $@ $(CFLAGS) ai.cc

clean:
	del $(OBJS)

.PHONY: clean