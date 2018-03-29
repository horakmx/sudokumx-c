# sudokumx-c
Sudoku solver in c

Initial ideas behind this work
- build original solution without inspiration from other sources 
- make this program to be fast as possible

After comparison to other solvers by different authors I found (what a surprise) that fasters solvers exist.

You can see some of the fastest solutions here: [https://github.com/horakmx/plb/tree/master/sudoku/incoming](https://github.com/horakmx/plb/tree/master/sudoku/incoming) (Fork from [Attractivechaos repo](https://github.com/attractivechaos/plb)).

Fastest solution I can find is [fsss2](https://github.com/horakmx/fsss2) (Fork from [Dobrichev's repo]( https://github.com/dobrichev/fsss2))
- fsss2 is about 2x (for hardest known sudokus) faster than sudokumx-c
- fsss2 is about 6.5x (for easy sudokus) faster than sudokumx-c

If you want look at my code I propose to start with [BoardUtils.md](BoardUtils.md)
