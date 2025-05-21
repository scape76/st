#ifndef ASSIGNMENT_H
#define ASSIGNMENT_H

#include "Memento.h"
#include "TaskState.h"
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

// Forward declaration
class Subject;
class Notification;
class TaskState;
class TaskMemento;

// Using a type alias for date/time
using DateTime = std::chrono::system_clock::time_point;

// Task class
class Task {
private:
  std::string title;
  std::string description;
  DateTime deadline;
  std::shared_ptr<Subject> subject;
  std::vector<std::shared_ptr<Notification>> notifications;
  bool completed;
  int marks;
  float progress;
  std::shared_ptr<TaskState> state;
  friend class TaskState;
  friend class NotStartedState;
  friend class InProgressState;
  friend class CompletedState;
  friend class OverdueState;

public:
  // Constructor
  Task(const std::string &title, const DateTime &deadline,
       const std::string &description = "");

  // Virtual destructor for polymorphism
  virtual ~Task() = default;

  // State pattern methods
  void setState(std::shared_ptr<TaskState> state);
  std::shared_ptr<TaskState> getState() const;
  void startTask();
  void updateProgress(float percentage);
  void checkDeadline();
  std::string getStateName() const;
  std::string getStateDescription() const;

  // Getters
  virtual std::string getTitle() const;
  virtual std::string getDescription() const;
  virtual DateTime getDeadline() const;
  virtual std::shared_ptr<Subject> getSubject() const;
  virtual std::vector<std::shared_ptr<Notification>> getNotifications() const;
  virtual bool isCompleted() const;
  virtual int getMarks() const;
  virtual float getProgress() const;

  // Setters
  virtual void setTitle(const std::string &title);
  virtual void setDescription(const std::string &description);
  virtual void setDeadline(const DateTime &deadline);
  virtual void setSubject(std::shared_ptr<Subject> subject);
  virtual void setCompleted(bool completed);
  virtual void setMarks(int marks);
  virtual void setProgress(float progress);

  // Notification management
  virtual void addNotification(std::shared_ptr<Notification> notification);
  virtual void removeNotification(size_t index);

  // Display task information
  virtual void displayInfo() const;

  // Task types
  virtual std::string getType() const = 0;

  // Memento pattern methods
  std::shared_ptr<TaskMemento> createMemento() const;
  void restoreFromMemento(const std::shared_ptr<TaskMemento> &memento);
};

// Concrete Task types
class LabTask : public Task {
public:
  LabTask(const std::string &title, const DateTime &deadline,
          const std::string &description = "");

  std::string getType() const override { return "Lab"; }
};

class ProjectTask : public Task {
public:
  ProjectTask(const std::string &title, const DateTime &deadline,
              const std::string &description = "");

  std::string getType() const override { return "Project"; }
};

class ExamTask : public Task {
public:
  ExamTask(const std::string &title, const DateTime &deadline,
           const std::string &description = "");

  std::string getType() const override { return "Exam"; }
};

// Task Factory (Factory Method Pattern)
class TaskFactory {
public:
  virtual std::shared_ptr<Task>
  createTask(const std::string &title, const DateTime &deadline,
             const std::string &description = "") = 0;

  virtual ~TaskFactory() = default;
};

// Concrete factories
class LabFactory : public TaskFactory {
public:
  std::shared_ptr<Task>
  createTask(const std::string &title, const DateTime &deadline,
             const std::string &description = "") override;
};

class ProjectFactory : public TaskFactory {
public:
  std::shared_ptr<Task>
  createTask(const std::string &title, const DateTime &deadline,
             const std::string &description = "") override;
};

class ExamFactory : public TaskFactory {
public:
  std::shared_ptr<Task>
  createTask(const std::string &title, const DateTime &deadline,
             const std::string &description = "") override;
};

#endif // ASSIGNMENT_H
