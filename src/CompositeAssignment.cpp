#include "CompositeAssignment.h"
#include "Course.h"
#include <iostream>
#include <algorithm>
#include <numeric>

// Constructor
CompositeAssignment::CompositeAssignment(const std::string& title, 
                                        const DateTime& deadline, 
                                        const std::string& description)
    : Assignment(title, deadline, description) {
}

// Add a child assignment
void CompositeAssignment::addChildAssignment(std::shared_ptr<Assignment> assignment) {
    // Don't add null assignments
    if (!assignment) {
        return;
    }
    
    // Check for duplicates
    auto it = std::find_if(childAssignments.begin(), childAssignments.end(),
                         [&](const std::shared_ptr<Assignment>& a) {
                             return a->getTitle() == assignment->getTitle();
                         });
    
    if (it == childAssignments.end()) {
        childAssignments.push_back(assignment);
        
        // Set the same course for the child assignment
        if (getCourse()) {
            assignment->setCourse(getCourse());
        }
    } else {
        std::cout << "Child assignment with title '" << assignment->getTitle() 
                  << "' already exists in this composite assignment." << std::endl;
    }
}

// Remove a child assignment by index
void CompositeAssignment::removeChildAssignment(size_t index) {
    if (index < childAssignments.size()) {
        childAssignments.erase(childAssignments.begin() + index);
    } else {
        std::cout << "Invalid child assignment index." << std::endl;
    }
}

// Remove a child assignment by title
void CompositeAssignment::removeChildAssignment(const std::string& title) {
    auto it = std::find_if(childAssignments.begin(), childAssignments.end(),
                         [&](const std::shared_ptr<Assignment>& a) {
                             return a->getTitle() == title;
                         });
    
    if (it != childAssignments.end()) {
        childAssignments.erase(it);
    } else {
        std::cout << "Child assignment with title '" << title 
                  << "' not found in this composite assignment." << std::endl;
    }
}

// Get all child assignments
std::vector<std::shared_ptr<Assignment>> CompositeAssignment::getChildAssignments() const {
    return childAssignments;
}

// Find a child assignment by title
std::shared_ptr<Assignment> CompositeAssignment::findChildAssignment(const std::string& title) const {
    auto it = std::find_if(childAssignments.begin(), childAssignments.end(),
                         [&](const std::shared_ptr<Assignment>& a) {
                             return a->getTitle() == title;
                         });
    
    return (it != childAssignments.end()) ? *it : nullptr;
}

// Override display to include child assignments
void CompositeAssignment::displayInfo() const {
    // First display this assignment's info using the base class method
    Assignment::displayInfo();
    
    // Then display child assignments
    std::cout << "Child Assignments: " << childAssignments.size() << std::endl;
    
    for (size_t i = 0; i < childAssignments.size(); ++i) {
        std::cout << "  " << (i + 1) << ". " << childAssignments[i]->getTitle() 
                  << " (" << childAssignments[i]->getType() << ")";
        
        if (childAssignments[i]->isCompleted()) {
            std::cout << " - Completed: " << childAssignments[i]->getMarks() << " marks";
        } else {
            std::cout << " - Not completed";
        }
        
        std::cout << std::endl;
    }
}

// Calculate overall completion status based on child assignments
bool CompositeAssignment::isCompleted() const {
    // If no child assignments, use the parent's completion status
    if (childAssignments.empty()) {
        return Assignment::isCompleted();
    }
    
    // Check if all child assignments are completed
    return std::all_of(childAssignments.begin(), childAssignments.end(),
                      [](const std::shared_ptr<Assignment>& a) {
                          return a->isCompleted();
                      });
}

// Calculate average marks based on completed child assignments
int CompositeAssignment::getMarks() const {
    // If no child assignments, use the parent's marks
    if (childAssignments.empty()) {
        return Assignment::getMarks();
    }
    
    // Count completed assignments and sum their marks
    int completedCount = 0;
    int totalMarks = 0;
    
    for (const auto& assignment : childAssignments) {
        if (assignment->isCompleted()) {
            totalMarks += assignment->getMarks();
            completedCount++;
        }
    }
    
    // Calculate average marks
    return completedCount > 0 ? totalMarks / completedCount : 0;
}

// Propagate status changes to children
void CompositeAssignment::setCompleted(bool completed) {
    // Set the parent's status
    Assignment::setCompleted(completed);
    
    // Propagate to all children
    for (auto& assignment : childAssignments) {
        assignment->setCompleted(completed);
    }
}

void CompositeAssignment::setMarks(int marks) {
    // Set the parent's marks
    Assignment::setMarks(marks);
    
    // Only propagate marks if marking as completed with uniform marks
    if (isCompleted()) {
        for (auto& assignment : childAssignments) {
            assignment->setMarks(marks);
        }
    }
}

// Propagate course changes to children
void CompositeAssignment::setCourse(std::shared_ptr<Course> course) {
    // Set the parent's course
    Assignment::setCourse(course);
    
    // Propagate to all children
    for (auto& assignment : childAssignments) {
        assignment->setCourse(course);
    }
}

// Get total number of child assignments (recursively)
int CompositeAssignment::getTotalChildCount() const {
    int count = childAssignments.size();
    
    // Count child assignments of composite children
    for (const auto& assignment : childAssignments) {
        if (auto compositeAssignment = std::dynamic_pointer_cast<CompositeAssignment>(assignment)) {
            count += compositeAssignment->getTotalChildCount();
        }
    }
    
    return count;
}

// Factory for creating composite assignments
std::shared_ptr<Assignment> CompositeAssignmentFactory::createAssignment(
    const std::string& title,
    const DateTime& deadline,
    const std::string& description) {
    return std::make_shared<CompositeAssignment>(title, deadline, description);
}