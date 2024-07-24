
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

    sf::Vector2u winsize = sf::Vector2u(static_cast<unsigned int>(width), bestMode.height + placeForExpr);

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

        std::string savePath = "src/levels/.save.txt";
        std::ifstream isave(savePath);
        std::string lvl;
        isave >> lvl;
        isave.close();

        int level = std::stoi(lvl);
        bool open = true;
        for (int i = level; i < size; i++)
        {
            if (!open) break;
            std::string path = "src/levels/level" + std::to_string(i) + ".txt";
            Game game(path, i, winpos, winsize);
            while (game.window.isOpen())
            {
                game.handle();
                game.update();
                game.draw();
            }
            if (game.isIgnoreLevel())
            {
                i--;
                int prevLevel = std::stoi(lvl);
                prevLevel--;
                lvl = std::to_string(prevLevel);
            }
            if (!game.getOpen())
            {
                open = false;
                break;
            }
            winpos = game.getWinpos();
            winsize = game.window.getSize();

            int nextLevel = std::stoi(lvl);
            nextLevel++;
            lvl = std::to_string(nextLevel);
        }

        if (size == std::stoi(lvl)) lvl = "1";

        std::ofstream osave(savePath, std::ios::out | std::ios::trunc);
        osave << lvl;
        osave.close();
        if (!open) return 0;
    }

    return 0;
}


