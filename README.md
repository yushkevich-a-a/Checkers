# checkers
Desktop application for playing checkers with a bot / friend in C++.__
Using the SDL2 framework for rendering.__
Supports the game bot vs bot with the setting of the depth of calculation for each separately (from main.cpp as args for Game::play).__
For developers:__
The calculation is made for the number of steps equal to depth + 1, where, for example, steps with multiple takes are counted as 1 step.__
State traversal uses a minimax algorithm with alpha-beta pruning heuristics.__
To calculate values in leaf states, the Logic::calc_score function is used.
