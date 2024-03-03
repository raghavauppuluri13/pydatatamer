CC = clang++
SRCS := $(shell find . -name '*.cpp')
PYTHON_VERSION = 3.9
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    CFLAGS = -g -Werror -Wall -std=c++17 -DMCAP_IMPLEMENTATION -DMCAP_COMPRESSION_NO_ZSTD -DMCAP_COMPRESSION_NO_LZ4  -O3 -fPIC `python3 -m pybind11 --includes` -lpython$(PYTHON_VERSION) -Wextra -pedantic -Werror -shared -I ./include/
    PROG = mcap_tamer.so
endif

$(PROG):$(SRCS)
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS)

clean:
	rm $(PROG)

print:
	@echo $(SRCS)
