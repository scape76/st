#include "Course.h"
#include "Assignment.h"
#include <iostream>
#include <algorithm>

// Course implementation
Course::Course(const std::string& name, const std::string& code, const std::string& description)
    : name(name), code(code), description(description) {}

std::string Course::getName() const {
    return name;
}

std::string Course::getCode() const {
    return code;
}

std::string Course::getDescription() const {
    return description;
}

std::vector<std::shared_ptr<Assignment>> Course::getAssignments() const {
    return assignments;
}

void Course::addAssignment(std::shared_ptr<Assignment> assignment) {
    // Avoid duplicates and set the course reference in the assignment
    auto it = std::find_if(assignments.begin(), assignments.end(),
                        [&](const std::shared_ptr<Assignment>& a) {
                            return a->getTitle() == assignment->getTitle();
                        });
    
    if (it == assignments.end()) {
        assignments.push_back(assignment);
        assignment->setCourse(std::shared_ptr<Course>(this, [](Course*){})); // Non-owning reference
    } else {
        std::cout << "Assignment with title '" << assignment->getTitle() 
                  << "' already exists in this course." << std::endl;
    }
}

void Course::removeAssignment(const std::string& title) {
    auto it = std::find_if(assignments.begin(), assignments.end(),
                        [&](const std::shared_ptr<Assignment>& a) {
                            return a->getTitle() == title;
                        });
    
    if (it != assignments.end()) {
        assignments.erase(it);
    } else {
        std::cout << "Assignment with title '" << title 
                  << "' not found in this course." << std::endl;
    }
}

std::shared_ptr<Assignment> Course::findAssignment(const std::string& title) const {
    auto it = std::find_if(assignments.begin(), assignments.end(),
                        [&](const std::shared_ptr<Assignment>& a) {
                            return a->getTitle() == title;
                        });
    
    return (it != assignments.end()) ? *it : nullptr;
}

void Course::displayInfo() const {
    std::cout << "Course: " << name << " (" << code << ")" << std::endl;
    
    if (!description.empty()) {
        std::cout << "Description: " << description << std::endl;
    }
    
    std::cout << "Assignments: " << assignments.size() << std::endl;
    
    for (const auto& assignment : assignments) {
        std::cout << "  - " << assignment->getTitle() << " (" << assignment->getType() << ")" << std::endl;
    }
}

// Memento pattern methods
std::shared_ptr<CourseMemento> Course::createMemento() const {
    std::vector<std::string> assignmentTitles;
    for (const auto& assignment : assignments) {
        assignmentTitles.push_back(assignment->getTitle());
    }
    
    return std::make_shared<CourseMemento>(
        name,
        code,
        description,
        assignmentTitles
    );
}

void Course::restoreFromMemento(const std::shared_ptr<CourseMemento>& memento) {
    name = memento->getName();
    code = memento->getCode();
    description = memento->getDescription();
    
    // Note: Assignment references can't be fully restored from the memento
    // as we only store titles. The caller must reconnect assignments separately.
}

// CourseBuilder implementation
CourseBuilder& CourseBuilder::setName(const std::string& name) {
    this->name = name;
    return *this;
}

CourseBuilder& CourseBuilder::setCode(const std::string& code) {
    this->code = code;
    return *this;
}

CourseBuilder& CourseBuilder::setDescription(const std::string& description) {
    this->description = description;
    return *this;
}

std::shared_ptr<Course> CourseBuilder::build() const {
    // Validate required fields
    if (name.empty() || code.empty()) {
        throw std::invalid_argument("Course name and code are required");
    }
    
    return std::make_shared<Course>(name, code, description);
}