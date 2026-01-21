//******************************************************************************
//File Name: managementInformationSystem.hpp
//Description: Object for managing employee information.
//Author: Austin Bachurski
//Created: January 20, 2026
//******************************************************************************

#ifndef MANAGEMENT_INFORMATION_SYSTEM_HPP
#define MANAGEMENT_INFORMATION_SYSTEM_HPP

#include "employees.hpp"

#include <memory>
#include <vector>


// Management class.
class ManagementInformationSystem
{
public:
    // Public function to log in to the management system.
    void login();

private:
    // Displays and selects menu actions.
    void displayMenu();

    // Displays employees based on user permissions.
    void viewEmployees() const;

    // Allows the user to search for employees if permissions are sufficient.
    void searchEmployees() const;

    // Allows the user to modify an employee if permissions are sufficient.
    void modifyEmployee();

    // Allows the user to add an employee if permissions are sufficient.
    void addEmployee();

    // Allows the user to remove an employee if permissions are sufficient.
    void removeEmployee();

    // Vector of Employee objects that serves as the pseudo-database for the exercise.
    std::vector<std::unique_ptr<Employee>> employees;

    // The currently logged in user.
    Employee *loggedInUser{ nullptr };

};

#endif

