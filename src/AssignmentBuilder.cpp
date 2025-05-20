#include "AssignmentBuilder.h"
#include <stdexcept>

// Constructor
AssignmentBuilder::AssignmentBuilder() {
    reset();
}

// Fluent interface methods
AssignmentBuilder& AssignmentBuilder::setTitle(const std::string& title) {
    this->title = title;
    return *this;
}

AssignmentBuilder& AssignmentBuilder::setDescription(const std::string& description) {
    this->description = description;
    return *this;
}

AssignmentBuilder& AssignmentBuilder::setDeadline(const DateTime& deadline) {
    this->deadline = deadline;
    return *this;
}

// Set which type of assignment to build
AssignmentBuilder& AssignmentBuilder::asHomework() {
    factory = std::make_shared<HomeworkFactory>();
    return *this;
}

AssignmentBuilder& AssignmentBuilder::asProject() {
    factory = std::make_shared<ProjectFactory>();
    return *this;
}

AssignmentBuilder& AssignmentBuilder::asExam() {
    factory = std::make_shared<ExamFactory>();
    return *this;
}

// Build the assignment
std::shared_ptr<Assignment> AssignmentBuilder::build() const {
    // Validate required fields
    if (title.empty()) {
        throw std::invalid_argument("Assignment title is required");
    }
    
    if (factory == nullptr) {
        throw std::invalid_argument("Assignment type (homework/project/exam) must be specified");
    }
    
    // Create the assignment using the factory
    return factory->createAssignment(title, deadline, description);
}

// Reset the builder to create another assignment
void AssignmentBuilder::reset() {
    title.clear();
    description.clear();
    deadline = DateTime();
    factory = nullptr;
}