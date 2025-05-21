#ifndef COMPOSITE_TASK_H
#define COMPOSITE_TASK_H

#include "Task.h"
#include <string>
#include <memory>
#include <vector>

// CompositeTask implements the Composite pattern for tasks
// It allows a task to be composed of multiple sub-tasks
class CompositeTask : public Task {
private:
    std::vector<std::shared_ptr<Task>> childTasks;
    
public:
    // Constructor
    CompositeTask(const std::string& title, 
                       const DateTime& deadline, 
                       const std::string& description = "");
    
    // Destructor
    virtual ~CompositeTask() = default;
    
    // Add a child task
    void addChildTask(std::shared_ptr<Task> task);
    
    // Remove a child task by index
    void removeChildTask(size_t index);
    
    // Remove a child task by title
    void removeChildTask(const std::string& title);
    
    // Get all child tasks
    std::vector<std::shared_ptr<Task>> getChildTasks() const;
    
    // Find a child task by title
    std::shared_ptr<Task> findChildTask(const std::string& title) const;
    
    // Override methods from base class
    void displayInfo() const override;
    std::string getType() const override { return "Composite"; }
    
    // Calculate overall completion status based on child tasks
    bool isCompleted() const override;
    
    // Calculate average marks based on completed child tasks
    int getMarks() const override;
    
    // Propagate status changes to children
    void setCompleted(bool completed) override;
    void setMarks(int marks) override;
    
    // Propagate subject changes to children
    void setSubject(std::shared_ptr<Subject> subject) override;
    
    // Get total number of child tasks (recursively)
    int getTotalChildCount() const;
};

// Factory for creating composite tasks
class CompositeTaskFactory : public TaskFactory {
public:
    std::shared_ptr<Task> createTask(
        const std::string& title,
        const DateTime& deadline,
        const std::string& description = "") override;
};

#endif // COMPOSITE_TASK_H