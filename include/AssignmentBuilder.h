#ifndef ASSIGNMENT_BUILDER_H
#define ASSIGNMENT_BUILDER_H

#include "Assignment.h"
#include <string>
#include <memory>
#include <chrono>

// Builder Pattern for creating assignments
class AssignmentBuilder {
private:
    std::string title;
    std::string description;
    DateTime deadline;
    std::shared_ptr<AssignmentFactory> factory;

public:
    // Constructor
    AssignmentBuilder();
    
    // Fluent interface methods
    AssignmentBuilder& setTitle(const std::string& title);
    AssignmentBuilder& setDescription(const std::string& description);
    AssignmentBuilder& setDeadline(const DateTime& deadline);
    
    // Set which type of assignment to build
    AssignmentBuilder& asHomework();
    AssignmentBuilder& asProject();
    AssignmentBuilder& asExam();
    
    // Build the assignment
    std::shared_ptr<Assignment> build() const;
    
    // Reset the builder to create another assignment
    void reset();
};

#endif // ASSIGNMENT_BUILDER_H