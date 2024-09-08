
#include "Game.h" 

int main() {

    /*Game game;

    while (game.window.isOpen()) {
        game.handle();
        game.update();
        game.draw();
    }*/

    std::ifstream ifile("src/lvls.txt");
    if (!ifile.is_open())
        throw std::exception("Can't open file");

    std::string str;
    char ch;

    bool isLine = false;
    while (ifile.get(ch)) {
        if (ch == '"' && isLine == false) {
            isLine = true;
            continue;
        }
        if (ch == '"' && isLine == true) {
            isLine = false;
            continue;
        }
        if (isLine == true) {
            str += ch;
        }
    }

    ifile.close();

    int counter = 0;
    for (int i = 151; i <= 200; i++) {
        std::ofstream ofile("levels/level" + std::to_string(i) + ".txt");
        std::string level = "";
        level += "                                    \n";
        level += "                                    \n";
        level += "                                    \n";
        for (int j = 0; j < 16; j++) {
            for (int k = 0; k < 28; k++) {
                switch (str[counter]) {
                    case ' ': level += ' '; break;
                    case '#': level += '1'; break;
                    case '@': level += '2'; break;
                    case 'H': level += '3'; break;
                    case '-': level += '4'; break;
                    case '$': level += '5'; break;
                    case 'S': level += '6'; break;
                    case '0': level += '7'; break;
                    case '&': level += '8'; break;
                    case 'X': level += 'i'; break;
                }
                counter++;
            }
            level += "        \n";
        }
        level += "999999999999999999999999999999999999";
        ofile << level;
        ofile.close();
    }


    return 0;
}



