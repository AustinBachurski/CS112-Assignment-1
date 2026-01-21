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
#include <utility>


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
        std::print("Enter password for ID {}: ", employee->getID());
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

std::string getStringArgFromConsole(std::string_view arg)
{
    std::print("Enter an employee {}: ", arg);

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
    std::string name{ getStringArgFromConsole("name") };

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

void removeCurrentEmployee(std::vector<std::unique_ptr<Employee>> &employees)
{
    unsigned id{ getIdFromConsole() };

    auto idIs{ [id](std::unique_ptr<Employee> const &employee)
                         {
                            return employee->getID() == id;
                         } };

    auto found{ std::ranges::find_if(employees, idIs) };

    if (found == employees.end())
    {
        clearScreen();
        std::println("Employee ID: \"{}\" was not found in the database.", id);
        clearScreenWhenReady();
        return;
    }

    std::println("Employee:\n\n{}\nhas been removed from the database.\n", **found);

    std::erase_if(employees, idIs);

    clearScreenWhenReady();
}

bool contains(std::span<std::unique_ptr<Employee> const> employees, unsigned const id)
{
    return std::ranges::find_if( employees, [id](std::unique_ptr<Employee> const &employee)
        { return employee->getID() == id; } ) != employees.end();
}

unsigned getValidId(std::span<std::unique_ptr<Employee> const> employees)
{
    unsigned id{};

    while (true)
    {
        id = getIdFromConsole();

        if (contains(employees, id))
        {
            std::println("ID {} already exists in the database, try again.", id);
        }
        else
        {
            return id;
        }
    }
}

void addNewEmployee(std::vector<std::unique_ptr<Employee>> &employees)
{
    unsigned id{ getValidId(employees) };
    std::string name{ getStringArgFromConsole("name") };
    std::string password{ getStringArgFromConsole("password") };

    std::string type;

    while (true)
    {
        type = getStringArgFromConsole("employee type");
        
        if (type == "GeneralEmployee")
        {
            employees.push_back(std::make_unique<GeneralEmployee>(Employee::EmployeeBuilder{ id, name, password }));
            break;
        }
        else if (type == "HumanResourcesEmployee")
        {
            employees.push_back(std::make_unique<HumanResourcesEmployee>(Employee::EmployeeBuilder{ id, name, password }));
            break;
        }
        else if (type == "ManagerEmployee")
        {
            employees.push_back(std::make_unique<ManagerEmployee>(Employee::EmployeeBuilder{ id, name, password }));
            break;
        }

        std::println("{} is not a valid employee type, try again.", type);
    }

    std::println("Employee:\n\n{}\nhas been added to the database.\n", *employees.back());
    clearScreenWhenReady();
}

unsigned getExistingEmployeeId(std::span<std::unique_ptr<Employee> const> employees)
{
    unsigned id{};

    while (true)
    {
        id = getIdFromConsole();

        auto found{ std::ranges::find_if(employees, [id](std::unique_ptr<Employee> const &employee)
                                         {
                                             return employee->getID() == id;
                                         }) };

        if (found != employees.end())
        {
            std::println("Found employee:\n\n{}", **found);
            return id;
        }

        std::println("Employee ID: \"{}\" was not found in the database.", id);
    }
}

enum struct Field : unsigned
{
    invalid,
    id,
    name,
    password,
    title,
    count,
};

Field selectFieldToModify()
{
    std::println("Select a field to modify.\n1. ID\n2. Name\n3. Password\n4. Title");

    std::string input;
    unsigned selection{};

    while (true)
    {
        std::getline(std::cin, input);

        if (std::from_chars(input.data(), input.data() + input.size(), selection).ec == std::errc{})
        {
            if (Field{ selection } > Field::invalid && Field{ selection } < Field::count)
            {
                clearScreen();
                return Field{selection};
            }
        }

        std::println("Invalid selection.");
    }
}

void modifyEmployeeId(std::vector<std::unique_ptr<Employee>> &employees, unsigned id)
{
    auto found{ std::ranges::find_if(employees, [id](std::unique_ptr<Employee> const &employee)
                         {
                            return employee->getID() == id;
                         }) };

    if (found != employees.end())
    {
        (*found)->setID(getValidId(employees));

        clearScreen();
        std::println("Employee ID updated\n\n{}\n", **found);
        clearScreenWhenReady();
    }
    else
    {
        // ID should already be verified before calling this function.
        std::unreachable();
    }
}

void modifyEmployeeName(std::vector<std::unique_ptr<Employee>> &employees, unsigned id)
{
    auto found{ std::ranges::find_if(employees, [id](std::unique_ptr<Employee> const &employee)
                         {
                            return employee->getID() == id;
                         }) };

    if (found != employees.end())
    {
        (*found)->setName(getStringArgFromConsole("name"));

        clearScreen();
        std::println("Employee name updated\n\n{}\n", **found);
        clearScreenWhenReady();
    }
    else
    {
        // ID should already be verified before calling this function.
        std::unreachable();
    }
}

void modifyEmployeePassword(std::vector<std::unique_ptr<Employee>> &employees, unsigned id)
{
    auto found{ std::ranges::find_if(employees, [id](std::unique_ptr<Employee> const &employee)
                         {
                            return employee->getID() == id;
                         }) };

    if (found != employees.end())
    {
        (*found)->setPassword(getStringArgFromConsole("password"));

        clearScreen();
        std::println("Employee password updated\n\n{}\n", **found);
        clearScreenWhenReady();
    }
    else
    {
        // ID should already be verified before calling this function.
        std::unreachable();
    }
}

void modifyEmployeeTitle(std::vector<std::unique_ptr<Employee>> &employees, unsigned id)
{
    auto idIs{ [id](std::unique_ptr<Employee> const &employee)
                         {
                            return employee->getID() == id;
                         } };

    auto found{ std::ranges::find_if(employees, idIs) };

    if (found == employees.end())
    {
        // ID should already be verified before calling this function.
        std::unreachable();
    }

    std::string name{ (*found)->getName() };
    std::string password{ (*found)->getPassword() };


    std::string type;

    while (true)
    {
        type = getStringArgFromConsole("employee type");
        
        if (type == "GeneralEmployee")
        {
            std::erase_if(employees, idIs);
            employees.push_back(std::make_unique<GeneralEmployee>(Employee::EmployeeBuilder{ id, name, password }));
            break;
        }
        else if (type == "HumanResourcesEmployee")
        {
            std::erase_if(employees, idIs);
            employees.push_back(std::make_unique<HumanResourcesEmployee>(Employee::EmployeeBuilder{ id, name, password }));
            break;
        }
        else if (type == "ManagerEmployee")
        {
            std::erase_if(employees, idIs);
            employees.push_back(std::make_unique<ManagerEmployee>(Employee::EmployeeBuilder{ id, name, password }));
            break;
        }

        std::println("{} is not a valid employee type, try again.", type);
    }

    found = std::ranges::find_if(employees, idIs);

    if (found == employees.end())
    {
        std::println("Error updating database!");
    }

    clearScreen();
    std::println("Employee title updated\n\n{}\n", **found);
    clearScreenWhenReady();
}

void modifyExistingEmployee(std::vector<std::unique_ptr<Employee>> &employees)
{
    std::println("Which employee do you wish to modify?");

    unsigned id{ getExistingEmployeeId(employees) };

    Field field{ selectFieldToModify() };

    switch (field)
    {
        case Field::id:
            return modifyEmployeeId(employees, id);
        case Field::name:
            return modifyEmployeeName(employees, id);
        case Field::password:
            return modifyEmployeePassword(employees, id);
        case Field::title:
            return modifyEmployeeTitle(employees, id);
        case Field::invalid: [[fallthrough]];
        case Field::count:
            std::unreachable();
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

    modifyExistingEmployee(employees);
}

void ManagementInformationSystem::addEmployee()
{
    if (!loggedInUser->canAddEmployee())
    {
        return nope();
    }

    addNewEmployee(employees);
}

void ManagementInformationSystem::removeEmployee()
{
    if (!loggedInUser->canRemoveEmployee())
    {
        return nope();
    }

    removeCurrentEmployee(employees);
}

