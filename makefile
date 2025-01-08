files := $(wildcard *.c)
lib   := $(wildcard ./mem/*.c)
sources := $(files) $(lib)
#opt_flags :=  -O3 -funroll-all-loops -flto -pipe
flags := -lfl -ggdb

scc: $(sources)
	gcc $^ $(opt_flags) $(flags) -o $@
clean:
	rm -f scc

