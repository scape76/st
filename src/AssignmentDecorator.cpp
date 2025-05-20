#include "AssignmentDecorator.h"
#include "Course.h"
#include <iostream>

// Base AssignmentDecorator implementation
AssignmentDecorator::AssignmentDecorator(std::shared_ptr<Assignment> assignment)
    : Assignment("", std::chrono::system_clock::now(), ""), wrappedAssignment(assignment) {
}

std::string AssignmentDecorator::getTitle() const {
    return wrappedAssignment->getTitle();
}

std::string AssignmentDecorator::getDescription() const {
    return wrappedAssignment->getDescription();
}

DateTime AssignmentDecorator::getDeadline() const {
    return wrappedAssignment->getDeadline();
}

std::shared_ptr<Course> AssignmentDecorator::getCourse() const {
    return wrappedAssignment->getCourse();
}

std::vector<std::shared_ptr<Notification>> AssignmentDecorator::getNotifications() const {
    return wrappedAssignment->getNotifications();
}

bool AssignmentDecorator::isCompleted() const {
    return wrappedAssignment->isCompleted();
}

int AssignmentDecorator::getMarks() const {
    return wrappedAssignment->getMarks();
}

void AssignmentDecorator::setTitle(const std::string& title) {
    wrappedAssignment->setTitle(title);
}

void AssignmentDecorator::setDescription(const std::string& description) {
    wrappedAssignment->setDescription(description);
}

void AssignmentDecorator::setDeadline(const DateTime& deadline) {
    wrappedAssignment->setDeadline(deadline);
}

void AssignmentDecorator::setCourse(std::shared_ptr<Course> course) {
    wrappedAssignment->setCourse(course);
}

void AssignmentDecorator::setCompleted(bool completed) {
    wrappedAssignment->setCompleted(completed);
}

void AssignmentDecorator::setMarks(int marks) {
    wrappedAssignment->setMarks(marks);
}

void AssignmentDecorator::addNotification(std::shared_ptr<Notification> notification) {
    wrappedAssignment->addNotification(notification);
}

void AssignmentDecorator::removeNotification(size_t index) {
    wrappedAssignment->removeNotification(index);
}

void AssignmentDecorator::displayInfo() const {
    wrappedAssignment->displayInfo();
}

std::string AssignmentDecorator::getType() const {
    return wrappedAssignment->getType();
}

// PriorityAssignmentDecorator implementation
PriorityAssignmentDecorator::PriorityAssignmentDecorator(std::shared_ptr<Assignment> assignment, int priorityLevel)
    : AssignmentDecorator(assignment), priorityLevel(priorityLevel) {
    // Ensure priority level is within valid range
    if (priorityLevel < 1) this->priorityLevel = 1;
    if (priorityLevel > 5) this->priorityLevel = 5;
}

void PriorityAssignmentDecorator::setPriorityLevel(int level) {
    priorityLevel = level;
    if (priorityLevel < 1) priorityLevel = 1;
    if (priorityLevel > 5) priorityLevel = 5;
}

int PriorityAssignmentDecorator::getPriorityLevel() const {
    return priorityLevel;
}

void PriorityAssignmentDecorator::displayInfo() const {
    wrappedAssignment->displayInfo();
    
    std::cout << "Priority: ";
    switch (priorityLevel) {
        case 1:
            std::cout << "Very Low";
            break;
        case 2:
            std::cout << "Low";
            break;
        case 3:
            std::cout << "Medium";
            break;
        case 4:
            std::cout << "High";
            break;
        case 5:
            std::cout << "Very High";
            break;
        default:
            std::cout << "Unknown";
    }
    std::cout << " (" << priorityLevel << ")" << std::endl;
}

// TaggedAssignmentDecorator implementation
TaggedAssignmentDecorator::TaggedAssignmentDecorator(std::shared_ptr<Assignment> assignment, 
                                                   const std::vector<std::string>& initialTags)
    : AssignmentDecorator(assignment), tags(initialTags) {
}

void TaggedAssignmentDecorator::addTag(const std::string& tag) {
    // Check if tag already exists
    for (const auto& existingTag : tags) {
        if (existingTag == tag) {
            return; // Tag already exists
        }
    }
    
    tags.push_back(tag);
}

void TaggedAssignmentDecorator::removeTag(const std::string& tag) {
    auto it = std::find(tags.begin(), tags.end(), tag);
    if (it != tags.end()) {
        tags.erase(it);
    }
}

std::vector<std::string> TaggedAssignmentDecorator::getTags() const {
    return tags;
}

void TaggedAssignmentDecorator::displayInfo() const {
    wrappedAssignment->displayInfo();
    
    if (!tags.empty()) {
        std::cout << "Tags: ";
        for (size_t i = 0; i < tags.size(); ++i) {
            std::cout << tags[i];
            if (i < tags.size() - 1) {
                std::cout << ", ";
            }
        }
        std::cout << std::endl;
    }
}

// ColoredAssignmentDecorator implementation
ColoredAssignmentDecorator::ColoredAssignmentDecorator(std::shared_ptr<Assignment> assignment, Color color)
    : AssignmentDecorator(assignment), color(color) {
}

void ColoredAssignmentDecorator::setColor(Color color) {
    this->color = color;
}

ColoredAssignmentDecorator::Color ColoredAssignmentDecorator::getColor() const {
    return color;
}

std::string ColoredAssignmentDecorator::getColorName() const {
    switch (color) {
        case Color::RED:
            return "Red";
        case Color::YELLOW:
            return "Yellow";
        case Color::GREEN:
            return "Green";
        case Color::BLUE:
            return "Blue";
        case Color::PURPLE:
            return "Purple";
        default:
            return "Default";
    }
}

void ColoredAssignmentDecorator::displayInfo() const {
    wrappedAssignment->displayInfo();
    std::cout << "Color: " << getColorName() << std::endl;
}