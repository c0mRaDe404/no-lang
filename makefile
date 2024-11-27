files := $(wildcard *.c)
flags := -lfl -ggdb

scc: $(files)
	gcc $^ $(flags) -o $@
clean:
	rm -f scc

