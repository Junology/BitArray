#include <iostream>
#include <chrono>

#include <bitset>

int main(int, char**)
{
    volatile std::size_t i;

    std::cout << "Bench mark of count() for std::bitset." << std::endl;

    auto beg_time = std::chrono::system_clock::now();
    for(unsigned long long x = 0ull; x < 0xFFFFFFFull; ++x) {
        i = std::bitset<128>{x}.count();
    }
    auto end_time = std::chrono::system_clock::now();
    std::cout
        << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - beg_time).count()
        << "ms" << std::endl;;

    return EXIT_SUCCESS;
}
