//******************************************************************************
//File Name: employee.hpp
//Description: Employee base class for CS112: Assignment 1.
//Author: Austin Bachurski
//Created: January 20, 2026
//******************************************************************************

#ifndef EMPLOYEE_BASE_HPP
#define EMPLOYEE_BASE_HPP

#include <format>
#include <string>
#include <string_view>


class Employee
{
public:
    virtual ~Employee() = default;

    struct EmployeeBuilder
    {
        std::string_view name;
        std::string_view password;
    };

    std::string_view getName() { return name; }
    bool isCorrectPassword(std::string_view userPassword) { return userPassword == password; }

protected:
    Employee(EmployeeBuilder const &params)
    : name{ params.name }
    , password{ params.password }
    {}

private:
    std::string name;
    std::string password;
};

class GeneralEmployee : public Employee
{
public:
    GeneralEmployee(EmployeeBuilder const &params)
    : Employee(params)
    {}
};

class HumanResourcesEmployee : public Employee
{
public:
    HumanResources(EmployeeBuilder const &params)
    : Employee(params)
    {}
};

class ManagerEmployee : public Employee
{
public:
    ManagerEmployee(EmployeeBuilder const &params)
    : Employee(params)
    {}
};

template<>
struct std::formatter<GeneralEmployee>
{
    constexpr auto parse(std::format_parse_context &context) const
    {
        return context.begin();
    }
    
    auto format(GeneralEmployee const &employee, std::format_context &context) const
    {
        return std::format_to(context.out(),
                "\nLogged in as General Employee"
                "Please make a selection:\n"
                "0. Exit\n"
                "1. View employee file.\n"
                );
    }
};

template<>
struct std::formatter<HumanResourcesEmployee>
{
    constexpr auto parse(std::format_parse_context &context) const
    {
        return context.begin();
    }
    
    auto format(HumanResourcesEmployee const &employee, std::format_context &context) const
    {
        return std::format_to(context.out(),
                "\nLogged in as Human Resources Employee"
                "Please make a selection:\n"
                "0. Exit\n"
                "1. View employees.\n"
                "2. Search for an employee.\n"
                "3. Modify an employee.\n"
                "4. Add an employee.\n"
                "5. Remove an employee.\n"
                );
    }
};

template<>
struct std::formatter<ManagerEmployee>
{
    constexpr auto parse(std::format_parse_context &context) const
    {
        return context.begin();
    }
    
    auto format(ManagerEmployee const &employee, std::format_context &context) const
    {
        return std::format_to(context.out(),
                "\nLogged in as Manager Employee"
                "Please make a selection:\n"
                "0. Exit\n"
                "1. View employees.\n"
                "2. Search for an employee.\n"
                );
    }
};

#endif

