#include <iostream>
#include <cstdlib> // For atoi()

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Please provide two numbers to add!" << std::endl;
        return 1;
    }

    // Convert arguments to integers
    int num1 = std::atoi(argv[1]);
    int num2 = std::atoi(argv[2]);

    // Add the numbers
    int sum = num1 + num2;

    // Output the result
    std::cout << "Result: " << sum << std::endl;

    return 0;
}
