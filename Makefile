# 编译环境 MinGW 4.9.2，编译器选择 g++
# 采用 C++ 11 标准

CXX=g++
SRCS=main.cc ai.cc
OBJS=$(SRCS:.cc=.o)

CFLAGS=-c -Wall -std=c++11 -O2 

LINK=-lgraphics -lgdi32 -limm32 -lmsimg32 \
	 -lole32 -loleaut32 -lwinmm -luuid

# 源码字符集采用 UTF-8，为了避免 Windows 出现乱码，
# 将可执行文件字符集设置为 GBK
CHARSET=-finput-charset=UTF-8 \
		-fexec-charset=GBK

gobang: $(OBJS)
	$(CXX) -o $@ $^ $(LINK) -mwindows
	@echo *************** Compile successfully! ***************

%.o: %.cc gobang.h
	$(CXX) -o $@ $(CFLAGS) $(CHARSET) $<

clean:
	del $(OBJS)

.PHONY: clean