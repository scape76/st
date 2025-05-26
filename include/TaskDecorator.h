#ifndef ASSIGNMENT_DECORATOR_H
#define ASSIGNMENT_DECORATOR_H

#include "Task.h"
#include <memory>
#include <string>

class TaskDecorator : public Task {
protected:
  std::shared_ptr<Task> wrappedTask;

public:
  TaskDecorator(std::shared_ptr<Task> assignment);

  std::string getTitle() const override;
  std::string getDescription() const override;
  DateTime getDeadline() const override;
  std::shared_ptr<Subject> getSubject() const override;
  std::vector<std::shared_ptr<Notification>> getNotifications() const override;
  bool isCompleted() const override;
  int getMarks() const override;

  void setTitle(const std::string &title) override;
  void setDescription(const std::string &description) override;
  void setDeadline(const DateTime &deadline) override;
  void setSubject(std::shared_ptr<Subject> subject) override;
  void setCompleted(bool completed) override;
  void setMarks(int marks) override;

  void addNotification(std::shared_ptr<Notification> notification) override;
  void removeNotification(size_t index) override;

  virtual void displayInfo() const override;
  virtual std::string getType() const override;
};

class PriorityTaskDecorator : public TaskDecorator {
private:
  int priorityLevel;

public:
  PriorityTaskDecorator(std::shared_ptr<Task> assignment,
                        int priorityLevel = 1);

  void setPriorityLevel(int level);
  int getPriorityLevel() const;

  void displayInfo() const override;
};

class TaggedTaskDecorator : public TaskDecorator {
private:
  std::vector<std::string> tags;

public:
  TaggedTaskDecorator(std::shared_ptr<Task> assignment,
                      const std::vector<std::string> &initialTags = {});

  void addTag(const std::string &tag);
  void removeTag(const std::string &tag);
  std::vector<std::string> getTags() const;

  void displayInfo() const override;
};

class ColoredTaskDecorator : public TaskDecorator {
public:
  enum class Color {
    RED,
    YELLOW,
    GREEN,
    BLUE,
    PURPLE,
    DEFAULT
  };

private:
  Color color;

public:
  ColoredTaskDecorator(std::shared_ptr<Task> assignment,
                       Color color = Color::DEFAULT);

  void setColor(Color color);
  Color getColor() const;
  std::string getColorName() const;

  void displayInfo() const override;
};

#endif