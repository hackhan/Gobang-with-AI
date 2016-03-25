#.SUFFIXES:.cc .o

CXX=g++
SRCS=main.cc ai.cc
OBJS=$(SRCS:.cc=.o)
CFLAGS=-c -Wall -O2 -std=c++11
LINK=-lgraphics -lgdi32 -limm32 -lmsimg32 \
	 -lole32 -loleaut32 -lwinmm -luuid
	 
gobang: $(OBJS)
	$(CXX) -o $@ $^ $(LINK) -mwindows
	@echo *************** OK ***************

#$(OBJS): $(SRCS) gobang.h
#	$(CXX) -o $@ -c -Wall -O2 -std=c++11 $^
main.o: main.cc gobang.h
	$(CXX) -o $@ $(CFLAGS) main.cc 
ai.o: ai.cc gobang.h
	$(CXX) -o $@ $(CFLAGS) ai.cc

clean:
	del $(OBJS)

.PHONY: clean