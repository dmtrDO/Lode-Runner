
#include "Game.h" 

int main() {
    Game game;
    while (game.window.isOpen()) {
        game.handle();
        game.update();
        game.draw();
    }
    return 0;
}


