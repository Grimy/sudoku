#include <stdio.h>
#include "graph.c"

static char grid[82] = {0};

/* Returns 1 if the grid is in a valid state, 0 otherwise */
static int check_grid(long pos) {
	const long* const neighbors = graph[pos];
	for (long i = 0; i < (long) (sizeof(*graph) / sizeof(**graph)); ++i)
		if (grid[neighbors[i]] == grid[pos])
			return 0;
	return 1;
}

/* Tries to fill the grid with a solution . Returns 1 on success,
   0 on failure (requires backtracking) */
static int solve_grid(long pos) {
	if (grid[pos] != ':')
		return grid[pos] ? solve_grid(pos + 1) : 1;
	for (grid[pos] = '1';  grid[pos] <= '9'; ++grid[pos])
		if (check_grid(pos) && solve_grid(pos + 1))
			return 1;
	return 0;
}

static void display_grid() {
	static const char* const formats[] = {
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
	char *r = grid;
	for (long row = 0; row < 9; ++row, r += 9)
		printf("%s\n║%c┃%c┃%c║%c┃%c┃%c║%c┃%c┃%c║\n", formats[row],
			r[0], r[1], r[2], r[3], r[4], r[5], r[6], r[7], r[8]);
	printf("%s\n", formats[9]);
}

int main(void) {
	fgets(grid, sizeof(grid), stdin);
	int solved = solve_grid(0);
	if (solved)
		display_grid();
	else
		printf("No solution!\n");
	return !solved;
}
