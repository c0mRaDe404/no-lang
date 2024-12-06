files := $(wildcard *.c)
#opt_flags :=  -O3 -funroll-all-loops -flto -pipe
flags := -lfl -ggdb

scc: $(files)
	gcc $^ $(opt_flags) $(flags) -o $@
clean:
	rm -f scc

