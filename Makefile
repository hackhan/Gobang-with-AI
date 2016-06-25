# 编译环境 MinGW 4.9.2
# 采用 GNU C++ 11 标准

CXX=g++
SRCS=main.cc ai.cc
OBJS=$(SRCS:.cc=.o)
EXEC=gobang
CXXFLAGS=-c -Wall -std=gnu++11 -g -O0 -DDEBUG \
         -gdwarf-2 -g3

LIBS=-lgraphics -lgdi32 -limm32 -lmsimg32 \
     -lole32 -loleaut32 -lwinmm -luuid

# 源码字符集采用 UTF-8，为了避免 Windows 出现乱码，
# 将可执行文件字符集设置为 GBK
CHARSET=-finput-charset=UTF-8 \
        -fexec-charset=GBK

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CXX) -o $@ $^ $(LIBS) -mwindows
	@echo '********** Compile successfully! **********'

main.o: gobang.h

ai.o: gobang.h chess_library.h

%.o: %.cc
	$(CXX) -o $@ $(CXXFLAGS) $(CHARSET) $<

clean:
	-rm -rf $(OBJS) $(EXEC)
.PHONY: clean

