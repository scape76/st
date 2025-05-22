#ifndef OBSERVER_H
#define OBSERVER_H

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

class Observer {
public:
  virtual ~Observer() = default;
  virtual void update(const std::string &message, void *subject) = 0;
};

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

class DeadlineObserver : public Observer {
private:
  int daysWarning;

public:
  DeadlineObserver(int daysBeforeDeadline = 1)
      : daysWarning(daysBeforeDeadline) {}

  void update(const std::string &message, void *subject) override;

  int getDaysWarning() const { return daysWarning; }
  void setDaysWarning(int days) { daysWarning = days; }
};

class GradeObserver : public Observer {
public:
  GradeObserver() = default;
  void update(const std::string &message, void *subject) override;
};

class ProgressObserver : public Observer {
private:
  float lastRecordedProgress;

public:
  ProgressObserver() : lastRecordedProgress(0.0f) {}
  void update(const std::string &message, void *subject) override;

  float getLastRecordedProgress() const { return lastRecordedProgress; }
};

class ObservableTask : public SubjectI {
public:
  virtual ~ObservableTask() = default;

  void deadlineApproaching();
  void stateChanged(const std::string &newState);
  void progressUpdated(float newProgress);
  void gradeAssigned(int marks);
};

class ObservableSubject : public SubjectI {
public:
  virtual ~ObservableSubject() = default;

  void taskAdded(const std::string &taskTitle);
  void subjectCompleted();
  void finalGradeAssigned(const std::string &grade);
};

#endif