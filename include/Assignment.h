#ifndef ASSIGNMENT_H
#define ASSIGNMENT_H

#include <string>
#include <memory>
#include <chrono>
#include <vector>
#include <iostream>
#include "AssignmentState.h"
#include "Memento.h"

// Forward declaration
class Course;
class Notification;
class AssignmentState;
class AssignmentMemento;

// Using a type alias for date/time
using DateTime = std::chrono::system_clock::time_point;

// Assignment class
class Assignment {
private:
    std::string title;
    std::string description;
    DateTime deadline;
    std::shared_ptr<Course> course;
    std::vector<std::shared_ptr<Notification>> notifications;
    bool completed;
    int marks;
    float progress;
    std::shared_ptr<AssignmentState> state;
    friend class AssignmentState;
    friend class NotStartedState;
    friend class InProgressState;
    friend class CompletedState;
    friend class OverdueState;

public:
    // Constructor
    Assignment(const std::string& title, 
               const DateTime& deadline, 
               const std::string& description = "");
    
    // Virtual destructor for polymorphism
    virtual ~Assignment() = default;
    
    // State pattern methods
    void setState(std::shared_ptr<AssignmentState> state);
    std::shared_ptr<AssignmentState> getState() const;
    void startAssignment();
    void updateProgress(float percentage);
    void checkDeadline();
    std::string getStateName() const;
    std::string getStateDescription() const;
    
    // Getters
    virtual std::string getTitle() const;
    virtual std::string getDescription() const;
    virtual DateTime getDeadline() const;
    virtual std::shared_ptr<Course> getCourse() const;
    virtual std::vector<std::shared_ptr<Notification>> getNotifications() const;
    virtual bool isCompleted() const;
    virtual int getMarks() const;
    virtual float getProgress() const;
    
    // Setters
    virtual void setTitle(const std::string& title);
    virtual void setDescription(const std::string& description);
    virtual void setDeadline(const DateTime& deadline);
    virtual void setCourse(std::shared_ptr<Course> course);
    virtual void setCompleted(bool completed);
    virtual void setMarks(int marks);
    virtual void setProgress(float progress);
    
    // Notification management
    virtual void addNotification(std::shared_ptr<Notification> notification);
    virtual void removeNotification(size_t index);
    
    // Display assignment information
    virtual void displayInfo() const;
    
    // Assignment types
    virtual std::string getType() const = 0;
    
    // Memento pattern methods
    std::shared_ptr<AssignmentMemento> createMemento() const;
    void restoreFromMemento(const std::shared_ptr<AssignmentMemento>& memento);
};

// Concrete Assignment types
class HomeworkAssignment : public Assignment {
public:
    HomeworkAssignment(const std::string& title, 
                      const DateTime& deadline, 
                      const std::string& description = "");
    
    std::string getType() const override { return "Homework"; }
};

class ProjectAssignment : public Assignment {
public:
    ProjectAssignment(const std::string& title, 
                     const DateTime& deadline, 
                     const std::string& description = "");
    
    std::string getType() const override { return "Project"; }
};

class ExamAssignment : public Assignment {
public:
    ExamAssignment(const std::string& title, 
                  const DateTime& deadline, 
                  const std::string& description = "");
    
    std::string getType() const override { return "Exam"; }
};

// Assignment Factory (Factory Method Pattern)
class AssignmentFactory {
public:
    virtual std::shared_ptr<Assignment> createAssignment(
        const std::string& title,
        const DateTime& deadline,
        const std::string& description = "") = 0;
    
    virtual ~AssignmentFactory() = default;
};

// Concrete factories
class HomeworkFactory : public AssignmentFactory {
public:
    std::shared_ptr<Assignment> createAssignment(
        const std::string& title,
        const DateTime& deadline,
        const std::string& description = "") override;
};

class ProjectFactory : public AssignmentFactory {
public:
    std::shared_ptr<Assignment> createAssignment(
        const std::string& title,
        const DateTime& deadline,
        const std::string& description = "") override;
};

class ExamFactory : public AssignmentFactory {
public:
    std::shared_ptr<Assignment> createAssignment(
        const std::string& title,
        const DateTime& deadline,
        const std::string& description = "") override;
};

#endif // ASSIGNMENT_H