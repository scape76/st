#include "Observer.h"
#include "Assignment.h"
#include "Course.h"
#include <iostream>
#include <chrono>

// StudentObserver implementation
void StudentObserver::update(const std::string& message, void* subject) {
    std::cout << "[STUDENT NOTIFICATION] " << studentName << " (ID: " << studentId << "): " << message << std::endl;
    
    // Handle different subjects
    if (auto assignment = static_cast<Assignment*>(subject)) {
        std::cout << "  Assignment: " << assignment->getTitle() << std::endl;
        if (auto course = assignment->getCourse()) {
            std::cout << "  Course: " << course->getName() << " (" << course->getCode() << ")" << std::endl;
        }
    } else if (auto course = static_cast<Course*>(subject)) {
        std::cout << "  Course: " << course->getName() << " (" << course->getCode() << ")" << std::endl;
    }
}

// DeadlineObserver implementation
void DeadlineObserver::update(const std::string& message, void* subject) {
    if (auto assignment = static_cast<Assignment*>(subject)) {
        auto now = std::chrono::system_clock::now();
        auto deadline = assignment->getDeadline();
        
        // Calculate days until deadline
        auto daysUntilDeadline = std::chrono::duration_cast<std::chrono::hours>(deadline - now).count() / 24;
        
        if (daysUntilDeadline <= daysWarning && daysUntilDeadline >= 0) {
            std::cout << "[DEADLINE WARNING] " << message << std::endl;
            std::cout << "  Assignment: " << assignment->getTitle() << " is due in " << daysUntilDeadline << " days." << std::endl;
            
            if (auto course = assignment->getCourse()) {
                std::cout << "  Course: " << course->getName() << " (" << course->getCode() << ")" << std::endl;
            }
        } else if (daysUntilDeadline < 0) {
            // Assignment is overdue
            std::cout << "[OVERDUE NOTICE] " << message << std::endl;
            std::cout << "  Assignment: " << assignment->getTitle() << " is overdue by " << -daysUntilDeadline << " days." << std::endl;
            
            if (auto course = assignment->getCourse()) {
                std::cout << "  Course: " << course->getName() << " (" << course->getCode() << ")" << std::endl;
            }
        }
    }
}

// GradeObserver implementation
void GradeObserver::update(const std::string& message, void* subject) {
    std::cout << "[GRADE NOTIFICATION] " << message << std::endl;
    
    if (auto assignment = static_cast<Assignment*>(subject)) {
        std::cout << "  Assignment: " << assignment->getTitle() << std::endl;
        std::cout << "  Grade: " << assignment->getMarks() << " / 100" << std::endl;
        
        if (auto course = assignment->getCourse()) {
            std::cout << "  Course: " << course->getName() << " (" << course->getCode() << ")" << std::endl;
        }
    } else if (auto course = static_cast<Course*>(subject)) {
        std::cout << "  Course: " << course->getName() << " (" << course->getCode() << ")" << std::endl;
        // For final course grades, we'd need additional information not currently available
    }
}

// ProgressObserver implementation
void ProgressObserver::update(const std::string& message, void* subject) {
    if (auto assignment = static_cast<Assignment*>(subject)) {
        float currentProgress = assignment->getProgress();
        
        if (currentProgress != lastRecordedProgress) {
            std::cout << "[PROGRESS UPDATE] " << message << std::endl;
            std::cout << "  Assignment: " << assignment->getTitle() << std::endl;
            std::cout << "  Progress: " << lastRecordedProgress << "% -> " << currentProgress << "%" << std::endl;
            
            if (auto course = assignment->getCourse()) {
                std::cout << "  Course: " << course->getName() << " (" << course->getCode() << ")" << std::endl;
            }
            
            lastRecordedProgress = currentProgress;
        }
    }
}

// ObservableAssignment implementation
void ObservableAssignment::deadlineApproaching() {
    notify("Deadline is approaching!");
}

void ObservableAssignment::stateChanged(const std::string& newState) {
    notify("Assignment state changed to: " + newState);
}

void ObservableAssignment::progressUpdated(float newProgress) {
    notify("Progress updated to " + std::to_string(int(newProgress)) + "%");
}

void ObservableAssignment::gradeAssigned(int marks) {
    notify("Grade assigned: " + std::to_string(marks) + " marks");
}

// ObservableCourse implementation
void ObservableCourse::assignmentAdded(const std::string& assignmentTitle) {
    notify("New assignment added: " + assignmentTitle);
}

void ObservableCourse::courseCompleted() {
    notify("Course has been completed!");
}

void ObservableCourse::finalGradeAssigned(const std::string& grade) {
    notify("Final grade assigned: " + grade);
}