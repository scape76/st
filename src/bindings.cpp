#include <chrono>
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <iomanip>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "../include/Command.h"
#include "../include/CommandManager.h"
#include "../include/HtmlProvider.h"
#include "../include/Internship.h"
#include "../include/MarkdownParser.h"
#include "../include/Notification.h"
#include "../include/Registry.h"
#include "../include/SetTaskStateCommand.h"
#include "../include/ShortcodeExpanderDecorator.h"
#include "../include/Subject.h"
#include "../include/Task.h"
#include "../include/TaskBuilder.h"
#include "../include/TaskState.h"

using namespace emscripten;

Registry &registry = Registry::instance();

void seed() {
  auto &registry = Registry::instance();

  registry.internships["internship_1"] = std::make_shared<Internship>(
      "internship_1", "Google", "Software Engineer Intern",
      InternshipStatus::STARTED, "2024-06-01", "2024-08-31");

  SubjectBuilder subjectBuilder;

  auto math = subjectBuilder.setName("Mathematics")
                  .setCode("MATH101")
                  .setDescription("Fundamental concepts of mathematics.")
                  .build();
  auto cs = subjectBuilder.setName("Computer Science")
                .setCode("CS101")
                .setDescription("Introduction to programming and algorithms.")
                .build();
  auto physics = subjectBuilder.setName("Physics")
                     .setCode("PHYS101")
                     .setDescription("Classical mechanics and thermodynamics.")
                     .build();

  LabFactory labFactory;
  ProjectFactory projectFactory;
  ExamFactory examFactory;

  auto now = std::chrono::system_clock::now();
  DateTime deadline_past = now - std::chrono::hours(72);
  DateTime deadline_soon = now + std::chrono::hours(24 * 7);
  DateTime deadline_future = now + std::chrono::hours(24 * 30);

  auto math_lab1 = labFactory.createTask("Lab 1: Algebra", deadline_past,
                                         "Basic algebraic manipulations");
  math_lab1->completeTask();
  math_lab1->setMarks(90);
  math_lab1->setSubject(math);
  math->addTask(math_lab1);

  auto math_exam = examFactory.createTask("Midterm Exam", deadline_soon,
                                          "Covers first half of the course");
  math_exam->startTask();
  math_exam->setSubject(math);
  math->addTask(math_exam);

  auto cs_project = projectFactory.createTask(
      "Project: Web App", deadline_future, "Develop a simple web application");
  cs_project->startTask();
  cs_project->setSubject(cs);
  cs->addTask(cs_project);

  auto cs_lab1 = labFactory.createTask("Lab 1: Python Basics", deadline_past,
                                       "Introduction to Python syntax");
  cs_lab1->completeTask();
  cs_lab1->setMarks(95);
  cs_lab1->setSubject(cs);
  cs->addTask(cs_lab1);

  auto cs_lab2 = labFactory.createTask("Lab 2: Data Structures", deadline_soon,
                                       "Implement lists and dictionaries");
  cs_lab2->completeTask();
  cs_lab2->setMarks(88);
  cs_lab2->setSubject(cs);
  cs->addTask(cs_lab2);

  auto physics_lab = labFactory.createTask("Lab: Kinematics", deadline_soon,
                                           "Experiments on motion");
  physics_lab->startTask();
  physics_lab->setSubject(physics);
  physics->addTask(physics_lab);

  auto physics_exam = examFactory.createTask(
      "Final Exam", deadline_future, "Comprehensive exam on all topics");
  physics_exam->startTask();
  physics_exam->setSubject(physics);
  physics->addTask(physics_exam);

  registry.subjects[math->getCode()] = math;
  registry.subjects[cs->getCode()] = cs;
  registry.subjects[physics->getCode()] = physics;

  std::cout << "Registry populated with initial data." << std::endl;
  registry.subjects[physics->getCode()] = physics;
}

InternshipStatus internshipStatusFromInt(int statusInt) {
  switch (statusInt) {
  case 0:
    return InternshipStatus::PENDING;
  case 1:
    return InternshipStatus::STARTED;
  case 2:
    return InternshipStatus::ENDED;
  case 3:
    return InternshipStatus::CANCELLED;
  default:
    return InternshipStatus::PENDING;
  }
}

std::shared_ptr<TaskState> getTargetStateFromInt(int targetStateInt) {
  switch (targetStateInt) {
  case 0:
    return std::make_shared<PendingState>();
  case 1:
    return std::make_shared<InProgressState>();
  case 2:
    return std::make_shared<CompletedState>();
  default:
    return nullptr;
  }
}

val internshipToJS(const std::shared_ptr<Internship> &internship) {
  val jsInternship = val::object();
  if (internship) {
    jsInternship.set("id", internship->id);
    jsInternship.set("company", internship->company);
    jsInternship.set("position", internship->position);
    jsInternship.set("status", internshipStatusToString(internship->status));
    jsInternship.set("startDate", internship->startDate);
    jsInternship.set("endDate", internship->endDate);
  }
  return jsInternship;
}

val taskToJS(const std::shared_ptr<Task> &task) {
  val result = val::object();
  if (task) {
    result.set("title", task->getTitle());
    result.set("description", task->getDescription());

    auto time_point = task->getDeadline();
    auto c_time_t =
        std::chrono::system_clock::to_time_t(time_point); // Convert to time_t
    std::tm tm_local = {};
#ifdef _WIN32
    localtime_s(&tm_local, &c_time_t);
#else
    localtime_r(&c_time_t, &tm_local);
#endif

    char buffer[32];
    if (std::strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%S", &tm_local)) {
      result.set("deadline", std::string(buffer));
    } else {
      result.set("deadline", "Error formatting date");
    }

    result.set("type", task->getType());
    result.set("completed", task->isCompleted());
    result.set("marks", task->getMarks());
    result.set("progress", task->getProgress());
    result.set("stateName", task->getStateName());
  }
  return result;
}

val subjectToJS(const std::shared_ptr<Subject> &subject) {
  val result = val::object();
  if (subject) {
    result.set("name", subject->getName());
    result.set("code", subject->getCode());
    result.set("description", subject->getDescription());

    val jsTasks = val::array();
    auto tasks = subject->getTasks();

    for (size_t i = 0; i < tasks.size(); ++i) {
      jsTasks.set(i, taskToJS(tasks[i]));
    }
    result.set("tasks", jsTasks);
  }
  return result;
}

val resumeToJS(const std::shared_ptr<Resume> &resume) {
  val jsResume = val::object();
  if (resume) {
    jsResume.set("id", resume->getId());
    jsResume.set("title", resume->getTitle());
    jsResume.set("body", resume->getHtmlBody());
  }
  return jsResume;
}

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

    std::time_t time = std::mktime(&timeinfo);
    DateTime deadline = std::chrono::system_clock::from_time_t(time);

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
    return tasks.size() - 1;
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

std::string convertMarkdownToHtml(const std::string &markdownInput) {
  try {
    std::unique_ptr<HtmlProvider> baseMarkdownProvider =
        std::make_unique<MarkdownAdapter>();
    std::unique_ptr<HtmlProvider> finalMarkdownProvider =
        std::make_unique<ShortcodeExpanderDecorator>(
            std::move(baseMarkdownProvider));

    return finalMarkdownProvider->getHtml(markdownInput);
  } catch (const std::exception &e) {
    return std::string("Error converting Markdown: ") + e.what();
  }
}

std::string convertAsciiDocToHtml(const std::string &asciiDocInput) {
  try {
    std::unique_ptr<HtmlProvider> baseAsciiDocProvider =
        std::make_unique<AsciiDocAdapter>();
    std::unique_ptr<HtmlProvider> finalAsciiDocProvider =
        std::make_unique<ShortcodeExpanderDecorator>(
            std::move(baseAsciiDocProvider));

    return finalAsciiDocProvider->getHtml(asciiDocInput);
  } catch (const std::exception &e) {
    return std::string("Error converting AsciiDoc: ") + e.what();
  }
}

static long long nextResumeIdCounter = 1;

std::string createNewResume(const std::string &title,
                            const std::string &htmlBody) {
  if (title.empty()) {
    return "";
  }

  std::string generatedId = "resume_" + std::to_string(nextResumeIdCounter++);

  while (registry.resumes.count(generatedId)) {
    generatedId = "resume_" + std::to_string(nextResumeIdCounter++);
  }

  auto resume = std::make_shared<Resume>(generatedId, title, htmlBody);
  registry.resumes[resume->getId()] = resume;
  return resume->getId();
}

val getSpecificResume(const std::string &resumeId) {
  try {
    const auto &resume = registry.resumes.at(resumeId);
    return resumeToJS(resume);
  } catch (const std::out_of_range &e) {
    return val::null();
  }
}

val getAllStoredResumes() {
  val jsResumesArray = val::array();
  size_t index = 0;
  for (const auto &pair : registry.resumes) { // Iterate through the resumes map
    jsResumesArray.set(index++, resumeToJS(pair.second));
  }
  return jsResumesArray;
}

static long long nextInternshipIdCounter = 1;

std::string createNewInternship(const std::string &company,
                                const std::string &position, int statusInt,
                                const std::string &startDate,
                                const std::string &endDate) {
  if (company.empty() || position.empty() || startDate.empty() ||
      endDate.empty()) {
    return "";
  }

  std::string generatedId =
      "internship_" + std::to_string(nextInternshipIdCounter++);

  while (registry.internships.count(generatedId)) {
    generatedId = "internship_" + std::to_string(nextInternshipIdCounter++);
  }

  InternshipStatus status = internshipStatusFromInt(statusInt);

  auto internship = std::make_shared<Internship>(generatedId, company, position,
                                                 status, startDate, endDate);
  registry.internships[generatedId] = internship;
  return generatedId;
}

val getAllStoredInternships() {
  val jsInternshipsArray = val::array();
  size_t index = 0;
  for (const auto &pair : registry.internships) {
    jsInternshipsArray.set(index++, internshipToJS(pair.second));
  }
  return jsInternshipsArray;
}

bool changeTaskState(const std::string &subjectCode, int taskIndex,
                     int targetStateInt) {
  try {
    auto subjectIt = registry.subjects.find(subjectCode);
    if (subjectIt == registry.subjects.end()) {
      std::cout << "Error: Subject with code '" << subjectCode << "' not found."
                << std::endl;
      return false;
    }
    std::shared_ptr<Subject> subject = subjectIt->second;

    auto tasksVec = subject->getTasks();

    if (taskIndex < 0 || static_cast<size_t>(taskIndex) >= tasksVec.size()) {
      std::cout << "Error: Invalid task index " << taskIndex << " for subject '"
                << subjectCode << "'. Max index: " << (tasksVec.size() - 1)
                << std::endl;
      return false;
    }

    Task &taskToChange = *tasksVec[static_cast<size_t>(taskIndex)];

    std::shared_ptr<TaskState> targetState =
        getTargetStateFromInt(targetStateInt);
    if (!targetState) {
      return false;
    }

    auto command =
        std::make_shared<SetTaskStateCommand>(taskToChange, targetState);
    CommandManager::instance().executeCommand(command);
    return true;

  } catch (const std::out_of_range &oor) {
    std::cout << "Error: Subject or Task not found (out_of_range): "
              << oor.what() << std::endl;
    return false;
  } catch (const std::exception &e) {
    std::cout << "Error changing task state: " << e.what() << std::endl;
    return false;
  } catch (...) {
    std::cout << "An unknown error occurred while changing task state."
              << std::endl;
    return false;
  }
}

bool undoLastTaskCommand() {
  if (CommandManager::instance().canUndo()) {
    CommandManager::instance().undoLastCommand();
    return true;
  }
  emscripten::val::global("console").call<void>(
      "log", std::string("No command to undo."));
  return false;
}

EMSCRIPTEN_BINDINGS(academic_progress_tracker) {
  function("seed", &seed);

  function("createSubject", &createSubject);
  function("getAllSubjects", &getAllSubjects);
  function("getSubject", &getSubject);
  function("getSubjectTasks", &getSubjectTasks);

  function("createTask", &createTask);

  function("convertMarkdownToHtml", &convertMarkdownToHtml);
  function("convertAsciiDocToHtml", &convertAsciiDocToHtml);

  function("createNewResume", &createNewResume);
  function("getAllResumes", &getAllStoredResumes);
  function("getResume", &getSpecificResume);

  function("createInternship", &createNewInternship);
  function("getAllInternships", &getAllStoredInternships);

  function("changeTaskState", &changeTaskState);
  function("undoLastTaskCommand", &undoLastTaskCommand);
}
