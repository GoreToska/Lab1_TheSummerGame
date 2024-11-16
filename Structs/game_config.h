#pragma once

struct game_config
{
    const int citizen_acre_productivity = 10;
    const int citizen_wheat_usage = 20;
    const int min_wheat_per_acre = 1;
    const int max_wheat_per_acre = 6;
    const int min_acre_cost = 17;
    const int max_acre_cost = 26;
    const int max_rounds = 10;
    const int start_citizen = 100;
    const int start_wheat = 2800;
    const int start_arces = 1000;
    const float wheat_per_acre = 0.5f;
    const float min_rats_multiplier = 0;
    const float max_rats_multiplier = 0.07f;
    const float plague_chance = 0.15f;
    const float citizens_dead_percent_for_lose = 0.45f; 
};
