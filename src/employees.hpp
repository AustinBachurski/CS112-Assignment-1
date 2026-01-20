//******************************************************************************
//File Name: employee.hpp
//Description: Employee base class for CS112: Assignment 1.
//Author: Austin Bachurski
//Created: January 20, 2026
//******************************************************************************

#ifndef EMPLOYEE_BASE_HPP
#define EMPLOYEE_BASE_HPP

#include <string>
#include <string_view>


class Employee
{
public:
    virtual ~Employee() = default;

    std::string_view getName() { return name; }
    bool isCorrectPassword(std::string_view userPassword) { return userPassword == password; }

protected:
    Employee(std::string_view employeeName, std::string_view employeePassword)
    : name{ employeeName }
    , password{ employeePassword }
    {}

private:
    std::string name;
    std::string password;
};

class HumanResources : public Employee
{
public:
    HumanResources(std::string_view employeeName, std::string_view employeePassword)
    : Employee(employeeName, employeePassword)
    {}
};

#endif

