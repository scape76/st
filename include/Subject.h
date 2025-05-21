#ifndef COURSE_H
#define COURSE_H

#include "Memento.h"
#include <memory>
#include <string>
#include <vector>

// Forward declaration to avoid circular dependencies
class Task;
class SubjectMemento;

class Subject {
private:
  std::string name;
  std::string code;
  std::string description;
  std::vector<std::shared_ptr<Task>> tasks;

public:
  // Constructor
  Subject(const std::string &name, const std::string &code,
          const std::string &description = "");

  // Getters
  std::string getName() const;
  std::string getCode() const;
  std::string getDescription() const;
  std::vector<std::shared_ptr<Task>> getTasks() const;

  // Methods
  void addTask(std::shared_ptr<Task> assignment);
  void removeTask(const std::string &title);
  std::shared_ptr<Task> findTask(const std::string &title) const;

  // Display subject information
  void displayInfo() const;

  // Memento pattern methods
  std::shared_ptr<SubjectMemento> createMemento() const;
  void restoreFromMemento(const std::shared_ptr<SubjectMemento> &memento);
};

// SubjectBuilder class (Builder Pattern)
class SubjectBuilder {
private:
  std::string name;
  std::string code;
  std::string description;

public:
  SubjectBuilder() = default;

  SubjectBuilder &setName(const std::string &name);
  SubjectBuilder &setCode(const std::string &code);
  SubjectBuilder &setDescription(const std::string &description);

  std::shared_ptr<Subject> build() const;
};

#endif // COURSE_H
