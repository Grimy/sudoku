#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

// the sparse representation of the binary matrix
static uint64_t auxr[324][9]; // M(r[c][i], c) is a non-zero element
static uint64_t auxc[729][4]; // M(r, c[r][j]) is a non-zero element
static uint8_t sc[324]; // bit 1-7: # allowed choices; bit 8: the constraint has been used or not
static int8_t sr[729];  // sr[r]: # times the row is forbidden by others;
static char grid_string[82];

static void display_grid() {
	char const (*shape)[58] = (char[][58]) {
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
	for (char *r = grid_string; *r; r += 9)
		printf("%s\n║%c┃%c┃%c║%c┃%c┃%c║%c┃%c┃%c║\n", *shape++,
				r[0], r[1], r[2], r[3], r[4], r[5], r[6], r[7], r[8]);
	printf("%s\n", *shape);
}

// generate the sparse representation of the binary matrix
static void init_matrix(void) {
	int8_t nr[324] = {0};

	for (uint64_t r = 0; r < 729; ++r) {
		uint64_t num = r % 9;
		uint64_t col = r / 9 % 9;
		uint64_t row = r / 81;
		auxc[r][0] = 9 * row + col;                    // row-column constraint
		auxc[r][1] = (row/3*3 + col/3) * 9 + num + 81; // box-number constraint
		auxc[r][2] = 9 * row + num + 2*81;             // row-number constraint
		auxc[r][3] = 9 * col + num + 3*81;             // col-number constraint
	}
	for (uint64_t r = 0; r < 729; ++r) // generate r[][] from c[][]
		for (uint64_t c2 = 0; c2 < 4; ++c2)
			auxr[auxc[r][c2]][nr[auxc[r][c2]]++] = r;
}

// update the state vectors when we pick up choice r;
static inline uint64_t sd_update(uint64_t r, int64_t direction) {
	uint64_t min = 10, min_c = 0;

	for (uint64_t c2 = 0; c2 < 4; ++c2)
		sc[auxc[r][c2]] ^= 1 << 7;

	for (uint64_t c2 = 0; c2 < 4; ++c2) { // update # available choices
		uint64_t r2, rr, cc2, c = auxc[r][c2];
		if (direction < 0) {
			const uint64_t *p;
			for (r2 = 0; r2 < 9; ++r2) {
				if (--sr[rr = auxr[c][r2]])
					continue; // update the row status
				p = auxc[rr];
				++sc[p[0]];
				++sc[p[1]];
				++sc[p[2]];
				++sc[p[3]];
			}
		} else {
			for (r2 = 0; r2 < 9; ++r2) {
				if (sr[rr = auxr[c][r2]]++)
					continue; // update the row status
				for (cc2 = 0; cc2 < 4; ++cc2) {
					uint64_t cc = auxc[rr][cc2];
					if (--sc[cc] < min) // update # allowed choices
						min = sc[cc], min_c = cc; // register the minimum number
				}
			}
		}
	}
	return min<<16 | min_c; // return the col that has been modified and with the minimal available choices
}

static uint64_t hints = 0;

static void parse_grid() {
	hints = 0;
	for (uint64_t i = 0; i < 81; ++i) {
		if (grid_string[i] >= '1' && grid_string[i] <= '9') {
			++hints;
			sd_update(i * 9 + (uint64_t) grid_string[i] - '1', 1);
		}
	}
}

// solve a Sudoku; grid_string is the standard dot/number representation
static void solve_grid() {
	int c; // dir=1: forward; dir=-1: backtrack
	int8_t r2;
	uint64_t min, r, i, j;
	int8_t cr[81]; // cr[i]: row chosen at step i
	int16_t cc[81]; // cc[i]: col chosen at step i
	memset(sr, 0, sizeof(sr));
	for (c = 0; c < 324; ++c)
		sc[c] = 9; // 9 allowed choices; no constraint has been used

	parse_grid();
	for (i = 0; i < 81; ++i)
		cr[i] = cc[i] = -1;

	int dir = 1;
	uint64_t cand = 10 << 16;
	for (i = 0; i < 81u - hints;) { // maximum 81-hints steps
		if (dir == 1) {
			min = cand >> 16;
			cc[i] = (int16_t) cand;
			for (c = 0; c < 324 && min > 1; ++c) {
				if (sc[c] < min) {
					min = sc[c];
					cc[i] = (int16_t) c; // choose the top constraint
				}
			}
			if (min == 0 || min == 10)
				cr[i--] = dir = -1; // backtrack
		}
		c = cc[i];
		if (dir == -1 && cr[i] >= 0)
			sd_update(auxr[c][cr[i]], -1); // revert the choice
		// search for the choice to make
		for (r2 = cr[i] + 1; r2 < 9 && sr[auxr[c][r2]]; ++r2);
		if (r2 < 9) {
			cand = sd_update(auxr[c][r2], 1); // set the choice
			cr[i++] = r2;
			dir = 1; // moving forward
		} else {
			cr[i--] = dir = -1; // backtrack
		}
	}
	if (i > 81) // overflow: no solution
		return;

	for (j = 0; j < i; ++j) {
		r = auxr[cc[j]][cr[j]];
		grid_string[r/9] = r % 9 + '1';
	}
}

int main(void) {
	init_matrix();
	while (fgets(grid_string, sizeof(grid_string), stdin) && getchar() == '\n') {
		solve_grid();
		display_grid();
	}
	return 0;
}
