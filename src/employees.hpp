//******************************************************************************
//File Name: employees.hpp
//Description: Employee base class for CS112: Assignment 1.
//Author: Austin Bachurski
//Created: January 20, 2026
//******************************************************************************

#ifndef EMPLOYEE_BASE_HPP
#define EMPLOYEE_BASE_HPP

#include <format>
#include <print>
#include <string>
#include <string_view>


class Employee
{
public:
    virtual ~Employee() = default;

    struct EmployeeBuilder
    {
        unsigned id;
        std::string_view name;
        std::string_view password;
    };

    unsigned getID() const { return m_id; }
    std::string_view getName() const { return m_name; } 
    std::string_view getPassword() const { return m_password; } 
    bool isCorrectPassword(std::string_view password) const { return password == m_password; }
    void setID(unsigned id) { m_id = id; }
    void setName(std::string_view name) { m_name = name; }
    void setPassword(std::string_view password) { m_password = password; }

    virtual void displayMenu() const = 0;
    virtual std::string_view getTitle() const = 0;
    virtual bool canViewEmployees() const = 0;
    virtual bool canSearchEmployees() const = 0;
    virtual bool canModifyEmployee() const = 0;
    virtual bool canAddEmployee() const = 0;
    virtual bool canRemoveEmployee() const = 0;

protected:
    Employee(EmployeeBuilder const &params)
    : m_id{ params.id }
    , m_name{ params.name }
    , m_password{ params.password }
    {}

private:
    unsigned m_id{};
    std::string m_name;
    std::string m_password;
};

class GeneralEmployee : public Employee
{
public:
    GeneralEmployee(EmployeeBuilder const &params)
    : Employee(params)
    {}

    void displayMenu() const override
    {
        std::println(
            "Logged in as General Employee: {}\n"
            "Please make a selection:\n"
            "0. Exit\n"
            "1. View employee file.\n",
            getName()
        );
    }

    std::string_view getTitle() const override  { return "General Employee"; }
    bool canViewEmployees()     const override  { return false; }
    bool canSearchEmployees()   const override  { return false; }
    bool canModifyEmployee()    const override  { return false; }
    bool canAddEmployee()       const override  { return false; }
    bool canRemoveEmployee()    const override  { return false; }
};

class HumanResourcesEmployee : public Employee
{
public:
    HumanResourcesEmployee(EmployeeBuilder const &params)
    : Employee(params)
    {}

    void displayMenu() const override
    {
        std::println(
            "Logged in as Human Resources Employee: {}\n"
            "Please make a selection:\n"
            "0. Exit\n"
            "1. View employees.\n"
            "2. Search for an employee.\n"
            "3. Modify an employee.\n"
            "4. Add an employee.\n"
            "5. Remove an employee.\n",
            getName()
        );
    }

    std::string_view getTitle() const override  { return "Human Resources Employee"; }
    bool canViewEmployees()     const override  { return true; }
    bool canSearchEmployees()   const override  { return true; }
    bool canModifyEmployee()    const override  { return true; }
    bool canAddEmployee()       const override  { return true; }
    bool canRemoveEmployee()    const override  { return true; }
};

class ManagerEmployee : public Employee
{
public:
    ManagerEmployee(EmployeeBuilder const &params)
    : Employee(params)
    {}

    void displayMenu() const override
    {
        std::println(
            "Logged in as Manager Employee: {}\n"
            "Please make a selection:\n"
            "0. Exit\n"
            "1. View employees.\n"
            "2. Search for an employee.\n",
            getName()
        );
    }

    std::string_view getTitle() const override  { return "Manager Employee"; }
    bool canViewEmployees()     const override  { return true; }
    bool canSearchEmployees()   const override  { return true; }
    bool canModifyEmployee()    const override  { return false; }
    bool canAddEmployee()       const override  { return false; }
    bool canRemoveEmployee()    const override  { return false; }
};

template<>
struct std::formatter<Employee>
{
    constexpr auto parse(std::format_parse_context &context)
    {
        return context.begin();
    }
    
    auto format(Employee const &employee, std::format_context &context) const
    {
        return std::format_to(context.out(),
                              "Employee ID: {}\n"
                              "Employee Name: {}\n"
                              "Employee Title: {}\n",
                              employee.getID(),
                              employee.getName(),
                              employee.getTitle()
                              );
    }
};

#endif

