#ifndef ASSIGNMENT_DECORATOR_H
#define ASSIGNMENT_DECORATOR_H

#include "Assignment.h"
#include <string>
#include <memory>

// Base Assignment Decorator
class AssignmentDecorator : public Assignment {
protected:
    std::shared_ptr<Assignment> wrappedAssignment;
    
public:
    AssignmentDecorator(std::shared_ptr<Assignment> assignment);
    
    // Override Assignment's methods to delegate to wrapped assignment
    std::string getTitle() const override;
    std::string getDescription() const override;
    DateTime getDeadline() const override;
    std::shared_ptr<Course> getCourse() const override;
    std::vector<std::shared_ptr<Notification>> getNotifications() const override;
    bool isCompleted() const override;
    int getMarks() const override;
    
    void setTitle(const std::string& title) override;
    void setDescription(const std::string& description) override;
    void setDeadline(const DateTime& deadline) override;
    void setCourse(std::shared_ptr<Course> course) override;
    void setCompleted(bool completed) override;
    void setMarks(int marks) override;
    
    void addNotification(std::shared_ptr<Notification> notification) override;
    void removeNotification(size_t index) override;
    
    // These methods are pure virtual in the base class, so we need to implement them
    virtual void displayInfo() const override;
    virtual std::string getType() const override;
};

// PriorityAssignmentDecorator adds priority functionality
class PriorityAssignmentDecorator : public AssignmentDecorator {
private:
    int priorityLevel; // 1-5, with 5 being highest priority
    
public:
    PriorityAssignmentDecorator(std::shared_ptr<Assignment> assignment, int priorityLevel = 1);
    
    void setPriorityLevel(int level);
    int getPriorityLevel() const;
    
    void displayInfo() const override;
};

// TaggedAssignmentDecorator adds custom tags to assignments
class TaggedAssignmentDecorator : public AssignmentDecorator {
private:
    std::vector<std::string> tags;
    
public:
    TaggedAssignmentDecorator(std::shared_ptr<Assignment> assignment, 
                             const std::vector<std::string>& initialTags = {});
    
    void addTag(const std::string& tag);
    void removeTag(const std::string& tag);
    std::vector<std::string> getTags() const;
    
    void displayInfo() const override;
};

// ColoredAssignmentDecorator adds color-coding to assignments
class ColoredAssignmentDecorator : public AssignmentDecorator {
public:
    enum class Color {
        RED,    // Urgent
        YELLOW, // Important
        GREEN,  // Normal
        BLUE,   // Low priority
        PURPLE, // Special
        DEFAULT // No color
    };
    
private:
    Color color;
    
public:
    ColoredAssignmentDecorator(std::shared_ptr<Assignment> assignment, Color color = Color::DEFAULT);
    
    void setColor(Color color);
    Color getColor() const;
    std::string getColorName() const;
    
    void displayInfo() const override;
};

#endif // ASSIGNMENT_DECORATOR_H