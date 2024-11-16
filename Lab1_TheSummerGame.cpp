#include <iostream>
#include <string>
#include <fstream>

#include "Structs/game_config.h"
#include "Structs/player_rating.h"
#include "Structs/acres_of_land.h"
#include "Structs/citizens.h"
#include "Structs/wheat.h"
#include "Classes/utilities.h"

game_config g_config;
player_rating g_rating;
citizens g_citizens;
wheat g_wheat;
acres_of_land g_land;
int g_current_round;

#pragma region Saves

bool has_save()
{
    std::ifstream stream;
    stream.open("Save.txt");

    if (!stream.is_open())
        return false;

    bool isEmpty = stream.peek() == EOF;
    stream.close();
    return !isEmpty;
}

void load_game()
{
    std::ifstream stream;
    stream.open("Save.txt");

    if (!stream.is_open())
        return;

#pragma region Read values from file

    std::string value;
    getline(stream, value);
    g_citizens.current = std::stoi(value);
    getline(stream, value);
    g_citizens.starved = std::stoi(value);
    getline(stream, value);
    g_citizens.died_of_plague = std::stoi(value);
    getline(stream, value);
    g_citizens.came_this_year = std::stoi(value);
    getline(stream, value);
    g_citizens.damaged_by_plague = std::stoi(value);
    getline(stream, value);
    g_wheat.to_eat = std::stoi(value);
    getline(stream, value);
    g_citizens.starved_percent = std::stof(value);
    getline(stream, value);
    g_wheat.current = std::stoi(value);
    getline(stream, value);
    g_wheat.harvested_this_year = std::stoi(value);
    getline(stream, value);
    g_wheat.rats_destroyed = std::stof(value);
    getline(stream, value);
    g_land.current = std::stoi(value);
    getline(stream, value);
    g_land.planted = std::stoi(value);
    getline(stream, value);
    g_land.wheat_per_acre = std::stoi(value);
    getline(stream, value);
    g_land.cost = std::stoi(value);
    getline(stream, value);
    g_current_round = std::stoi(value);

#pragma endregion

    stream.close();
}

void save_game()
{
    std::ofstream stream;
    stream.open("Save.txt");

    if (!stream.is_open())
        return;

#pragma region Write values to file

    stream << g_citizens.current << "\n"
        << g_citizens.starved << "\n"
        << g_citizens.died_of_plague << "\n"
        << g_citizens.came_this_year << "\n"
        << g_citizens.damaged_by_plague << "\n"
        << g_wheat.to_eat << "\n"
        << g_citizens.starved_percent << "\n"
        << g_wheat.current << "\n"
        << g_wheat.harvested_this_year << "\n"
        << g_wheat.rats_destroyed << "\n"
        << g_land.current << "\n"
        << g_land.planted << "\n"
        << g_land.wheat_per_acre << "\n"
        << g_land.cost << "\n"
        << g_current_round << "\n";;

#pragma endregion

    stream.close();
}

bool want_to_load_game()
{
    if (!has_save())
        return false;

    std::string decision;
    std::cout << "You have a save. Would you like to load it? (yes/no) ";
    std::cin >> decision;

    if (decision == "yes")
        return true;

    return false;
}

void clear_save()
{
    std::ofstream stream;
    stream.open("Save.txt");
    stream.clear();
    stream.close();
}

#pragma endregion

#pragma region Citizens

void feed_citizens()
{
    int citizensCanEat = g_wheat.to_eat / g_config.citizen_wheat_usage;

    g_citizens.starved = g_citizens.current - citizensCanEat;
    g_citizens.starved_percent = g_citizens.starved / g_citizens.current;
    g_citizens.current -= g_citizens.starved;
    g_citizens.starved_total += g_citizens.starved;
}

void arrive_citizens()
{
    int arrived = g_citizens.starved / 2 + (5 - g_land.wheat_per_acre) * g_wheat.current / 600 + 1;
    arrived = arrived < 0 ? 0 : arrived;
    arrived = arrived > 50 ? 50 : arrived;

    g_citizens.current += arrived;
    g_citizens.came_this_year = arrived;
}

void plague_damage()
{
    float random = utilities::get_random_float(0, 100) / 100;
    if (random >= 1 - g_config.plague_chance)
    {
        g_citizens.died_of_plague = g_citizens.current / 2;
        g_citizens.current /= 2;
        g_citizens.damaged_by_plague = true;
        return;
    }

    g_citizens.damaged_by_plague = false;
}

#pragma endregion

#pragma region Wheat

void get_rats_damage()
{
    float rat_damage = utilities::get_random_float(g_config.min_rats_multiplier, g_config.max_rats_multiplier);
    g_wheat.rats_destroyed = rat_damage * g_wheat.current;
    g_wheat.current -= g_wheat.rats_destroyed;
}

#pragma endregion

#pragma region Acres

void plant_fields(int acres)
{
    g_wheat.current -= acres / 2;
    g_land.planted = acres;
}

void harvest_fields()
{
    g_land.wheat_per_acre = utilities::get_random_int(g_config.min_wheat_per_acre, g_config.max_wheat_per_acre);
    g_wheat.harvested_this_year = g_land.wheat_per_acre * g_land.planted;
    g_wheat.current += g_wheat.harvested_this_year;
}

void update_acre_cost()
{
    g_land.cost = utilities::get_random_int(g_config.min_acre_cost, g_config.max_acre_cost);
}

#pragma endregion

#pragma region Orders

bool buy_acres(int count, std::string& return_string)
{
    if (count * g_land.cost <= g_wheat.current)
    {
        return_string = "You bought " + std::to_string(count) + " acres of land and spent " + std::to_string(
            count * g_land.cost) + " bushels of wheat on it\n";
        g_wheat.current -= count * g_land.cost;
        g_land.current += count;

        return true;
    }

    return_string = "You don't have enough wheat.\n";
    return false;
}

bool wheat_to_eat(int count, std::string& return_string)
{
    if (count <= g_wheat.current)
    {
        return_string = "You ordered to eat " + std::to_string(count) + " bushels of wheat.\n";
        g_wheat.to_eat = count;
        g_wheat.current -= g_wheat.to_eat;

        return true;
    }

    return_string = "You don't have enough wheat.\n";
    return false;
}

bool acres_to_plant(int count, std::string& return_string)
{
    if (count > g_land.current)
    {
        return_string = "We don't have that much land!\n";
        return false;
    }

    if (count / 2 > g_wheat.current)
    {
        return_string = "We don't have that much wheat!\n";
        return false;
    }

    if (count < g_citizens.current * g_config.citizen_acre_productivity)
    {
        return_string = std::to_string(count) + " acres of land will be planted this year.\n";
        plant_fields(count);

        return true;
    }

    return_string = "We can't do that.\n";
    return false;
}

#pragma endregion

#pragma region Game

void init_new_game()
{
    g_citizens.current = g_config.start_citizen;
    g_wheat.current = g_config.start_wheat;
    g_land.current = g_config.start_arces;
    g_citizens.starved = 0;
    g_citizens.died_of_plague = 0;
    g_citizens.came_this_year = 0;
    g_wheat.harvested_this_year = 0;
    g_wheat.rats_destroyed = 0;
    g_land.planted = 0;
    g_land.wheat_per_acre = 0;
    g_citizens.damaged_by_plague = false;
    g_wheat.to_eat = 0;
    g_current_round = 1;
    update_acre_cost();
}

bool check_lose_condition()
{
    if (g_citizens.starved_percent >= g_config.citizens_dead_percent_for_lose)
        return true;

    return false;
}

void simulate_year()
{
    feed_citizens();
    plague_damage();
    arrive_citizens();
    harvest_fields();
    get_rats_damage();
    update_acre_cost();
}

void get_year_statistics()
{
    std::cout << "My lord, let me tell you\n";
    std::cout << "	In the " << g_current_round << " year of your supreme reign:\n";
    std::cout << "	" << g_citizens.starved << " peasants starved to death, "
        << g_citizens.came_this_year << " peasants have arrived in our great city;\n";

    if (g_citizens.damaged_by_plague)
        std::cout << "	The plague wiped out half of the population;\n";

    std::cout << "	The population of the city is now " << g_citizens.current << " people;\n";
    std::cout << "	We harvested " << g_wheat.harvested_this_year
        << " bushels of wheat, " << g_land.wheat_per_acre << " bushels per acre;\n";
    std::cout << "	Rats destroyed " << g_wheat.rats_destroyed << " bushels of wheat, leaving "
        << g_wheat.current << " bushels in barns;\n";
    std::cout << "	The city is " << g_land.current << " acres of land;\n";
    std::cout << "	1 acre of land is now worth " << g_land.cost << " bushels.\n\n\n";
}

void get_player_decisions()
{
    std::string input;
    std::string message;
    int converted_input;

    bool result = false;
    while (!result)
    {
        std::cout << "How many acres of land do you want to buy? (Price - " << g_land.cost << ") ";
        std::cin >> input;

        if (utilities::check_for_exit(input))
        {
            save_game();
            exit(0);
        }

        if (!utilities::string_is_int(input))
            continue;

        converted_input = std::stoi(input);
        result = buy_acres(converted_input, message);
        std::cout << message;
    }

    result = false;
    while (!result)
    {
        std::cout << "How many bushels of wheat do you order to eat? ";
        std::cin >> input;

        if (!utilities::string_is_int(input))
            continue;

        converted_input = std::stoi(input);
        result = wheat_to_eat(converted_input, message);
        std::cout << message;
    }

    result = false;
    while (!result)
    {
        std::cout << "How many acres of land do you order to plant? ";
        std::cin >> input;

        if (!utilities::string_is_int(input))
            continue;

        converted_input = std::stoi(input);
        result = acres_to_plant(converted_input, message);
        std::cout << message;
    }
}

void show_results()
{
    if (g_citizens.starved_total / g_current_round > g_rating.bad_deaths &&
        g_land.current / g_citizens.current < g_rating.bad_acres)
    {
        std::cout <<
            "Because of your incompetence in management, the people staged a riot, and drove you out of their city.\n"
            "Now you are forced to eke out a miserable existence in exile.\n";
        return;
    }
    if (g_citizens.starved_total / g_current_round > g_rating.passing_deaths &&
        g_land.current / g_citizens.current < g_rating.passing_acres)
    {
        std::cout << "You ruled with an iron hand, like Nero and Ivan the Terrible.\n"
            "The people have breathed a sigh of relief, and no one wants to see you as ruler anymore.\n";
        return;
    }
    if (g_citizens.starved_total / g_current_round > g_rating.good_deaths &&
        g_land.current / g_citizens.current < g_rating.good_acres)
    {
        std::cout << "You did quite well, of course, you have detractors, "
            "but many would like to see you at the head of the city again.\n";
        return;
    }

    std::cout << "Fantastic! Charlemagne, Disraeli and Jefferson couldn't have done better together.\n";
}

void show_resources()
{
    std::cout << "You have " << g_citizens.current << " citizens, "
        << g_wheat.current << " bushels of wheat and "
        << g_land.current << " acres of land.\n";
}

#pragma endregion

int main()
{
    if (want_to_load_game())
        load_game();
    else
        init_new_game();

    if (g_current_round == 1)
    {
        std::cout << "Here starts your reign!\n";
    }
    else
    {
        get_year_statistics();
    }

    while (g_current_round <= g_config.max_rounds)
    {
        show_resources();
        get_player_decisions();
        simulate_year();
        get_year_statistics();

        if (check_lose_condition())
        {
            std::cout << "You Lost!\n";
            clear_save();
            break;
        }

        g_current_round++;
        save_game();
    }

    if (g_current_round == 10)
        show_results();

    std::cin.ignore(2);
}
