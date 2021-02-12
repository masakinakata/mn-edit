PROGNAME := main
INCDIR := include
SRCDIR := src
LIBDIR := lib
OUTDIR := build
TARGET := $(OUTDIR)/$(PROGNAME)
SRCS := $(wildcard $(SRCDIR)/*.cpp) $(wildcard $(SRCDIR)/$(LIBDIR)/*.cpp)
OBJS := $(addprefix $(OUTDIR)/,$(patsubst %.cpp,%.o,$(SRCS)))
#$(warning $(OBJS))

CC = g++
CFLAGS = -I $(INCDIR)
LDFLAGS = -lncursesw

.PHONY: all clean
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OUTDIR)/%.o:%.cpp
	@if [ ! -e `dirname $@` ]; then mkdir -p `dirname $@`; fi
	$(CC) $(CFLAGS) -o $@ -c $< $(LDFLAGS)

clean:
	rm -rf $(OUTDIR)
