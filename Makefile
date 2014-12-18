NAME = sudoku
ARGS = < sudoku.in

# End of project-specific configuration; what follows is generic copy-pasta

MAKEFLAGS += --no-builtin-rules --no-builtin-vars --quiet
CFLAGS += -std=c99 -pedantic -march=native -fstrict-aliasing -fstrict-overflow

OPTI = -Ofast -fno-asynchronous-unwind-tables
DEBUG = -O1 -ggdb

SAN = -fsanitize=address,leak,undefined
ifeq (, $(shell echo | $(CC) $(SAN) -fsyntax-only -xc - 2>&1))
	DEBUG += $(SAN)
endif

ifeq ($(CC), clang)
	PROF = -fprofile-instr
	WALL = everything error
else
	PROF = -fprofile
	OPTI += -fgcse-sm -fgcse-las
	WALL = all extra error
	WALL += bad-function-cast c++-compat cast-align cast-qual conversion
	WALL += disabled-optimization float-equal format=2 init-self inline
	WALL += invalid-pch logical-op long-long missing-format-attribute
	WALL += missing-include-dirs missing-noreturn
	WALL += nested-externs old-style-definition packed padded
	WALL += redundant-decls shadow strict-prototypes switch-default
	WALL += switch-enum unreachable-code unsafe-loop-optimizations unused
	WALL += unused-macros vector-operation-performance write-strings
endif
override CFLAGS := $(foreach W, $(WALL), -W$W) $(CFLAGS)

PERF_EVENTS = task-clock page-faults cycles instructions branch branch-misses

$(NAME): $(NAME).c Makefile
	echo CC $<
	$(CC) $(CFLAGS) $(OPTI) $< -o $@

$(NAME)-debug: $(NAME).c Makefile
	echo CC $<
	$(CC) $(CFLAGS) $(DEBUG) $< -o $@

$(NAME).s: $(NAME).c Makefile
	echo CC $<
	$(CC) $(CFLAGS) $(OPTI) -S $<

.PHONY: run
run: $(NAME)
	./$< $(ARGS)

.PHONY: pgo
pgo: $(NAME).c Makefile
	echo CC -fprofile-generate $<
	$(CC) $(CFLAGS) $(OPTI) $(PROF)-generate $<
	./a.out $(ARGS)
	llvm-profdata merge default.profraw -o $(NAME).gcda || true
	echo CC -fprofile-use $<
	$(CC) $(CFLAGS) $(OPTI) $(PROF)-use=$(NAME).gcda $< -o $(NAME)

.PHONY: debug
debug: $(NAME)-debug
	gdb -q -ex 'br main' -ex 'r $(ARGS)' ./$<

.PHONY: stat
stat: $(NAME)
	echo STAT $<
	perf stat -d $(foreach e, $(PERF_EVENTS), -e$e) ./$< $(ARGS) >/dev/null

.PHONY: report
report: $(NAME)
	perf record -g ./$< $(ARGS)
	perf report
