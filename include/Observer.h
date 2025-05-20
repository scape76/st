#ifndef OBSERVER_H
#define OBSERVER_H

#include <string>
#include <vector>
#include <memory>
#include <algorithm>

// Forward declarations
class Assignment;
class Course;

// Observer interface
class Observer {
public:
    virtual ~Observer() = default;
    virtual void update(const std::string& message, void* subject) = 0;
};

// Subject interface
class Subject {
private:
    std::vector<std::shared_ptr<Observer>> observers;

public:
    virtual ~Subject() = default;

    void attach(std::shared_ptr<Observer> observer) {
        observers.push_back(observer);
    }

    void detach(std::shared_ptr<Observer> observer) {
        observers.erase(
            std::remove(observers.begin(), observers.end(), observer),
            observers.end()
        );
    }

    void notify(const std::string& message) {
        for (const auto& observer : observers) {
            observer->update(message, this);
        }
    }
};

// Concrete observers

// Student Observer - monitors assignments and courses for a specific student
class StudentObserver : public Observer {
private:
    std::string studentId;
    std::string studentName;

public:
    StudentObserver(const std::string& id, const std::string& name)
        : studentId(id), studentName(name) {}

    void update(const std::string& message, void* subject) override;
    
    std::string getStudentId() const { return studentId; }
    std::string getStudentName() const { return studentName; }
};

// Deadline Observer - specifically monitors assignment deadlines
class DeadlineObserver : public Observer {
private:
    int daysWarning; // How many days before deadline to warn

public:
    DeadlineObserver(int daysBeforeDeadline = 1)
        : daysWarning(daysBeforeDeadline) {}

    void update(const std::string& message, void* subject) override;
    
    int getDaysWarning() const { return daysWarning; }
    void setDaysWarning(int days) { daysWarning = days; }
};

// Grade Observer - monitors when grades are published
class GradeObserver : public Observer {
public:
    GradeObserver() = default;
    void update(const std::string& message, void* subject) override;
};

// Assignment Progress Observer - monitors progress on assignments
class ProgressObserver : public Observer {
private:
    float lastRecordedProgress;

public:
    ProgressObserver() : lastRecordedProgress(0.0f) {}
    void update(const std::string& message, void* subject) override;
    
    float getLastRecordedProgress() const { return lastRecordedProgress; }
};

// Make Assignment and Course classes observable
class ObservableAssignment : public Subject {
public:
    virtual ~ObservableAssignment() = default;
    
    // Events that will trigger notifications
    void deadlineApproaching();
    void stateChanged(const std::string& newState);
    void progressUpdated(float newProgress);
    void gradeAssigned(int marks);
};

class ObservableCourse : public Subject {
public:
    virtual ~ObservableCourse() = default;
    
    // Events that will trigger notifications
    void assignmentAdded(const std::string& assignmentTitle);
    void courseCompleted();
    void finalGradeAssigned(const std::string& grade);
};

#endif // OBSERVER_H