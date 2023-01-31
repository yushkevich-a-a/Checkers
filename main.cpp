#include "Game/Game.h"
#include "Game/Board.h"

#include <iostream>
#include <vector>

using namespace std;

int main( int argc, char *argv[] )
{
    Game g(500, 500);
    g.play(0, 1, 1, 8, 9);
}
