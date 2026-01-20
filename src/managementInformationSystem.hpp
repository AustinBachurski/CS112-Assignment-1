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


class ManagementInformationSystem
{
public:
    void login();


private:
    std::vector<std::unique_ptr<Employee>> employees;
    Employee *loggedInUser{ nullptr };

};

#endif

