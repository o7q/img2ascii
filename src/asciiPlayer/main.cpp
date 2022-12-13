#include <iostream>
#include <ctime>
using namespace std;

const string version = "v1.0.0";

main()
{
    int delay;
    cin >> delay;
    delay *= CLOCKS_PER_SEC;
    while (true)
    {
        clock_t now = clock();
        while (clock() - now < delay);

        cout << "TICK" << endl;
    }

    system("pause");
    return 0;
}