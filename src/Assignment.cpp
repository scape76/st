#include "Assignment.h"
#include "Course.h"
#include "Notification.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>

// Helper function to format DateTime
std::string formatDateTime(const DateTime& dt) {
    auto time = std::chrono::system_clock::to_time_t(dt);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M");
    return ss.str();
}

// Base Assignment implementation
Assignment::Assignment(const std::string& title, 
                       const DateTime& deadline, 
                       const std::string& description)
    : title(title), description(description), deadline(deadline) {}

std::string Assignment::getTitle() const {
    return title;
}

std::string Assignment::getDescription() const {
    return description;
}

DateTime Assignment::getDeadline() const {
    return deadline;
}

std::shared_ptr<Course> Assignment::getCourse() const {
    return course;
}

std::vector<std::shared_ptr<Notification>> Assignment::getNotifications() const {
    return notifications;
}

void Assignment::setTitle(const std::string& title) {
    this->title = title;
}

void Assignment::setDescription(const std::string& description) {
    this->description = description;
}

void Assignment::setDeadline(const DateTime& deadline) {
    this->deadline = deadline;
}

void Assignment::setCourse(std::shared_ptr<Course> course) {
    this->course = course;
}

void Assignment::addNotification(std::shared_ptr<Notification> notification) {
    notifications.push_back(notification);
}

void Assignment::removeNotification(size_t index) {
    if (index < notifications.size()) {
        notifications.erase(notifications.begin() + index);
    } else {
        std::cout << "Invalid notification index." << std::endl;
    }
}

void Assignment::displayInfo() const {
    std::cout << "Assignment: " << title << " (" << getType() << ")" << std::endl;
    
    if (!description.empty()) {
        std::cout << "Description: " << description << std::endl;
    }
    
    std::cout << "Deadline: " << formatDateTime(deadline) << std::endl;
    
    if (course) {
        std::cout << "Course: " << course->getName() << " (" << course->getCode() << ")" << std::endl;
    } else {
        std::cout << "Not assigned to any course" << std::endl;
    }
    
    std::cout << "Notifications: " << notifications.size() << std::endl;
}

// HomeworkAssignment implementation
HomeworkAssignment::HomeworkAssignment(const std::string& title, 
                                     const DateTime& deadline, 
                                     const std::string& description)
    : Assignment(title, deadline, description) {}

// ProjectAssignment implementation
ProjectAssignment::ProjectAssignment(const std::string& title, 
                                   const DateTime& deadline, 
                                   const std::string& description)
    : Assignment(title, deadline, description) {}

// ExamAssignment implementation
ExamAssignment::ExamAssignment(const std::string& title, 
                             const DateTime& deadline, 
                             const std::string& description)
    : Assignment(title, deadline, description) {}

// Factory implementations
std::shared_ptr<Assignment> HomeworkFactory::createAssignment(
    const std::string& title,
    const DateTime& deadline,
    const std::string& description) {
    return std::make_shared<HomeworkAssignment>(title, deadline, description);
}

std::shared_ptr<Assignment> ProjectFactory::createAssignment(
    const std::string& title,
    const DateTime& deadline,
    const std::string& description) {
    return std::make_shared<ProjectAssignment>(title, deadline, description);
}

std::shared_ptr<Assignment> ExamFactory::createAssignment(
    const std::string& title,
    const DateTime& deadline,
    const std::string& description) {
    return std::make_shared<ExamAssignment>(title, deadline, description);
}