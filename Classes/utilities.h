#pragma once
#include <iostream>

class utilities
{
public:
    static int get_random_int(int min, int max);
    static float get_random_float(float min, float max);
    static bool string_is_int(const std::string& string);
    static bool check_for_exit(const std::string& message);
};
