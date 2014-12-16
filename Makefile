CCFLAGS = -Wall -Wextra -Werror -Wno-override-init -std=gnu99 -march=native
OPTI = -fno-asynchronous-unwind-tables -Ofast
DEBUG = -O0 -ggdb

.PHONY: run
run: sudoku
	./$< < $<.in

sudoku: sudoku.c
	$(CC) $(CCFLAGS) $(OPTI) $< -o $@

a.out: sudoku.c
	gcc $(CCFLAGS) $(DEBUG) $<

.PHONY: stat
stat: sudoku
	perf stat -d -etask-clock -epage-faults -ecycles -einstructions -ebranch -ebranch-misses ./$< < $<.in | tee $<.out
	cmp $<.out answers

.PHONY: report
report: a.out
	perf record -g -einstructions ./$< < sudoku.in
	perf report

.PHONY: debug
debug: a.out
	gdb ./a.out
