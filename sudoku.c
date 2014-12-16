#include <stdio.h>
#include <assert.h>
#include <string.h>

#define ASSUME_SINGLE_SOLUTION 1
static long grid[81] = {0};
static long predef[81] = {0};
static long graph[81][21] = {{0}};
static long gcounts[81] = {0};

#define MANGLE(x) ((x) & ((x) >> 9) & ((x) >> 18))
#define SPREAD(x) ((x) | ((x) << 9) | ((x) << 18))
#define NEIGHBOR(p) (grid[*p & 255])
#define SHIFT(p) (*p >> 8)

static void blah(long pos, long i, long shift) {
	if (predef[pos])
		grid[i] &= ~(MANGLE(grid[pos]) << shift);
	if (pos > i) {
		graph[i][gcounts[i]++] = pos + (shift << 8);
		graph[i][gcounts[i]] = -1;
	}
}

static void fill_cell(long pos, long c) {
	if ((predef[pos] = c >= '1' && c <= '9'))
		grid[pos] = SPREAD(1 << (c - '1'));
	long col = pos % 9, row = pos / 9, region = row / 3 * 3 + col / 3;
	for (long i = 0; i < 81; i++) {
		if (i == pos)
			continue;
		long icol = i % 9, irow = i / 9, iregion = irow / 3 * 3 + icol / 3;
		if (icol == col)
			blah(pos, i, 0);
		else if (irow == row)
			blah(pos, i, 9);
		else if (iregion == region)
			blah(pos, i, 18);
	}
}

static int fill_grid() {
	memset(graph, 255, sizeof(graph));
	memset(gcounts, 0, sizeof(gcounts));
	for (long pos = 0; pos < 81; ++pos)
		grid[pos] = SPREAD(511l);
	for (long pos = 0; pos < 81; ++pos)
		fill_cell(pos, getchar());
	return getchar() == '\n';
}

static int display_grid() {
	char digits[82] = {0};
	for (long i = 0; i < 81; ++i)
		digits[i] = '1' + __builtin_ctz(MANGLE(grid[i]));
	printf("%s\n", digits);
	fflush(stdout);
	return ASSUME_SINGLE_SOLUTION;
}

static long place_digit(long pos, long mask) {
	grid[pos] = SPREAD(mask);
	for (long *p = graph[pos]; *p >= 0; ++p)
		if (!MANGLE(NEIGHBOR(p) & ~(mask << SHIFT(p))))
			return 0;
	for (long *p = graph[pos]; *p >= 0; ++p)
		NEIGHBOR(p) &= ~(mask << SHIFT(p));
	return 1;
}

static void remove_digit(long pos, long mask) {
	for (long *p = graph[pos]; *p >= 0; ++p)
		NEIGHBOR(p) |= (mask << SHIFT(p));
}

/* Tries to fill the grid with a solution . Returns 1 on success,
   0 on failure (requires backtracking) */
int solve_grid(long pos) {
	if (pos == 81)
		return display_grid();
	if (predef[pos])
		return solve_grid(pos + 1);
	long save = grid[pos];
	long val = MANGLE(save);
	while (val) {
		long mask = val & -val;
		val &= ~mask;
		if (place_digit(pos, mask)) {
			if (solve_grid(pos + 1))
				return 1;
			remove_digit(pos, mask);
		}
	}
	grid[pos] = save;
	return 0;
}

int main(void) {
	while (fill_grid())
		if (!solve_grid(0) && ASSUME_SINGLE_SOLUTION)
			printf("No solution!\n");
	return 0;
}
