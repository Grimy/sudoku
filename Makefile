NAME = sudoku
EXTRA_FLAGS = -pedantic
ARGS = < sudoku.in

MAKEFLAGS += --no-builtin-rules --no-builtin-vars --quiet
CFLAGS += -xc -std=c99 -fstrict-aliasing -fstrict-overflow -march=native

WALL += error
ifeq ($(CC),clang)
	WALL += everything no-gnu
else
	WALL += all extra
	WALL += bad-function-cast c++-compat cast-align cast-qual conversion
	WALL += disabled-optimization float-equal format=2 init-self inline
	WALL += invalid-pch logical-op long-long missing-include-dirs
	WALL += nested-externs old-style-definition packed padded
	WALL += redundant-decls shadow strict-prototypes switch-default
	WALL += switch-enum unreachable-code unsafe-loop-optimizations unused
	WALL += vector-operation-performance write-strings
	# WALL += -Wsuggest-attribute=pure,const,noreturn,format
endif
CFLAGS += $(foreach W,$(WALL),-W$W) $(EXTRA_FLAGS)

OPTI = -Ofast -fno-asynchronous-unwind-tables

DEBUG = -O1 -ggdb
SAN = -fsanitize=address,leak,undefined
ifeq (, $(shell echo | $(CC) $(SAN) -fsyntax-only -xc - 2>&1))
	DEBUG += $(SAN)
endif

PERF_EVENTS += -etask-clock -epage-faults -ecycles -einstructions -ebranch -ebranch-misses

$(NAME): $(NAME).c Makefile
	echo CC $<
	$(CC) $(CFLAGS) $(OPTI) $< -o $@
	# $(CC) $(CFLAGS) $(OPTI) -fprofile-generate $< -o $@
	# ./$@ $(ARGS)
	# $(CC) $(CFLAGS) $(OPTI) -fprofile-use $< -o $@

$(NAME)-debug: $(NAME).c Makefile
	echo CC $<
	$(CC) $(CFLAGS) $(DEBUG) $< -o $@

$(NAME).s: $(NAME).c Makefile
	echo CC $<
	$(CC) $(CFLAGS) $(OPTI) -S $<

.PHONY: run
run: $(NAME)
	./$< $(ARGS)

.PHONY: debug
debug: $(NAME)-debug
	gdb -q -ex 'br main' -ex 'r $(ARGS)' ./$<

.PHONY: stat
stat: $(NAME)
	echo STAT $<
	perf stat -d $(PERF_EVENTS) ./$< >/dev/null

.PHONY: report
report: $(NAME)
	perf record ./$<
	perf report
