//******************************************************************************
//File Name: menu.cpp
//Description: Menu implementation for CS112: Assignment 1
//Author: Austin Bachurski
//Created: January 20, 2026
//******************************************************************************

#include "managementInformationSystem.hpp"
#include "employees.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <print>
#include <ranges>
#include <span>
#include <string>
#include <string_view>


namespace
{

std::unique_ptr<Employee> makeEmployee(std::string_view line)
{
        auto fields{ line | std::views::split(',')
                          | std::views::transform([](auto&& split_view)
                              { return std::string_view{ split_view }; }) };

        auto fieldsIter = fields.begin();

        auto nextField{ [&fieldsIter, &fields, line]()
            {
                if (fieldsIter == fields.end())
                { 
                    std::println("Unable to parse input: {}", line);
                }
                return *fieldsIter++;
            }};

        return std::make_unique<HumanResources>(nextField(), nextField());
}

std::vector<std::unique_ptr<Employee>> populateEmployeesFromFile(std::filesystem::path pathToCSV)
{
    std::vector<std::unique_ptr<Employee>> employees;

    if (!std::filesystem::exists(pathToCSV))
    {
        std::println("Employee database not found at {}", pathToCSV.string());
        return employees;
    }

    std::fstream file{ pathToCSV };

    if (!file.is_open())
    {
        std::println("Failed to open employee database file, {}", pathToCSV.string());
        return employees;
    }

    std::string line;
    std::getline(file, line);  // Skip CSV header.

    while (std::getline(file, line))
    {
        employees.push_back(makeEmployee(line));
    }

    return employees;
}

Employee *getUserIfPresentOrNull(std::span<std::unique_ptr<Employee>> employees, std::string_view username)
{
    auto found{ std::ranges::find_if(employees, [username](std::unique_ptr<Employee>& employee)
                         {
                            return employee->getName() == username;
                         }) };

    if (found != employees.end())
    {
        return found->get();
    }

    return nullptr;
}

Employee *requestUserLogin(std::span<std::unique_ptr<Employee>> employees)
{
    std::string input;

    std::println("Please enter your credentials.");

    Employee *employee{ nullptr };

    while (!employee)
    {
        std::print("Enter Employee Name: ");
        std::getline(std::cin, input);
        employee = getUserIfPresentOrNull(employees, input);

        if (!employee)
        {
            std::println("Employee name \"{}\" was not found.", input);
        }
    }

    while (true)
    {
        std::print("Enter password for {}: ", employee->getName());
        std::getline(std::cin, input);

        if (employee->isCorrectPassword(input))
        {
            std::println("Login Success.");
            return employee;
        }
        else
        {
            std::println("Password incorrect.");
        }
    }

    return nullptr;
}

} // anonymous namespace

void ManagementInformationSystem::login()
{
    employees = populateEmployeesFromFile("data/employees.csv");

    std::println("**************************************************************");
    std::println("Employee Management");
    std::println("**************************************************************");
    std::println();
    std::println("Please enter your credentials to login.");

    loggedInUser = requestUserLogin(employees);
}

