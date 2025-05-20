#ifndef COURSE_H
#define COURSE_H

#include "Memento.h"
#include <memory>
#include <string>
#include <vector>

// Forward declaration to avoid circular dependencies
class Assignment;
class CourseMemento;

class Course {
private:
  std::string name;
  std::string code;
  std::string description;
  std::vector<std::shared_ptr<Assignment>> assignments;

public:
  // Constructor
  Course(const std::string &name, const std::string &code,
         const std::string &description = "");

  // Getters
  std::string getName() const;
  std::string getCode() const;
  std::string getDescription() const;
  std::vector<std::shared_ptr<Assignment>> getAssignments() const;

  // Methods
  void addAssignment(std::shared_ptr<Assignment> assignment);
  void removeAssignment(const std::string &title);
  std::shared_ptr<Assignment> findAssignment(const std::string &title) const;

  // Display course information
  void displayInfo() const;

  // Memento pattern methods
  std::shared_ptr<CourseMemento> createMemento() const;
  void restoreFromMemento(const std::shared_ptr<CourseMemento> &memento);
};

// CourseBuilder class (Builder Pattern)
class CourseBuilder {
private:
  std::string name;
  std::string code;
  std::string description;

public:
  CourseBuilder() = default;

  CourseBuilder &setName(const std::string &name);
  CourseBuilder &setCode(const std::string &code);
  CourseBuilder &setDescription(const std::string &description);

  std::shared_ptr<Course> build() const;
};

#endif // COURSE_H
