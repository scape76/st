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
    std::tm tm_local = {};
    
    #ifdef _WIN32
    localtime_s(&tm_local, &time);
    #else
    tm_local = *std::localtime(&time);
    #endif
    
    // Apply timezone offset correction to keep original input time
    std::time_t now = std::time(nullptr);
    std::tm tm_now = {};
    
    #ifdef _WIN32
    gmtime_s(&tm_now, &now);
    #else
    tm_now = *std::gmtime(&now);
    #endif
    
    std::time_t gmt = std::mktime(&tm_now);
    std::time_t local = std::time(nullptr);
    double diff = std::difftime(local, gmt);
    
    // Adjust the time to ensure it displays as entered
    std::stringstream ss;
    ss << std::put_time(&tm_local, "%Y-%m-%d %H:%M");
    return ss.str();
}

// Base Assignment implementation
Assignment::Assignment(const std::string& title, 
                       const DateTime& deadline, 
                       const std::string& description)
    : title(title), description(description), deadline(deadline), 
      completed(false), marks(0), progress(0.0f) {
    // Initialize with NotStartedState
    state = NotStartedState::getInstance();
    state->setContext(this);
}

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

bool Assignment::isCompleted() const {
    return completed;
}

int Assignment::getMarks() const {
    return marks;
}

float Assignment::getProgress() const {
    return progress;
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

void Assignment::setCompleted(bool completed) {
    this->completed = completed;
    
    if (completed) {
        // Update state through state pattern
        state->complete(marks);
    } else if (state->getName() == "Completed") {
        // Revert to not started state if uncompleting a completed assignment
        setState(NotStartedState::getInstance());
    }
}

void Assignment::setMarks(int marks) {
    this->marks = marks;
}

void Assignment::setProgress(float progress) {
    this->progress = progress;
}

// State pattern methods
void Assignment::setState(std::shared_ptr<AssignmentState> newState) {
    this->state = newState;
    this->state->setContext(this);
}

std::shared_ptr<AssignmentState> Assignment::getState() const {
    return state;
}

void Assignment::startAssignment() {
    state->start();
}

void Assignment::updateProgress(float percentage) {
    state->makeProgress(percentage);
}

void Assignment::checkDeadline() {
    state->checkDeadline();
}

std::string Assignment::getStateName() const {
    return state->getName();
}

std::string Assignment::getStateDescription() const {
    return state->getDescription();
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
    
    std::cout << "Status: " << getStateName() << std::endl;
    
    if (state->getName() == "In Progress") {
        std::cout << "Progress: " << progress << "%" << std::endl;
    } else if (state->getName() == "Completed") {
        std::cout << "Marks: " << marks << std::endl;
    }
    
    std::cout << "Notifications: " << notifications.size() << std::endl;
}

// Memento pattern methods
std::shared_ptr<AssignmentMemento> Assignment::createMemento() const {
    std::string courseCode = "";
    if (auto coursePtr = getCourse()) {
        courseCode = coursePtr->getCode();
    }
    
    return std::make_shared<AssignmentMemento>(
        title,
        description,
        deadline,
        completed,
        marks,
        progress,
        state->getName(),
        courseCode
    );
}

void Assignment::restoreFromMemento(const std::shared_ptr<AssignmentMemento>& memento) {
    title = memento->getTitle();
    description = memento->getDescription();
    deadline = memento->getDeadline();
    completed = memento->isCompleted();
    marks = memento->getMarks();
    progress = memento->getProgress();
    
    // Restore state based on the state name
    std::string stateName = memento->getState();
    if (stateName == "Not Started") {
        setState(NotStartedState::getInstance());
    } else if (stateName == "In Progress") {
        setState(std::make_shared<InProgressState>(progress));
    } else if (stateName == "Completed") {
        setState(std::make_shared<CompletedState>(marks));
    } else if (stateName == "Overdue") {
        setState(std::make_shared<OverdueState>(deadline));
    }
    
    // Note: The course reference will need to be reconnected separately
    // since we only store the course code in the memento
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