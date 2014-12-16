#include <stdio.h>
#include <string.h>
#include "graph.c"

#define ASSUME_SINGLE_SOLUTION 1
static long grid[81] = {0};
static long stack[81][81] = {{0}};
static long nb = 0;

static char render[] = {
	[511   ] = '.',
	[1 << 0] = '1',
	[1 << 1] = '2',
	[1 << 2] = '3',
	[1 << 3] = '4',
	[1 << 4] = '5',
	[1 << 5] = '6',
	[1 << 6] = '7',
	[1 << 7] = '8',
	[1 << 8] = '9',
};

static int fill_grid() {
	for (unsigned long i = 0; i < 81; ++i) {
		char c = getchar();
		grid[i] = c >= '1' && c <= '9' ? 1 << (c - '1') : 511;
	}
	return getchar() == '\n';
}

static int place_digit(long pos, long digit) {
	const long* const neighbors = graph[pos];
	long mask = 1 << digit;

	memcpy(stack[nb++], grid, sizeof(grid));
	grid[pos] = mask;
	printf("%ld: %ld [nb = %ld]\n", pos, digit, nb);
	for (long i = 0; i < (long) (sizeof(*graph) / sizeof(**graph)); ++i) {
		printf("%ld before: %ld\n", neighbors[i], grid[neighbors[i]]);
		if (!(grid[neighbors[i]] &= ~mask))
			return 0;
		printf("%ld after: %ld\n", neighbors[i], grid[neighbors[i]]);
	}
	return 1;
}

static void remove_digit() {
	memcpy(grid, stack[--nb], sizeof(grid));
}

static int display_grid() {
	static const char* const shape[] = {
		"╔═╤═╤═╦═╤═╤═╦═╤═╤═╗",
		"╟━╋━╋━╫━╋━╋━╫━╋━╋━╢",
		"╟━╋━╋━╫━╋━╋━╫━╋━╋━╢",
		"╠═╪═╪═╬═╪═╪═╬═╪═╪═╣",
		"╟━╋━╋━╫━╋━╋━╫━╋━╋━╢",
		"╟━╋━╋━╫━╋━╋━╫━╋━╋━╢",
		"╠═╪═╪═╬═╪═╪═╬═╪═╪═╣",
		"╟━╋━╋━╫━╋━╋━╫━╋━╋━╢",
		"╟━╋━╋━╫━╋━╋━╫━╋━╋━╢",
		"╚═╧═╧═╩═╧═╧═╩═╧═╧═╝",
	};
	char digits[81] = {0};
	for (unsigned long i = 0; i < sizeof(digits); ++i)
		digits[i] = render[grid[i]];
	char *r = digits;
	for (long row = 0; row < 9; ++row, r += 9)
		printf("%s\n║%c┃%c┃%c║%c┃%c┃%c║%c┃%c┃%c║\n", shape[row],
				r[0], r[1], r[2], r[3], r[4], r[5], r[6], r[7], r[8]);
	printf("%s\n", shape[9]);
	return ASSUME_SINGLE_SOLUTION;
}

/* Tries to fill the grid with a solution . Returns 1 on success,
   0 on failure (requires backtracking) */
int solve_grid(long pos) {
	if (pos == 81)
		return display_grid();
	if (render[grid[pos]] < '0')
		return solve_grid(pos + 1);
	for (long digit = 1;  digit <= 9; ++digit)
		if (place_digit(pos, digit) && solve_grid(pos + 1))
			return 1;
		else
			remove_digit();
	return 0;
}

int main(void) {
	while (fill_grid())
		if (!solve_grid(0) && ASSUME_SINGLE_SOLUTION)
			printf("No solution!\n");
	return 0;
}
