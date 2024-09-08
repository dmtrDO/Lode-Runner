
#include "Game.h" 

int WinMain() {

    Game game;

    while (game.window.isOpen()) {
        game.handle();
        game.update();
        game.draw();
    }

    return 0;
}


