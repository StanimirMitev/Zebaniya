// Game Health Test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

int damage(int a)
{
    int b;
    b = a - 1;
    return b;
}

int heal()
{
    return 4;
}

int main()
{
    int x = 4;
    int y = 0;
    while (x >= 1)
    {
        std::cout << "Heath Count: ";
        std::cout << x;
        std::cout << "\n";
        std::cin >> y;
        if (y == 1)
        {
            x = damage(x);
            y = 0;
        }
        else if (y == 2)
        {
            x = heal();
            y = 0;
        }
        else
        {
            std::cout << "Invalid Command \n";
            y = 0;
        }
    }
    std::cout << "Game Over \n";
}

