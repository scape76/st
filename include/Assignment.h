#ifndef ASSIGNMENT_H
#define ASSIGNMENT_H

#include <string>
#include <memory>
#include <chrono>
#include <vector>
#include <iostream>

// Forward declaration
class Course;
class Notification;

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

public:
    // Constructor
    Assignment(const std::string& title, 
               const DateTime& deadline, 
               const std::string& description = "");
    
    // Virtual destructor for polymorphism
    virtual ~Assignment() = default;
    
    // Getters
    std::string getTitle() const;
    std::string getDescription() const;
    DateTime getDeadline() const;
    std::shared_ptr<Course> getCourse() const;
    std::vector<std::shared_ptr<Notification>> getNotifications() const;
    
    // Setters
    void setTitle(const std::string& title);
    void setDescription(const std::string& description);
    void setDeadline(const DateTime& deadline);
    void setCourse(std::shared_ptr<Course> course);
    
    // Notification management
    void addNotification(std::shared_ptr<Notification> notification);
    void removeNotification(size_t index);
    
    // Display assignment information
    virtual void displayInfo() const;
    
    // Assignment types
    virtual std::string getType() const = 0;
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