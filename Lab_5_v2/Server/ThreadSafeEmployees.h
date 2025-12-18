#pragma once
#include "Employee.h"
#include <vector>
#include <memory>
#include <shared_mutex>
#include <algorithm>
#include <cstring>

class ThreadSafeEmployees {
private:
    std::vector<Employee> employees;
    mutable std::shared_mutex global_mutex;
    std::vector<std::unique_ptr<std::shared_mutex>> record_mutexes;

public:
    ThreadSafeEmployees() = default;

    ThreadSafeEmployees(const ThreadSafeEmployees&) = delete;
    ThreadSafeEmployees& operator=(const ThreadSafeEmployees&) = delete;

    ThreadSafeEmployees(ThreadSafeEmployees&&) = default;
    ThreadSafeEmployees& operator=(ThreadSafeEmployees&&) = default;

    bool read_employee(int id, Employee& emp) const {
        std::shared_lock<std::shared_mutex> global_lock(global_mutex);

        for (size_t i = 0; i < employees.size(); ++i) {
            if (employees[i].num == id) {
                std::shared_lock<std::shared_mutex> record_lock(*record_mutexes[i]);
                emp = employees[i];
                return true;
            }
        }
        return false;
    }

    bool write_employee(int id, const Employee& new_emp) {
        std::unique_lock<std::shared_mutex> global_lock(global_mutex);

        for (size_t i = 0; i < employees.size(); ++i) {
            if (employees[i].num == id) {
                std::unique_lock<std::shared_mutex> record_lock(*record_mutexes[i]);
                employees[i] = new_emp;
                return true;
            }
        }
        return false;
    }

    void add_employee(const Employee& emp) {
        std::unique_lock<std::shared_mutex> lock(global_mutex);
        employees.push_back(emp);
        record_mutexes.push_back(std::make_unique<std::shared_mutex>());
    }

    bool update_employee(int id, const Employee& new_emp) {
        return write_employee(id, new_emp);
    }

    bool delete_employee(int id) {
        std::unique_lock<std::shared_mutex> lock(global_mutex);

        for (size_t i = 0; i < employees.size(); ++i) {
            if (employees[i].num == id) {
                employees.erase(employees.begin() + i);
                record_mutexes.erase(record_mutexes.begin() + i);
                return true;
            }
        }
        return false;
    }

    std::vector<Employee> get_all() const {
        std::shared_lock<std::shared_mutex> lock(global_mutex);
        return employees;
    }

    size_t size() const {
        std::shared_lock<std::shared_mutex> lock(global_mutex);
        return employees.size();
    }

    void clear() {
        std::unique_lock<std::shared_mutex> lock(global_mutex);
        employees.clear();
        record_mutexes.clear();
    }

    bool exists(int id) const {
        std::shared_lock<std::shared_mutex> lock(global_mutex);
        for (const auto& emp : employees) {
            if (emp.num == id) return true;
        }
        return false;
    }

    Employee get_employee_copy(int id) const {
        Employee emp;
        if (read_employee(id, emp)) {
            return emp;
        }
        throw std::runtime_error("Employee not found");
    }
};