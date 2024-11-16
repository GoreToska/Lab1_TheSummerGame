#include "utilities.h"
#include <random>

int utilities::get_random_int(int min, int max)
{
    std::random_device rd;
    std::mt19937 gen(time(nullptr));
    std::uniform_int_distribution<> distribute(min, max);

    return distribute(gen);
}

float utilities::get_random_float(float min, float max)
{
    std::random_device rd;
    std::mt19937 gen(time(nullptr));
    std::uniform_real_distribution<float> distribute(min, max);

    return distribute(gen);
}

bool utilities::string_is_int(const std::string& string)
{
    return string.find_first_not_of("0123456789") == std::string::npos;
}

bool utilities::check_for_exit(const std::string& message)
{
    if (message != "exit")
        return false;

    return true;
}
