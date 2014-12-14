CCFLAGS = -Wall -Wextra -pedantic -Werror -std=c99 -march=native
OPTI = -fno-asynchronous-unwind-tables -Ofast
DEBUG = -Og -ggdb -fsanitize=address,leak,undefined

.PHONY: run
run: sudoku
	./$< < $<.in

sudoku: sudoku.c
	$(CC) $(CCFLAGS) $(OPTI) -fprofile-generate $< -o $@
	./$@ < $@.in
	$(CC) $(CCFLAGS) $(OPTI) -fprofile-use $< -o $@

.PHONY: stat
stat: sudoku
	perf stat -d -etask-clock -epage-faults -ecycles -einstructions -ealignment-faults ./$< < $<.in

.PHONY: report
report: sudoku
	perf record -e LLC-load-misses ./$< < $<.in
	perf report

.PHONY: debug
debug:
	gcc $(CCFLAGS) $(DEBUG) sudoku.c
	./a.out < sudoku.in
