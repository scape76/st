#ifndef ASSIGNMENT_STATE_H
#define ASSIGNMENT_STATE_H

#include <chrono>
#include <memory>
#include <string>

class Task;
class NotStartedState;
class InProgressState;
class CompletedState;
class OverdueState;

class TaskState {
protected:
  Task *context;

public:
  virtual ~TaskState() = default;

  void setContext(Task *assignment) { this->context = assignment; }

  virtual void start() = 0;
  virtual void makeProgress(float percentage) = 0;
  virtual void complete(int marks) = 0;
  virtual void checkDeadline() = 0;
  virtual void handleNotification() = 0;

  virtual std::string getName() const = 0;

  virtual std::string getDescription() const = 0;

  virtual std::string getColor() const = 0;
};

class NotStartedState : public TaskState {
public:
  void start() override;
  void makeProgress(float percentage) override;
  void complete(int marks) override;
  void checkDeadline() override;
  void handleNotification() override;

  std::string getName() const override { return "Not Started"; }

  std::string getDescription() const override {
    return "Task has not been started yet.";
  }

  std::string getColor() const override { return "Gray"; }

  static std::shared_ptr<TaskState> getInstance();

private:
  static std::shared_ptr<NotStartedState> instance;
};

class InProgressState : public TaskState {
private:
  float progressPercentage;

public:
  InProgressState(float initialProgress = 0.0f)
      : progressPercentage(initialProgress) {}

  void start() override;
  void makeProgress(float percentage) override;
  void complete(int marks) override;
  void checkDeadline() override;
  void handleNotification() override;

  float getProgressPercentage() const { return progressPercentage; }

  std::string getName() const override { return "In Progress"; }

  std::string getDescription() const override {
    return "Task is currently being worked on.";
  }

  std::string getColor() const override { return "Blue"; }
};

class CompletedState : public TaskState {
private:
  int achievedMarks;

public:
  CompletedState(int marks = 0) : achievedMarks(marks) {}

  void start() override;
  void makeProgress(float percentage) override;
  void complete(int marks) override;
  void checkDeadline() override;
  void handleNotification() override;

  int getAchievedMarks() const { return achievedMarks; }

  std::string getName() const override { return "Completed"; }

  std::string getDescription() const override {
    return "Task has been completed.";
  }

  std::string getColor() const override { return "Green"; }
};

class OverdueState : public TaskState {
private:
  std::chrono::system_clock::time_point overdueDate;

public:
  OverdueState(std::chrono::system_clock::time_point overdueDate =
                   std::chrono::system_clock::now())
      : overdueDate(overdueDate) {}

  void start() override;
  void makeProgress(float percentage) override;
  void complete(int marks) override;
  void checkDeadline() override;
  void handleNotification() override;

  std::string getName() const override { return "Overdue"; }

  std::string getDescription() const override {
    return "Task is past its deadline.";
  }

  std::string getColor() const override { return "Red"; }
};

#endif