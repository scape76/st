#ifndef COMPOSITE_ASSIGNMENT_H
#define COMPOSITE_ASSIGNMENT_H

#include "Assignment.h"
#include <string>
#include <memory>
#include <vector>

// CompositeAssignment implements the Composite pattern for assignments
// It allows an assignment to be composed of multiple sub-assignments
class CompositeAssignment : public Assignment {
private:
    std::vector<std::shared_ptr<Assignment>> childAssignments;
    
public:
    // Constructor
    CompositeAssignment(const std::string& title, 
                       const DateTime& deadline, 
                       const std::string& description = "");
    
    // Destructor
    virtual ~CompositeAssignment() = default;
    
    // Add a child assignment
    void addChildAssignment(std::shared_ptr<Assignment> assignment);
    
    // Remove a child assignment by index
    void removeChildAssignment(size_t index);
    
    // Remove a child assignment by title
    void removeChildAssignment(const std::string& title);
    
    // Get all child assignments
    std::vector<std::shared_ptr<Assignment>> getChildAssignments() const;
    
    // Find a child assignment by title
    std::shared_ptr<Assignment> findChildAssignment(const std::string& title) const;
    
    // Override methods from base class
    void displayInfo() const override;
    std::string getType() const override { return "Composite"; }
    
    // Calculate overall completion status based on child assignments
    bool isCompleted() const override;
    
    // Calculate average marks based on completed child assignments
    int getMarks() const override;
    
    // Propagate status changes to children
    void setCompleted(bool completed) override;
    void setMarks(int marks) override;
    
    // Propagate course changes to children
    void setCourse(std::shared_ptr<Course> course) override;
    
    // Get total number of child assignments (recursively)
    int getTotalChildCount() const;
};

// Factory for creating composite assignments
class CompositeAssignmentFactory : public AssignmentFactory {
public:
    std::shared_ptr<Assignment> createAssignment(
        const std::string& title,
        const DateTime& deadline,
        const std::string& description = "") override;
};

#endif // COMPOSITE_ASSIGNMENT_H