#include <chrono>
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "../include/Notification.h"
#include "../include/Registry.h"
#include "../include/Subject.h"
#include "../include/Task.h"
#include "../include/TaskBuilder.h"

using namespace emscripten;

// Global state variables
Registry &registry = Registry::instance();

// Helper functions to convert C++ data types to JavaScript
val subjectToJS(const std::shared_ptr<Subject> &subject) {
  val result = val::object();
  if (subject) {
    result.set("name", subject->getName());
    result.set("code", subject->getCode());
    result.set("description", subject->getDescription());

    // Convert tasks
    val jsTasks = val::array();
    auto tasks = subject->getTasks();
    for (size_t i = 0; i < tasks.size(); i++) {
      val jsTask = val::object();
      jsTask.set("title", tasks[i]->getTitle());
      jsTask.set("description", tasks[i]->getDescription());

      // Convert deadline to ISO string
      auto time = std::chrono::system_clock::to_time_t(tasks[i]->getDeadline());
      std::tm *tm = std::localtime(&time);
      char buffer[30];
      std::strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%S", tm);
      jsTask.set("deadline", std::string(buffer));

      jsTask.set("type", tasks[i]->getType());
      jsTask.set("completed", tasks[i]->isCompleted());
      jsTask.set("marks", tasks[i]->getMarks());
      jsTask.set("progress", tasks[i]->getProgress());
      jsTask.set("stateName", tasks[i]->getStateName());

      jsTasks.set(i, jsTask);
    }
    result.set("tasks", jsTasks);
  }
  return result;
}

val taskToJS(const std::shared_ptr<Task> &task) {
  val result = val::object();
  if (task) {
    result.set("title", task->getTitle());
    result.set("description", task->getDescription());

    // Convert deadline to ISO string
    auto time = std::chrono::system_clock::to_time_t(task->getDeadline());
    std::tm *tm = std::localtime(&time);
    char buffer[30];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%S", tm);
    result.set("deadline", std::string(buffer));

    result.set("type", task->getType());
    result.set("completed", task->isCompleted());
    result.set("marks", task->getMarks());
    result.set("progress", task->getProgress());
    result.set("stateName", task->getStateName());
  }
  return result;
}

// Subject management functions
std::string createSubject(const std::string &name, const std::string &code,
                          const std::string &description) {
  auto subject = SubjectBuilder()
                     .setName(name)
                     .setCode(code)
                     .setDescription(description)
                     .build();

  registry.subjects[subject->getCode()] = subject;
  return subject->getCode();
}

val getAllSubjects() {
  auto result = val::array();

  size_t i = 0;
  for (const auto &[_, value] : registry.subjects) {
    result.set(i, subjectToJS(value));
    ++i;
  }

  return result;
}

val getSubject(const std::string &code) {
  try {
    auto subject = registry.subjects.at(code);

    return subjectToJS(subject);
  } catch (const std::out_of_range &e) {
    return val::null();
  }
}

// Task management functions
int createTask(const std::string &subjectCode, const std::string &title,
               const std::string &description, const std::string &deadlineStr,
               int taskType) {
  try {
    std::tm timeinfo = {};
    std::stringstream ss(deadlineStr);
    ss >> std::get_time(&timeinfo, "%Y-%m-%dT%H:%M:%S");

    if (ss.fail()) {
      return -1;
    }

    // Convert to time_point
    std::time_t time = std::mktime(&timeinfo);
    DateTime deadline = std::chrono::system_clock::from_time_t(time);

    // Build the task
    TaskBuilder builder;
    builder.setTitle(title).setDescription(description).setDeadline(deadline);

    switch (taskType) {
    case 1:
      builder.asLab();
      break;
    case 2:
      builder.asProject();
      break;
    case 3:
      builder.asExam();
      break;
    default:
      builder.asLab();
      break;
    }

    auto task = builder.build();
    registry.subjects[subjectCode]->addTask(task);

    auto tasks = registry.subjects[subjectCode]->getTasks();
    return tasks.size() - 1; // Return the index
  } catch (const std::out_of_range &e) {
    return -1;
  }
}

val getSubjectTasks(const std::string &subjectCode) {
  val result = val::array();

  try {
    auto tasks = registry.subjects[subjectCode]->getTasks();
    for (size_t i = 0; i < tasks.size(); i++) {
      result.set(i, taskToJS(tasks[i]));
    }
  } catch (std::out_of_range &e) {
  }

  return result;
}

// bool markTaskAsCompleted(int subjectIndex, int taskIndex, int marks) {
//   if (subjectIndex < 0 || subjectIndex >= static_cast<int>(subjects.size()))
//   {
//     return false;
//   }

//   auto tasks = subjects[subjectIndex]->getTasks();
//   if (taskIndex < 0 || taskIndex >= static_cast<int>(tasks.size())) {
//     return false;
//   }

//   auto task = tasks[taskIndex];
//   task->setCompleted(true);
//   task->setMarks(marks);
//   return true;
// }

EMSCRIPTEN_BINDINGS(academic_progress_tracker) {
  function("createSubject", &createSubject);
  function("getAllSubjects", &getAllSubjects);
  function("getSubject", &getSubject);

  function("createTask", &createTask);
  function("getSubjectTasks", &getSubjectTasks);
  // function("markTaskAsCompleted", &markTaskAsCompleted);
}
