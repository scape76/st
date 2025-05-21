#include "../include/Observer.h"
#include "../include/Subject.h"
#include "../include/Task.h"
#include <chrono>
#include <iostream>

// DeadlineObserver implementation
void DeadlineObserver::update(const std::string &message, void *subject) {
  if (auto task = static_cast<Task *>(subject)) {
    auto now = std::chrono::system_clock::now();
    auto deadline = task->getDeadline();

    // Calculate days until deadline
    auto daysUntilDeadline =
        std::chrono::duration_cast<std::chrono::hours>(deadline - now).count() /
        24;

    if (daysUntilDeadline <= daysWarning && daysUntilDeadline >= 0) {
      std::cout << "[DEADLINE WARNING] " << message << std::endl;
      std::cout << "  Task: " << task->getTitle() << " is due in "
                << daysUntilDeadline << " days." << std::endl;

      if (auto subject = task->getSubject()) {
        std::cout << "  Subject: " << subject->getName() << " ("
                  << subject->getCode() << ")" << std::endl;
      }
    } else if (daysUntilDeadline < 0) {
      // Task is overdue
      std::cout << "[OVERDUE NOTICE] " << message << std::endl;
      std::cout << "  Task: " << task->getTitle() << " is overdue by "
                << -daysUntilDeadline << " days." << std::endl;

      if (auto subject = task->getSubject()) {
        std::cout << "  Subject: " << subject->getName() << " ("
                  << subject->getCode() << ")" << std::endl;
      }
    }
  }
}

// GradeObserver implementation
void GradeObserver::update(const std::string &message, void *subject) {
  std::cout << "[GRADE NOTIFICATION] " << message << std::endl;

  if (auto task = static_cast<Task *>(subject)) {
    std::cout << "  Task: " << task->getTitle() << std::endl;
    std::cout << "  Grade: " << task->getMarks() << " / 100" << std::endl;

    if (auto subject = task->getSubject()) {
      std::cout << "  Subject: " << subject->getName() << " ("
                << subject->getCode() << ")" << std::endl;
    }
  } else if (Subject *subject = static_cast<Subject *>(subject)) {
    std::cout << "  Subject: " << subject->getName() << " ("
              << subject->getCode() << ")" << std::endl;
    // For final subject grades, we'd need additional information not currently
    // available
  }
}

// ProgressObserver implementation
void ProgressObserver::update(const std::string &message, void *subject) {
  if (auto task = static_cast<Task *>(subject)) {
    float currentProgress = task->getProgress();

    if (currentProgress != lastRecordedProgress) {
      std::cout << "[PROGRESS UPDATE] " << message << std::endl;
      std::cout << "  Task: " << task->getTitle() << std::endl;
      std::cout << "  Progress: " << lastRecordedProgress << "% -> "
                << currentProgress << "%" << std::endl;

      if (auto subject = task->getSubject()) {
        std::cout << "  Subject: " << subject->getName() << " ("
                  << subject->getCode() << ")" << std::endl;
      }

      lastRecordedProgress = currentProgress;
    }
  }
}

// ObservableTask implementation
void ObservableTask::deadlineApproaching() {
  notify("Deadline is approaching!");
}

void ObservableTask::stateChanged(const std::string &newState) {
  notify("Task state changed to: " + newState);
}

void ObservableTask::progressUpdated(float newProgress) {
  notify("Progress updated to " + std::to_string(int(newProgress)) + "%");
}

void ObservableTask::gradeAssigned(int marks) {
  notify("Grade assigned: " + std::to_string(marks) + " marks");
}

// ObservableSubject implementation
void ObservableSubject::taskAdded(const std::string &taskTitle) {
  notify("New task added: " + taskTitle);
}

void ObservableSubject::subjectCompleted() {
  notify("Subject has been completed!");
}

void ObservableSubject::finalGradeAssigned(const std::string &grade) {
  notify("Final grade assigned: " + grade);
}
