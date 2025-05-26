#ifndef COMPOSITE_TASK_H
#define COMPOSITE_TASK_H

#include "Task.h"
#include <string>
#include <memory>
#include <vector>

class CompositeTask : public Task {
private:
    std::vector<std::shared_ptr<Task>> childTasks;
    
public:
    CompositeTask(const std::string& title, 
                       const DateTime& deadline, 
                       const std::string& description = "");
    
    virtual ~CompositeTask() = default;
    
    void addChildTask(std::shared_ptr<Task> task);
    
    void removeChildTask(size_t index);
    
    void removeChildTask(const std::string& title);
    
    std::vector<std::shared_ptr<Task>> getChildTasks() const;
    
    std::shared_ptr<Task> findChildTask(const std::string& title) const;
    
    void displayInfo() const override;
    std::string getType() const override { return "Composite"; }
    
    bool isCompleted() const override;
    
    int getMarks() const override;
    
    void setCompleted(bool completed) override;
    void setMarks(int marks) override;
    
    void setSubject(std::shared_ptr<Subject> subject) override;
    
    int getTotalChildCount() const;
};

class CompositeTaskFactory : public TaskFactory {
public:
    std::shared_ptr<Task> createTask(
        const std::string& title,
        const DateTime& deadline,
        const std::string& description = "") override;
};

#endif