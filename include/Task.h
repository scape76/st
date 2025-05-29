#ifndef TASK_H
#define TASK_H

#include <chrono>
#include <memory>
#include <string>
#include <vector>

class Subject;
class Notification;
class TaskState;

using DateTime = std::chrono::system_clock::time_point;

class Task {
private:
  std::string title_;
  std::string description_;
  DateTime deadline_;
  std::shared_ptr<Subject> subject_;
  std::vector<std::shared_ptr<Notification>> notifications_;
  int marks_;
  std::shared_ptr<TaskState> state_;

public:
  Task(const std::string &title, const DateTime &deadline,
       const std::string &description = "");

  virtual ~Task() = default;

  void setState(std::shared_ptr<TaskState> newState);
  std::shared_ptr<TaskState> getState() const;

  std::string getTitle() const;
  std::string getDescription() const;
  DateTime getDeadline() const;
  std::shared_ptr<Subject> getSubject() const;
  std::vector<std::shared_ptr<Notification>> getNotifications() const;
  int getMarks() const;

  bool isCompleted() const;
  float getProgress() const;
  std::string getStateName() const;

  void setTitle(const std::string &title);
  void setDescription(const std::string &description);
  void setDeadline(const DateTime &deadline);
  void setSubject(std::shared_ptr<Subject> subject);
  void setMarks(int marks);

  void startTask();
  void completeTask();
  void reopenTask();

  void addNotification(std::shared_ptr<Notification> notification);
  void removeNotification(size_t index);

  virtual void displayInfo() const;
  virtual std::string getType() const = 0;

  friend class PendingState;
  friend class InProgressState;
  friend class CompletedState;
};

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

class TaskFactory {
public:
  virtual std::shared_ptr<Task>
  createTask(const std::string &title, const DateTime &deadline,
             const std::string &description = "") = 0;

  virtual ~TaskFactory() = default;
};

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

#endif
