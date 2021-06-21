#include "Arduino.h"

int main()
{
    setup();

    while (1)
    {
        loop();
        if(serialEventRun) serialEventRun();
    }

    return 0;
}