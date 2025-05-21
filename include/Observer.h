#ifndef OBSERVER_H
#define OBSERVER_H

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

// Forward declarations
// class Task;
// class Subject;

// Observer interface
class Observer {
public:
  virtual ~Observer() = default;
  virtual void update(const std::string &message, void *subject) = 0;
};

// Subject interface
class SubjectI {
private:
  std::vector<std::shared_ptr<Observer>> observers;

public:
  virtual ~SubjectI() = default;

  void attach(std::shared_ptr<Observer> observer) {
    observers.push_back(observer);
  }

  void detach(std::shared_ptr<Observer> observer) {
    observers.erase(std::remove(observers.begin(), observers.end(), observer),
                    observers.end());
  }

  void notify(const std::string &message) {
    for (const auto &observer : observers) {
      observer->update(message, this);
    }
  }
};

// Concrete observers

// Deadline Observer - specifically monitors task deadlines
class DeadlineObserver : public Observer {
private:
  int daysWarning; // How many days before deadline to warn

public:
  DeadlineObserver(int daysBeforeDeadline = 1)
      : daysWarning(daysBeforeDeadline) {}

  void update(const std::string &message, void *subject) override;

  int getDaysWarning() const { return daysWarning; }
  void setDaysWarning(int days) { daysWarning = days; }
};

// Grade Observer - monitors when grades are published
class GradeObserver : public Observer {
public:
  GradeObserver() = default;
  void update(const std::string &message, void *subject) override;
};

// Task Progress Observer - monitors progress on tasks
class ProgressObserver : public Observer {
private:
  float lastRecordedProgress;

public:
  ProgressObserver() : lastRecordedProgress(0.0f) {}
  void update(const std::string &message, void *subject) override;

  float getLastRecordedProgress() const { return lastRecordedProgress; }
};

// Make Task and Subject classes observable
class ObservableTask : public SubjectI {
public:
  virtual ~ObservableTask() = default;

  // Events that will trigger notifications
  void deadlineApproaching();
  void stateChanged(const std::string &newState);
  void progressUpdated(float newProgress);
  void gradeAssigned(int marks);
};

class ObservableSubject : public SubjectI {
public:
  virtual ~ObservableSubject() = default;

  // Events that will trigger notifications
  void taskAdded(const std::string &taskTitle);
  void subjectCompleted();
  void finalGradeAssigned(const std::string &grade);
};

#endif // OBSERVER_H
