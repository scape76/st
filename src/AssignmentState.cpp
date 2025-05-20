#include "AssignmentState.h"
#include "Assignment.h"
#include <iostream>

// Initialize the static instance for NotStartedState
std::shared_ptr<NotStartedState> NotStartedState::instance = nullptr;

// Factory method to get the singleton instance
std::shared_ptr<AssignmentState> NotStartedState::getInstance() {
    if (!instance) {
        instance = std::shared_ptr<NotStartedState>(new NotStartedState());
    }
    return instance;
}

// NotStartedState implementation
void NotStartedState::start() {
    std::cout << "Starting assignment '" << context->getTitle() << "'." << std::endl;
    context->setState(std::make_shared<InProgressState>(0.0f));
}

void NotStartedState::makeProgress(float percentage) {
    std::cout << "Cannot make progress on an assignment that hasn't been started yet." << std::endl;
    std::cout << "Starting assignment '" << context->getTitle() << "' with " << percentage << "% progress." << std::endl;
    context->setState(std::make_shared<InProgressState>(percentage));
}

void NotStartedState::complete(int marks) {
    std::cout << "Marking assignment '" << context->getTitle() << "' as completed directly." << std::endl;
    context->setState(std::make_shared<CompletedState>(marks));
}

void NotStartedState::checkDeadline() {
    auto now = std::chrono::system_clock::now();
    if (now > context->getDeadline()) {
        std::cout << "Assignment '" << context->getTitle() << "' is now overdue." << std::endl;
        context->setState(std::make_shared<OverdueState>(now));
    }
}

void NotStartedState::handleNotification() {
    std::cout << "NOTIFICATION: Assignment '" << context->getTitle() << "' hasn't been started yet!" << std::endl;
    std::cout << "Due date is approaching." << std::endl;
}

// InProgressState implementation
void InProgressState::start() {
    std::cout << "Assignment '" << context->getTitle() << "' is already in progress." << std::endl;
}

void InProgressState::makeProgress(float percentage) {
    if (percentage < 0.0f) percentage = 0.0f;
    if (percentage > 100.0f) percentage = 100.0f;
    
    std::cout << "Updating progress on assignment '" << context->getTitle() 
              << "' from " << progressPercentage << "% to " << percentage << "%." << std::endl;
    progressPercentage = percentage;
    context->setProgress(percentage);
    
    // If progress reaches 100%, transition to completed state
    if (progressPercentage >= 100.0f) {
        std::cout << "Assignment '" << context->getTitle() << "' is now complete!" << std::endl;
        context->setState(std::make_shared<CompletedState>(context->getMarks()));
    }
}

void InProgressState::complete(int marks) {
    std::cout << "Marking assignment '" << context->getTitle() << "' as completed with " << marks << " marks." << std::endl;
    context->setState(std::make_shared<CompletedState>(marks));
}

void InProgressState::checkDeadline() {
    auto now = std::chrono::system_clock::now();
    if (now > context->getDeadline()) {
        std::cout << "Assignment '" << context->getTitle() << "' is now overdue." << std::endl;
        context->setState(std::make_shared<OverdueState>(now));
    }
}

void InProgressState::handleNotification() {
    std::cout << "NOTIFICATION: Assignment '" << context->getTitle() << "' is " 
              << progressPercentage << "% complete." << std::endl;
    std::cout << "Keep working to finish before the deadline!" << std::endl;
}

// CompletedState implementation
void CompletedState::start() {
    std::cout << "Cannot start assignment '" << context->getTitle() << "' as it is already completed." << std::endl;
}

void CompletedState::makeProgress(float percentage) {
    std::cout << "Cannot update progress on assignment '" << context->getTitle() << "' as it is already completed." << std::endl;
}

void CompletedState::complete(int marks) {
    if (marks != achievedMarks) {
        std::cout << "Updating marks for completed assignment '" << context->getTitle() 
                  << "' from " << achievedMarks << " to " << marks << "." << std::endl;
        achievedMarks = marks;
        context->setMarks(marks);
    } else {
        std::cout << "Assignment '" << context->getTitle() << "' is already completed with " 
                  << marks << " marks." << std::endl;
    }
}

void CompletedState::checkDeadline() {
    // No state change needed for completed assignments regardless of deadline
    std::cout << "Assignment '" << context->getTitle() << "' is completed, so deadline check is not relevant." << std::endl;
}

void CompletedState::handleNotification() {
    std::cout << "NOTIFICATION: Assignment '" << context->getTitle() << "' has been completed with " 
              << achievedMarks << " marks." << std::endl;
}

// OverdueState implementation
void OverdueState::start() {
    std::cout << "Starting overdue assignment '" << context->getTitle() << "'." << std::endl;
    context->setState(std::make_shared<InProgressState>(0.0f));
}

void OverdueState::makeProgress(float percentage) {
    std::cout << "Working on overdue assignment '" << context->getTitle() << "'." << std::endl;
    context->setState(std::make_shared<InProgressState>(percentage));
}

void OverdueState::complete(int marks) {
    std::cout << "Completing overdue assignment '" << context->getTitle() << "' with " << marks << " marks." << std::endl;
    // Even though it's late, we can still complete it
    context->setState(std::make_shared<CompletedState>(marks));
}

void OverdueState::checkDeadline() {
    // Already overdue, no state change
    std::cout << "Assignment '" << context->getTitle() << "' is already overdue." << std::endl;
}

void OverdueState::handleNotification() {
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::hours>(now - context->getDeadline()).count();
    
    std::cout << "URGENT NOTIFICATION: Assignment '" << context->getTitle() << "' is overdue by ";
    
    if (duration < 24) {
        std::cout << duration << " hours." << std::endl;
    } else {
        std::cout << (duration / 24) << " days and " << (duration % 24) << " hours." << std::endl;
    }
    
    std::cout << "Please complete it as soon as possible!" << std::endl;
}