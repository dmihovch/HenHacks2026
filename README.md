# HenHacks2026
## High Noon Showdown
Devon Weckherlen, Jack Mallett, Daniel Mihovch

## TO DO LIST

- Clean up town.c
- Make interactive lobby (each person will do this for the best one)
- 3 Games, (Quick Draw, Donkey Kong/Platformer, )
- Graphics

## How to Run

The progam uses a CMakeList.txt file to help compile all of our .c files
into 1 executable

# In the terminal type:
- $> cmake -S . -B build
- $> cmake --build build
- $> .build/Debug/WildWestMG.exe

This sequence will create the build file and execute the game.

## How to Play

# In the Lobby:
- Male player controls are with WASD
- Female player controls with arrow keys
- Both players must walk to the same game to be able to enter
- If either player walks off the top or bottom of the screen, the game quits
- Can also press "Q" to quit

# In Bottles:
- Male player uses A and D to strafe and W to shoot at the bottles
- Female player uses left arrow and right arrow to strafe and up arrow to shoot at the bottles
- First person to 15 bottles broken wins!
- Score kept track in upper corners of screen

# In Platformer:
- Male player Uses WASD to navigate, W to jump
- Female player Uses arrow keys to navigate, up arrow to jump
- Both players must work together to scale the platforms and reach the top!
- Both players must be on the winning platform to win
- Dodge the Bandits shooting back at you

# In Quick Draw: 
- There is a countdown and at the end of the countdown each player must shoot the opposing players target
- Male player uses WASD to move crosshair
- Female player uses the arrow keys to move their crosshair
- Place your crosshair over the opposing persons target
- Male player presses left click to shoot while over others target
- Female player presses right click to shoot while over others target



