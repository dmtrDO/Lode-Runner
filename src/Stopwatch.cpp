
#ifndef STOPWATCH
#define STOPWATCH

#include <chrono>
#include <SFML/System/Clock.hpp>

namespace t = std::chrono;

class Stopwatch {
private:
    bool isPaused;
    float time1;
    float time2;
    sf::Clock pausedClock;
public:
    Stopwatch() {
        start();
        time1 = t::duration_cast<t::milliseconds>(t::high_resolution_clock::now().time_since_epoch()).count();
    }
    void start() {
        isPaused = false;
        time1 += pausedClock.getElapsedTime().asMilliseconds();
    }
    void stop() {
        pausedClock.restart();
        isPaused = true;
    }
    float getElapsedTime() {
        if (isPaused == false) 
            time2 = t::duration_cast<t::milliseconds>(t::high_resolution_clock::now().time_since_epoch()).count();
        return time2 - time1;
    }
};

#endif 