//******************************************************************************
//File Name: managementInformationSystem.cpp
//Description: Implementation for managementInformationSystem object.
//Author: Austin Bachurski
//Created: January 20, 2026
//******************************************************************************

#include "managementInformationSystem.hpp"
#include "employees.hpp"

#include <algorithm>
#include <cstdlib>
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

void clearScreen()
{
    std::system("clear");
}

void clearScreenWhenReady()
{
    std::println("Press `Enter` to continue...");
    std::cin.get();
    clearScreen();
}

[[noreturn]]
void invalidInput(std::string_view line)
{
    std::println("Invalid input encountered!");
    std::println("Found: {}", line);
    std::exit(1);
}

unsigned parseField(std::string_view field, std::string_view line)
{
    unsigned value{};

    if (!(std::from_chars(field.data(), field.data() + field.size(), value).ec == std::errc{}))
    {
        invalidInput(line);
    }

    return value;
}

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
                invalidInput(line);
            }
            return *fieldsIter++;
        }};

    Employee::EmployeeBuilder params{
        .id = parseField(nextField(), line),
        .name{ nextField() },
        .password{ nextField()}
    };

    auto type{ nextField() };

    if (type == "GeneralEmployee")
    {
        return std::make_unique<GeneralEmployee>(params);
    }
    else if (type == "HumanResourcesEmployee")
    {
        return std::make_unique<HumanResourcesEmployee>(params);
    }
    else if (type == "ManagerEmployee")
    {
        return std::make_unique<ManagerEmployee>(params);
    }

    invalidInput(line);
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

Employee *getUserIfPresentOrNull(std::span<std::unique_ptr<Employee> const> employees, unsigned id)
{
    auto found{ std::ranges::find_if(employees, [id](std::unique_ptr<Employee> const &employee)
                         {
                            return employee->getID() == id;
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
    unsigned id{};

    std::println("Please enter your credentials.");

    Employee *employee{ nullptr };

    while (!employee)
    {
        std::print("Enter Employee Number: ");
        std::getline(std::cin, input);

        if (!(std::from_chars(input.data(), input.data() + input.size(), id).ec == std::errc{}))
        {
            std::println("Please enter a valid ID number.");
            continue;
        }

        employee = getUserIfPresentOrNull(employees, id);

        if (!employee)
        {
            std::println("Employee id \"{}\" was not found.", input);
        }
    }

    while (true)
    {
        std::print("Enter password for {}: ", employee->getID());
        std::getline(std::cin, input);

        if (employee->isCorrectPassword(input))
        {
            clearScreen();
            return employee;
        }
        else
        {
            std::println("Password incorrect.");
        }
    }

    return nullptr;  // Should never get here.
}

unsigned getIdFromConsole()
{
    std::print("Enter an employee ID: ");

    std::string line;
    unsigned id{};

    while (true)
    {
        std::getline(std::cin, line);

        if (std::from_chars(line.data(), line.data() + line.size(), id).ec == std::errc{})
        {
            return id;
        }

        std::println("{} is not a valid id.", line);
    }
}

std::string getNameFromConsole()
{
    std::print("Enter an employee name: ");

    std::string line;
    std::getline(std::cin, line);

    return line;
}

void searchByID(std::span<std::unique_ptr<Employee> const> employees)
{
    unsigned id{ getIdFromConsole() };

    auto found{ std::ranges::find_if(employees, [id](std::unique_ptr<Employee> const &employee)
                         {
                            return employee->getID() == id;
                         }) };

    if (found == employees.end())
    {
        clearScreen();
        std::println("Employee ID: \"{}\" was not found in the database.", id);
        clearScreenWhenReady();
        return;
    }

    clearScreen();
    std::println("Found:\n{}", **found);
    clearScreenWhenReady();
}

void searchByName(std::span<std::unique_ptr<Employee> const> employees)
{
    std::string name{ getNameFromConsole() };

    auto nameIs{ [&name](std::unique_ptr<Employee> const &employee)
                         {
                            return employee->getName() == name;
                         } };

    auto found{ std::ranges::find_if(employees, nameIs) };

    if (found == employees.end())
    {
        clearScreen();
        std::println("Employee \"{}\" was not found in the database.", name);
        clearScreenWhenReady();
        return;
    }

    clearScreen();
    std::println("Found:\n");

    for (auto const &each : employees | std::views::filter(nameIs))
    {
         std::println("{}\n", *each);
    }

    clearScreenWhenReady();
}


void nope()
{
    std::println("User does not have permission to perform this action.");
}

void searchEmployeesBy(std::span<std::unique_ptr<Employee> const> employees)
{
    std::string line;

    while (true)
    {
        std::println("Select search type:\n1. Search by name.\n2. Search by ID.");

        std::getline(std::cin, line);

        if (line == "1")
        {
            clearScreen();
            return searchByName(employees);
        }

        if (line == "2")
        {
            clearScreen();
            return searchByID(employees);
        }

        std::println("Invalid selection.");
    }
}

} // anonymous namespace

void ManagementInformationSystem::login()
{
    employees = populateEmployeesFromFile("data/employees.csv");

    clearScreen();
    std::println("**************************************************************");
    std::println("Employee Management");
    std::println("**************************************************************");
    std::println();
    std::println("Please enter your credentials to login.");

    loggedInUser = requestUserLogin(employees);

    if (loggedInUser)
    {
        displayMenu();
    }
}

void ManagementInformationSystem::displayMenu()
{
    loggedInUser->displayMenu();

    std::string line;
    unsigned selection{};

    enum class MenuSelection
    {
        exit,
        view,
        search,
        modify,
        add,
        remove,
        selectionCount,
    };

    while (true)
    {
        std::getline(std::cin, line);


        if (std::from_chars(line.data(), line.data() + line.size(), selection).ec == std::errc{})
        {
            if (selection < static_cast<unsigned>(MenuSelection::selectionCount))
            {
                clearScreen();
                switch (MenuSelection(selection))
                {
                    case MenuSelection::exit:
                        std::println("Disconnected...");
                        return;
                    case MenuSelection::view:
                        viewEmployees();
                        break;
                    case MenuSelection::search:
                        searchEmployees();
                        break;
                    case MenuSelection::modify:
                        modifyEmployee();
                        break;
                    case MenuSelection::add:
                        addEmployee();
                        break;
                    case MenuSelection::remove:
                        removeEmployee();
                        break;
                    case MenuSelection::selectionCount:
                        std::unreachable();
                }
            }
        }
        else
        {
            std::println("Invalid selection.  Expected an integer, got {}.", line);
        }

        loggedInUser->displayMenu();
    }
}

void ManagementInformationSystem::viewEmployees() const
{
    if (!loggedInUser->canViewEmployees())
    {
        std::println("Your Employee Data:\n{}", *loggedInUser);
        clearScreenWhenReady();
        return;
    }

    std::println("************ ALL EMPLOYEES ************");

    for (auto const &employee : employees)
    {
         std::println("{}", *employee);
    }

    std::println("***************************************");
    clearScreenWhenReady();
}

void ManagementInformationSystem::searchEmployees() const
{
    if (!loggedInUser->canSearchEmployees())
    {
        return nope();
    }

    searchEmployeesBy(employees);
}

void ManagementInformationSystem::modifyEmployee()
{
    if (!loggedInUser->canModifyEmployee())
    {
        return nope();
    }
}

void ManagementInformationSystem::addEmployee()
{
    if (!loggedInUser->canAddEmployee())
    {
        return nope();
    }
}

void ManagementInformationSystem::removeEmployee()
{
    if (!loggedInUser->canRemoveEmployee())
    {
        return nope();
    }
}

