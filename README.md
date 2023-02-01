# Checkers  
Desktop application for playing checkers with a bot / friend in C++.  
Using the SDL2 framework for rendering.  
Supports the game bot vs bot with the setting of the depth of calculation for each separately (from settings.json).  
## For developers:  
The calculation is made for the number of steps equal to depth + 1, where, for example, steps with multiple takes are counted as 1 step.  
State traversal uses a minimax algorithm with alpha-beta pruning heuristics.  
To calculate values in leaf states, the Logic::calc_score function is used.  
You can set your params in settings.json:  
### WindowSize
Width - unsigned int from 0 to screen size. 0 - fullscreen.  
Hight - unsigned int from 0 to screen size. 0 - fullscreen.  
### Bot
IsWhiteBot - true/false.  
IsBlackBot - true/false.  
WhiteBotLevel - unsigned int from 0. If "IsWhiteBot" is set true then the depth of calculation will be "WhiteBotLevel" + 1.  
BlackBotLevel - unsigned int from 0. If "IsBlackBot" is set true then the depth of calculation will be "BlackBotLevel" + 1.  
BotScoringType - "NumberOnly" (the bot takes into account only the number of checkers)  or "NumberAndPotential" (the bot also takes into account the positions of checkers).  
BotDelayMS - unsigned int from 0. Minimum delay per bot move.  
NoRandom - true/false. Whether the bot will be deterministic.  
