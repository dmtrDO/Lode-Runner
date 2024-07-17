
#include "Game.h"

int main()
{
    int placeForExpr = 20;
    unsigned int w = 32 * 30, h = 23 * 30;
    sf::VideoMode bestMode;
    for (sf::VideoMode mode : sf::VideoMode::getFullscreenModes())
    {
        if (mode.height >= sf::VideoMode::getDesktopMode().height - 2 * placeForExpr) continue;
        bestMode = mode;
        break;
    }
    double k = static_cast<double>(bestMode.height) / static_cast<double>(h);
    double width = k * static_cast<double>(w);

    sf::Vector2i winpos = sf::Vector2i(static_cast<int>((sf::VideoMode::getDesktopMode().width - width) / 2),
        static_cast<int>((sf::VideoMode::getDesktopMode().height - bestMode.height) / 2 - 2 * placeForExpr));

    while (true)
    {
        int size = 1;
        std::ifstream file;
        while (true)
        {
            std::string path = "src/levels/level" + std::to_string(size) + ".txt";
            file.open(path);
            if (!file.is_open()) break;
            size++;
            file.close();
        }

        std::string savePath = "src/levels/save.txt";
        std::ifstream isave(savePath);
        char lvl = isave.get();
        isave.close();

        int level = lvl - '0';
        bool open = true;
        for (int i = level; i < size; i++, lvl++)
        {
            if (!open) break;
            std::string path = "src/levels/level" + std::to_string(i) + ".txt";
            Game game(path, i, winpos);
            while (game.window.isOpen())
            {
                game.handle();
                game.update();
                game.draw();
                winpos = game.getWinpos();
            }
            if (game.isIgnoreLevel())
            {
                i--;
                lvl--;
            }
            if (!game.getOpen())
            {
                open = false;
                break;
            }
        }

        if (size == lvl - '0') lvl = '1';

        std::ofstream osave(savePath, std::ios::out | std::ios::trunc);
        osave.put(lvl);
        osave.close();
        if (!open) return 0;
    }

    return 0;
}


