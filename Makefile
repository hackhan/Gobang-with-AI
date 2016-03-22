#.SUFFIXES:.cc .o

CXX=g++
SRCS=main.cc
OBJS=$(SRCS:.cc=.o)
EXEC=gobang
LINK=-lgraphics -lgdi32 -limm32 -lmsimg32 \
	 -lole32 -loleaut32 -lwinmm -luuid
	 
$(EXEC): $(OBJS)
	$(CXX) -o $@ $< -mwindows $(LINK)
	@echo *************** OK ***************

$(OBJS): $(SRCS) gobang.h
	$(CXX) -o $@ -c -Wall -O2 $<

clean:
	del $(OBJS)

.PHONY: clean