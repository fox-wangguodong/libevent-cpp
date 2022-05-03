#include "../EventBase.h"
#include "../Timer.h"
#include <bitset>
#include <iostream>
#include <list>
#include <map>
#include <optional>
#include <string>
#include <vector>

int main()
{
    EventBase eventBase;
    Timer timer(eventBase);
    timer.addTimer(
            []()
            {
                std::cout << "tick" << std::endl;
            },
            std::chrono::milliseconds(1000));
    eventBase.loop();
    return 0;
}