#include "TestEngine\TestEngine.h"
#include <stdio.h>

bool Init(int argc, char **argv)
{
    SetWindowTitle("Test");
    SetFrameRate(60);
    SetWindowSize(500, 500);
    
    return false;
}

bool Update(void)
{
    for(int y = 0; y < 500; ++y)
    {
        for(int x = 0; x < 500; ++x)
        {
            DrawPixel(x, y, 0xFF00FFAC);
        }
    }
    
    return false;
}

void Destroy(void)
{

}