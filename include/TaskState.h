#ifndef TASK_STATE_H
#define TASK_STATE_H

#include <memory>
#include <string>

class Task;

class TaskState {
public:
  virtual ~TaskState() = default;

  virtual void start(Task &task);
  virtual void complete(Task &task);
  virtual void reopen(Task &task);

  virtual std::string getName() const = 0;
  virtual float getConceptualProgress() const = 0;
  virtual bool isFinished() const;
};

class PendingState : public TaskState {
public:
  void start(Task &task) override;
  void complete(Task &task) override;

  std::string getName() const override { return "Pending"; }
  float getConceptualProgress() const override { return 0.0f; }
};

class InProgressState : public TaskState {
public:
  void complete(Task &task) override;
  void reopen(Task &task) override;

  std::string getName() const override { return "In Progress"; }
  float getConceptualProgress() const override { return 50.0f; }
};

class CompletedState : public TaskState {
public:
  void reopen(Task &task) override;

  std::string getName() const override { return "Completed"; }
  float getConceptualProgress() const override { return 100.0f; }
  bool isFinished() const override;
};

#endif
