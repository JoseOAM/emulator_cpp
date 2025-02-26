#include "Main.h"

int main()
{
    printf("Hello\n");

    Register<int>* Registers = new Register<int>[32];
    int tmp;

    printf("First Value = ");
    scanf_s("%d", &tmp);
    Registers[12].setRegister(tmp);

    printf("Second Value = ");
    scanf_s("%d", &tmp);
    Registers[13].setRegister(tmp);

    for (int i = 0; i < 32; i++)
    {
        printf("Register[%d] = %d\n", i, Registers[i].getRegister());
    }

    add(&Registers[10], &Registers[12], &Registers[13]);

    for (int i = 0; i < 32; i++)
    {
        printf("Register[%d] = %d\n", i, Registers[i].getRegister());
    }

    delete[] Registers;

    return 0;
}