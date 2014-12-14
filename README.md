A simple sudoku solver. Reads a grid from standard input and prints the
solution on standard output.

The input should match `^[1-9:]{81}` (81 digits or spaces). Each character
represent the content of one cell of the grid, in Z-order; colon means “empty”.
See the sudoku.in file for an example.

The output is pretty-printed using UTF-8 box-drawing characters. When using
sudoku.in as the input file, it should look like this:

    ╔═╤═╤═╦═╤═╤═╦═╤═╤═╗
    ║1┃2┃4║3┃8┃6║9┃7┃5║
    ╟━╋━╋━╫━╋━╋━╫━╋━╋━╢
    ║9┃5┃3║7┃1┃2║8┃4┃6║
    ╟━╋━╋━╫━╋━╋━╫━╋━╋━╢
    ║7┃6┃8║5┃4┃9║1┃2┃3║
    ╠═╪═╪═╬═╪═╪═╬═╪═╪═╣
    ║6┃7┃5║2┃9┃4║3┃8┃1║
    ╟━╋━╋━╫━╋━╋━╫━╋━╋━╢
    ║4┃3┃9║8┃5┃1║7┃6┃2║
    ╟━╋━╋━╫━╋━╋━╫━╋━╋━╢
    ║8┃1┃2║6┃7┃3║5┃9┃4║
    ╠═╪═╪═╬═╪═╪═╬═╪═╪═╣
    ║3┃9┃1║4┃6┃7║2┃5┃8║
    ╟━╋━╋━╫━╋━╋━╫━╋━╋━╢
    ║2┃8┃6║9┃3┃5║4┃1┃7║
    ╟━╋━╋━╫━╋━╋━╫━╋━╋━╢
    ║5┃4┃7║1┃2┃8║6┃3┃9║
    ╚═╧═╧═╩═╧═╧═╩═╧═╧═╝

If it doesn’t display correctly on your computer, either your software or your
fonts don’t properly support UTF-8 (*e.g.* Courier has issues with box-drawing
characters).

To compile and run the code with the test input sudoku.in, simply run:

    make

