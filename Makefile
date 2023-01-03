TARGET = RemoveCramTags

CXXFLAGS += -Wall -std=c++14 -fno-common -dynamic -fwrapv -O3 -DNDEBUG -pipe
LDLIBS += -lhts

# try and add conda environment
ifdef CONDA_PREFIX
    CPPFLAGS+=-I$(CONDA_PREFIX)/include
    LDFLAGS+=-L$(CONDA_PREFIX)/lib
endif
#
## Options to use target htslib or skia
#HTSLIB ?= ""
#ifneq ($(HTSLIB),"")
#    CPPFLAGS += -I$(HTSLIB)
#    LDFLAGS += -L$(HTSLIB)
#endif

.PHONY: default all debug clean

default: $(TARGET)

all: default
debug: default

OBJECTS = $(patsubst %.cpp, %.o, $(wildcard ./*.cpp))

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -g $(CPPFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CXX) -g $(OBJECTS) $(LDFLAGS) $(LDLIBS) -o $@

clean:
	-rm -f *.o ./src/*.o ./src/*.o.tmp
	-rm -f $(TARGET)
