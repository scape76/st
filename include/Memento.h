#ifndef MEMENTO_H
#define MEMENTO_H

#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

class Task;
class Subject;

class TaskMemento {
private:
  friend class Task;
  friend class MementoCaretaker;

  std::string title;
  std::string description;
  std::chrono::system_clock::time_point deadline;
  bool completed;
  int marks;
  float progress;
  std::string state;
  std::string subjectCode;

public:
  TaskMemento(const std::string &title, const std::string &description,
              const std::chrono::system_clock::time_point &deadline,
              bool completed, int marks, float progress,
              const std::string &state, const std::string &subjectCode)
      : title(title), description(description), deadline(deadline),
        completed(completed), marks(marks), progress(progress), state(state),
        subjectCode(subjectCode) {}

  std::string getTitle() const { return title; }
  std::string getDescription() const { return description; }
  std::chrono::system_clock::time_point getDeadline() const { return deadline; }
  bool isCompleted() const { return completed; }
  int getMarks() const { return marks; }
  float getProgress() const { return progress; }
  std::string getState() const { return state; }
  std::string getSubjectCode() const { return subjectCode; }
};

class SubjectMemento {
private:
  friend class Subject;
  friend class MementoCaretaker;

  std::string name;
  std::string code;
  std::string description;
  std::vector<std::string> taskTitles;

public:
  SubjectMemento(const std::string &name, const std::string &code,
                 const std::string &description,
                 const std::vector<std::string> &taskTitles)
      : name(name), code(code), description(description),
        taskTitles(taskTitles) {}

  std::string getName() const { return name; }
  std::string getCode() const { return code; }
  std::string getDescription() const { return description; }
  const std::vector<std::string> &getTaskTitles() const { return taskTitles; }
};

class AcademicProgressMemento {
private:
  std::vector<std::shared_ptr<SubjectMemento>> subjects;
  std::vector<std::shared_ptr<TaskMemento>> tasks;
  std::string timestamp;
  std::string description;

public:
  AcademicProgressMemento(const std::string &description = "")
      : description(description) {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d_%H-%M-%S");
    timestamp = ss.str();
  }

  void addSubjectMemento(std::shared_ptr<SubjectMemento> memento) {
    subjects.push_back(memento);
  }

  void addTaskMemento(std::shared_ptr<TaskMemento> memento) {
    tasks.push_back(memento);
  }

  const std::vector<std::shared_ptr<SubjectMemento>> &getSubjects() const {
    return subjects;
  }

  const std::vector<std::shared_ptr<TaskMemento>> &getTasks() const {
    return tasks;
  }

  std::string getTimestamp() const { return timestamp; }

  std::string getDescription() const { return description; }
};

class MementoCaretaker {
private:
  std::vector<std::shared_ptr<AcademicProgressMemento>> savedStates;

public:
  void addMemento(std::shared_ptr<AcademicProgressMemento> memento) {
    savedStates.push_back(memento);
  }

  std::shared_ptr<AcademicProgressMemento> getMemento(size_t index) {
    if (index < savedStates.size()) {
      return savedStates[index];
    }
    return nullptr;
  }

  std::shared_ptr<AcademicProgressMemento> getLatestMemento() {
    if (!savedStates.empty()) {
      return savedStates.back();
    }
    return nullptr;
  }

  std::vector<std::shared_ptr<AcademicProgressMemento>> getAllMementos() const {
    return savedStates;
  }

  size_t getMementoCount() const { return savedStates.size(); }

  bool saveToFile(const std::string &filename) {
    try {
      std::ofstream file(filename);
      if (!file.is_open()) {
        return false;
      }

      file << "AcademicProgressTracker_SaveFile_v1" << std::endl;
      file << savedStates.size() << std::endl;

      for (const auto &memento : savedStates) {
        file << memento->getTimestamp() << std::endl;
        file << memento->getDescription() << std::endl;

        const auto &subjects = memento->getSubjects();
        file << subjects.size() << std::endl;
        for (const auto &subject : subjects) {
          file << subject->getName() << std::endl;
          file << subject->getCode() << std::endl;
          file << subject->getDescription() << std::endl;

          file << subject->getTaskTitles().size() << std::endl;
          for (const auto &title : subject->getTaskTitles()) {
            file << title << std::endl;
          }
        }

        const auto &tasks = memento->getTasks();
        file << tasks.size() << std::endl;
        for (const auto &task : tasks) {
          file << task->getTitle() << std::endl;
          file << task->getDescription() << std::endl;

          auto time = std::chrono::system_clock::to_time_t(task->getDeadline());
          file << time << std::endl;

          file << task->isCompleted() << std::endl;
          file << task->getMarks() << std::endl;
          file << task->getProgress() << std::endl;
          file << task->getState() << std::endl;
          file << task->getSubjectCode() << std::endl;
        }
      }

      file.close();
      return true;
    } catch (const std::exception &e) {
      std::cerr << "Error saving memento: " << e.what() << std::endl;
      return false;
    }
  }

  bool loadFromFile(const std::string &filename) {
    try {
      std::ifstream file(filename);
      if (!file.is_open()) {
        return false;
      }

      std::string version;
      std::getline(file, version);
      if (version != "AcademicProgressTracker_SaveFile_v1") {
        return false;
      }

      size_t mementoCount;
      file >> mementoCount;
      file.ignore();

      savedStates.clear();

      for (size_t i = 0; i < mementoCount; ++i) {
        std::string timestamp, description;
        std::getline(file, timestamp);
        std::getline(file, description);

        auto memento = std::make_shared<AcademicProgressMemento>(description);

        size_t subjectCount;
        file >> subjectCount;
        file.ignore();

        for (size_t j = 0; j < subjectCount; ++j) {
          std::string name, code, subjectDescription;
          std::getline(file, name);
          std::getline(file, code);
          std::getline(file, subjectDescription);

          std::vector<std::string> taskTitles;
          size_t titleCount;
          file >> titleCount;
          file.ignore();

          for (size_t k = 0; k < titleCount; ++k) {
            std::string title;
            std::getline(file, title);
            taskTitles.push_back(title);
          }

          auto subjectMemento = std::make_shared<SubjectMemento>(
              name, code, subjectDescription, taskTitles);
          memento->addSubjectMemento(subjectMemento);
        }

        size_t taskCount;
        file >> taskCount;
        file.ignore();

        for (size_t j = 0; j < taskCount; ++j) {
          std::string title, description, state, subjectCode;
          std::getline(file, title);
          std::getline(file, description);

          std::time_t timestamp;
          file >> timestamp;
          file.ignore();
          auto deadline = std::chrono::system_clock::from_time_t(timestamp);

          bool completed;
          int marks;
          float progress;

          file >> completed;
          file >> marks;
          file >> progress;
          file.ignore();

          std::getline(file, state);
          std::getline(file, subjectCode);

          auto taskMemento = std::make_shared<TaskMemento>(
              title, description, deadline, completed, marks, progress, state,
              subjectCode);
          memento->addTaskMemento(taskMemento);
        }

        savedStates.push_back(memento);
      }

      file.close();
      return true;
    } catch (const std::exception &e) {
      std::cerr << "Error loading memento: " << e.what() << std::endl;
      return false;
    }
  }

  void clearMementos() { savedStates.clear(); }
};

#endif
