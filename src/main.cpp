#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "../include/CompositeTask.h"
#include "../include/GradingSystemAdapter.h"
#include "../include/Memento.h"
#include "../include/MockExternalSystems.h"
#include "../include/Notification.h"
#include "../include/Observer.h"
#include "../include/Subject.h"
#include "../include/Task.h"
#include "../include/TaskBuilder.h"
#include "../include/TaskDecorator.h"
#include "../include/TaskProcess.h"
#include "../include/TaskState.h"

class ObservableTask;

std::vector<std::shared_ptr<Subject>> subjects;

std::shared_ptr<GradingSystemFacade> gradingSystemFacade;

MementoCaretaker caretaker;

std::shared_ptr<DeadlineObserver> deadlineObserver;

void initializeApplication() {
  auto universitySystem = std::make_shared<MockUniversityGradingSystem>();
  auto onlinePlatformSystem =
      std::make_shared<MockOnlinePlatformGradingSystem>();

  auto universityAdapter =
      std::make_shared<UniversityGradingSystemAdapter>(universitySystem);
  auto onlinePlatformAdapter =
      std::make_shared<OnlinePlatformGradingSystemAdapter>(
          onlinePlatformSystem);

  gradingSystemFacade = std::make_shared<GradingSystemFacade>("S12345");

  gradingSystemFacade->registerGradingSystem("University", universityAdapter);
  gradingSystemFacade->registerGradingSystem("OnlinePlatform",
                                             onlinePlatformAdapter);

  gradingSystemFacade->setActiveGradingSystem("University");
}

void clearInputBuffer() {
  std::cin.clear();
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int getIntInput(const std::string &prompt,
                int min = std::numeric_limits<int>::min(),
                int max = std::numeric_limits<int>::max()) {
  int value;
  bool validInput = false;

  do {
    std::cout << prompt;
    if (std::cin >> value) {
      if (value >= min && value <= max) {
        validInput = true;
      } else {
        std::cout << "Value must be between " << min << " and " << max << "."
                  << std::endl;
      }
    } else {
      std::cout << "Invalid input. Please enter a number." << std::endl;
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
  } while (!validInput);

  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  return value;
}

std::string getStringInput(const std::string &prompt) {
  std::string input;
  std::cout << prompt;
  std::getline(std::cin, input);

  if (!input.empty()) {
    size_t first = input.find_first_not_of(" \t\n\r");
    size_t last = input.find_last_not_of(" \t\n\r");
    if (first != std::string::npos && last != std::string::npos) {
      input = input.substr(first, last - first + 1);
    } else {
      input = "";
    }
  }

  return input;
}

DateTime getDateTimeInput(const std::string &prompt) {
  std::string input;
  struct std::tm timeinfo = {};
  bool validInput = false;

  do {
    std::cout << prompt << " (format: YYYY-MM-DD HH:MM): ";
    std::getline(std::cin, input);

    std::istringstream ss(input);
    ss >> std::get_time(&timeinfo, "%Y-%m-%d %H:%M");

    if (ss.fail()) {
      std::cout << "Invalid date format. Please try again." << std::endl;
    } else {
      validInput = true;
    }
  } while (!validInput);

  int original_hour = timeinfo.tm_hour;
  int original_min = timeinfo.tm_min;

  std::time_t time = std::mktime(&timeinfo);
  auto time_point = std::chrono::system_clock::from_time_t(time);

  auto time_t_check = std::chrono::system_clock::to_time_t(time_point);
  std::tm *timeinfo_check = std::localtime(&time_t_check);

  if (timeinfo_check->tm_hour != original_hour ||
      timeinfo_check->tm_min != original_min) {
    int hour_diff = original_hour - timeinfo_check->tm_hour;
    int min_diff = original_min - timeinfo_check->tm_min;

    time_point +=
        std::chrono::hours(hour_diff) + std::chrono::minutes(min_diff);
  }

  return time_point;
}

void createSubject() {
  std::cout << "\n===== Create New Subject =====\n";

  std::string name, code, description;

  do {
    name = getStringInput("Enter subject name: ");
    if (name.empty()) {
      std::cout << "Subject name cannot be empty. Please try again."
                << std::endl;
    }
  } while (name.empty());

  do {
    code = getStringInput("Enter subject code: ");
    if (code.empty()) {
      std::cout << "Subject code cannot be empty. Please try again."
                << std::endl;
    }
  } while (code.empty());

  description = getStringInput("Enter subject description (optional): ");

  try {
    auto subject = SubjectBuilder()
                       .setName(name)
                       .setCode(code)
                       .setDescription(description)
                       .build();

    subjects.push_back(subject);
    std::cout << "Subject created successfully!\n";
  } catch (const std::exception &e) {
    std::cout << "Error creating subject: " << e.what() << std::endl;
  }
}

void listSubjects() {
  std::cout << "\n===== Subjects =====\n";

  if (subjects.empty()) {
    std::cout << "No subjects available. Create a subject first.\n";
    return;
  }

  for (size_t i = 0; i < subjects.size(); ++i) {
    std::cout << i + 1 << ". " << subjects[i]->getName() << " ("
              << subjects[i]->getCode() << ")\n";
  }
}

std::shared_ptr<Subject> selectSubject() {
  if (subjects.empty()) {
    std::cout << "No subjects available. Create a subject first.\n";
    return nullptr;
  }

  listSubjects();
  int choice = getIntInput(
      "Select a subject (1-" + std::to_string(subjects.size()) + "): ", 1,
      subjects.size());
  return subjects[choice - 1];
}

void createTask() {
  std::cout << "\n===== Create New Task =====\n";

  auto subject = selectSubject();
  if (!subject)
    return;

  std::string title;
  do {
    title = getStringInput("Enter task title: ");
    if (title.empty()) {
      std::cout << "Task title cannot be empty. Please try again." << std::endl;
    }
  } while (title.empty());

  std::string description =
      getStringInput("Enter task description (optional): ");
  DateTime deadline = getDateTimeInput("Enter task deadline");

  std::cout << "Select task type:\n";
  std::cout << "1. Lab\n";
  std::cout << "2. Project\n";
  std::cout << "3. Exam\n";
  int typeChoice = getIntInput("Enter your choice (1-3): ", 1, 3);

  try {
    TaskBuilder builder;
    builder.setTitle(title).setDescription(description).setDeadline(deadline);

    switch (typeChoice) {
    case 1:
      builder.asLab();
      break;
    case 2:
      builder.asProject();
      break;
    case 3:
      builder.asExam();
      break;
    }

    auto task = builder.build();
    subject->addTask(task);

    std::cout << "Do you want to add a notification for this task?\n";
    std::cout << "1. Yes\n";
    std::cout << "2. No\n";
    int notifChoice = getIntInput("Enter your choice (1-2): ", 1, 2);

    if (notifChoice == 1) {
      int days = getIntInput(
          "How many days before the deadline do you want to be notified? ", 1,
          30);
      std::string message = "Reminder for " + title;

      auto notification =
          std::make_shared<DeadlineNotification>(message, task, days);

      NotificationManager::getInstance().addNotification(notification);
      std::cout << "Notification set for " << days
                << " days before the deadline.\n";
    }

    std::cout << "Task created successfully!\n";
  } catch (const std::exception &e) {
    std::cout << "Error creating task: " << e.what() << std::endl;
  }
}

void listTasks() {
  std::cout << "\n===== Tasks =====\n";

  auto subject = selectSubject();
  if (!subject)
    return;

  auto tasks = subject->getTasks();

  if (tasks.empty()) {
    std::cout << "No tasks for this subject.\n";
    return;
  }

  std::cout << "Tasks for " << subject->getName() << ":\n";
  for (size_t i = 0; i < tasks.size(); ++i) {
    auto &task = tasks[i];

    auto time = std::chrono::system_clock::to_time_t(task->getDeadline());
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M");

    std::cout << i + 1 << ". " << task->getTitle() << " (" << task->getType()
              << ")" << " - Due: " << ss.str() << "\n";
  }
}

void viewTaskDetails() {
  std::cout << "\n===== Task Details =====\n";

  auto subject = selectSubject();
  if (!subject)
    return;

  auto tasks = subject->getTasks();

  if (tasks.empty()) {
    std::cout << "No tasks for this subject.\n";
    return;
  }

  std::cout << "Tasks for " << subject->getName() << ":\n";
  for (size_t i = 0; i < tasks.size(); ++i) {
    std::cout << i + 1 << ". " << tasks[i]->getTitle() << "\n";
  }

  int choice = getIntInput(
      "Select an task (1-" + std::to_string(tasks.size()) + "): ", 1,
      tasks.size());
  auto task = tasks[choice - 1];

  std::cout << "\n";
  task->displayInfo();

  auto &notificationManager = NotificationManager::getInstance();
  auto notifications = notificationManager.getNotificationsForTask(task);

  if (!notifications.empty()) {
    std::cout << "\nNotifications for this task:\n";
    for (size_t i = 0; i < notifications.size(); ++i) {
      auto &notification = notifications[i];

      if (auto deadlineNotif =
              std::dynamic_pointer_cast<DeadlineNotification>(notification)) {
        std::cout << i + 1 << ". " << deadlineNotif->getDaysBeforeDeadline()
                  << " days before deadline: " << deadlineNotif->getMessage()
                  << "\n";
      } else {
        auto time = std::chrono::system_clock::to_time_t(
            notification->getTriggerTime());
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M");

        std::cout << i + 1 << ". At " << ss.str() << ": "
                  << notification->getMessage() << "\n";
      }
    }
  } else {
    std::cout << "\nNo notifications set for this task.\n";
  }
}

void checkNotifications() {
  std::cout << "\n===== Checking Notifications =====\n";

  auto &notificationManager = NotificationManager::getInstance();
  notificationManager.checkNotifications();
}

void viewAllNotifications() {
  std::cout << "\n===== All Notifications =====\n";

  auto &notificationManager = NotificationManager::getInstance();
  auto notifications = notificationManager.getNotifications();

  if (notifications.empty()) {
    std::cout << "You have no notifications set up.\n";
    return;
  }

  std::cout << "You have " << notifications.size()
            << " notification(s) scheduled:\n";

  for (size_t i = 0; i < notifications.size(); ++i) {
    auto &notification = notifications[i];

    if (auto deadlineNotif =
            std::dynamic_pointer_cast<DeadlineNotification>(notification)) {
      if (auto task = deadlineNotif->getTask()) {
        auto time = std::chrono::system_clock::to_time_t(task->getDeadline());
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M");

        std::cout << i + 1 << ". " << task->getTitle() << " - Due: " << ss.str()
                  << " - Reminder: " << deadlineNotif->getDaysBeforeDeadline()
                  << " days before deadline\n";
      }
    } else {
      auto time =
          std::chrono::system_clock::to_time_t(notification->getTriggerTime());
      std::stringstream ss;
      ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M");

      std::cout << i + 1 << ". Notification at " << ss.str() << ": "
                << notification->getMessage() << "\n";
    }
  }
}

void saveApplicationState() {
  std::cout << "\n===== Save Application State =====\n";

  std::string description =
      getStringInput("Enter a description for this save point: ");
  auto memento = std::make_shared<AcademicProgressMemento>(description);

  for (const auto &subject : subjects) {
    memento->addSubjectMemento(subject->createMemento());

    for (const auto &task : subject->getTasks()) {
      memento->addTaskMemento(task->createMemento());
    }
  }

  caretaker.addMemento(memento);

  std::string filename =
      "academic_progress_" + memento->getTimestamp() + ".save";
  bool success = caretaker.saveToFile(filename);

  if (success) {
    std::cout << "Application state saved successfully to file: " << filename
              << std::endl;
  } else {
    std::cout << "Failed to save application state to file." << std::endl;
  }
}

void restoreApplicationState() {
  std::cout << "\n===== Restore Application State =====\n";

  if (caretaker.getMementoCount() == 0) {
    std::cout << "No saved states available. Checking for save files..."
              << std::endl;

    std::string filename =
        getStringInput("Enter the name of the save file to load: ");
    bool success = caretaker.loadFromFile(filename);

    if (!success) {
      std::cout << "Failed to load save file: " << filename << std::endl;
      return;
    }

    std::cout << "Save file loaded successfully." << std::endl;
  }

  auto mementos = caretaker.getAllMementos();
  std::cout << "Available saved states:" << std::endl;

  for (size_t i = 0; i < mementos.size(); ++i) {
    std::cout << i + 1 << ". " << mementos[i]->getTimestamp() << " - "
              << mementos[i]->getDescription() << std::endl;
  }

  int choice = getIntInput("Select a saved state to restore (1-" +
                               std::to_string(mementos.size()) + "): ",
                           1, mementos.size());
  auto selectedMemento = mementos[choice - 1];

  subjects.clear();

  std::map<std::string, std::shared_ptr<Subject>> subjectMap;
  for (const auto &subjectMemento : selectedMemento->getSubjects()) {
    auto subject = std::make_shared<Subject>("", "", "");
    subject->restoreFromMemento(subjectMemento);
    subjects.push_back(subject);
    subjectMap[subject->getCode()] = subject;
  }

  for (const auto &taskMemento : selectedMemento->getTasks()) {
    std::shared_ptr<Task> task;
    std::string taskType = taskMemento->getState();
    if (taskType == "Lab") {
      task =
          std::make_shared<LabTask>("", std::chrono::system_clock::now(), "");
    } else if (taskType == "Project") {
      task = std::make_shared<ProjectTask>("", std::chrono::system_clock::now(),
                                           "");
    } else if (taskType == "Exam") {
      task =
          std::make_shared<ExamTask>("", std::chrono::system_clock::now(), "");
    } else {
      task =
          std::make_shared<LabTask>("", std::chrono::system_clock::now(), "");
    }

    task->restoreFromMemento(taskMemento);

    std::string subjectCode = taskMemento->getSubjectCode();
    if (subjectMap.find(subjectCode) != subjectMap.end()) {
      subjectMap[subjectCode]->addTask(task);
    }
  }

  std::cout << "Application state restored successfully." << std::endl;
  std::cout << "Restored " << subjects.size() << " subjects." << std::endl;
}

void updateTaskState() {
  std::cout << "\n===== Update Task State =====\n";

  auto subject = selectSubject();
  if (!subject)
    return;

  auto tasks = subject->getTasks();

  if (tasks.empty()) {
    std::cout << "No tasks for this subject.\n";
    return;
  }

  std::cout << "Tasks for " << subject->getName() << ":\n";
  for (size_t i = 0; i < tasks.size(); ++i) {
    auto &task = tasks[i];
    std::cout << i + 1 << ". " << task->getTitle()
              << " - Current state: " << task->getStateName() << "\n";
  }

  int choice = getIntInput(
      "Select an task (1-" + std::to_string(tasks.size()) + "): ", 1,
      tasks.size());
  auto task = tasks[choice - 1];

  std::cout << "Current state: " << task->getStateName() << std::endl;
  std::cout << "State description: " << task->getStateDescription()
            << std::endl;

  std::cout << "Select action:\n";
  std::cout << "1. Start task\n";
  std::cout << "2. Update progress\n";
  std::cout << "3. Complete task\n";
  std::cout << "4. Check deadline\n";

  int action = getIntInput("Enter your choice (1-4): ", 1, 4);

  switch (action) {
  case 1:
    task->startTask();
    std::cout << "Task started. New state: " << task->getStateName()
              << std::endl;
    break;
  case 2: {
    float progress = getIntInput("Enter progress percentage (0-100): ", 0, 100);
    task->updateProgress(progress);
    std::cout << "Progress updated. New state: " << task->getStateName()
              << std::endl;
    break;
  }
  case 3: {
    int marks = getIntInput("Enter marks achieved (0-100): ", 0, 100);
    task->getState()->complete(marks);
    std::cout << "Task completed. New state: " << task->getStateName()
              << std::endl;
    break;
  }
  case 4:
    task->checkDeadline();
    std::cout << "Deadline checked. Current state: " << task->getStateName()
              << std::endl;
    break;
  default:
    std::cout << "Invalid action." << std::endl;
  }
}

void processTaskWithTemplate() {
  std::cout << "\n===== Process Task with Template Method =====\n";

  auto subject = selectSubject();
  if (!subject)
    return;

  auto tasks = subject->getTasks();

  if (tasks.empty()) {
    std::cout << "No tasks for this subject.\n";
    return;
  }

  std::cout << "Tasks for " << subject->getName() << ":\n";
  for (size_t i = 0; i < tasks.size(); ++i) {
    auto &task = tasks[i];
    std::cout << i + 1 << ". " << task->getTitle() << " (" << task->getType()
              << ")\n";
  }

  int choice = getIntInput(
      "Select an task to process (1-" + std::to_string(tasks.size()) + "): ", 1,
      tasks.size());
  auto task = tasks[choice - 1];

  std::string submission = getStringInput("Enter submission content: ");

  std::shared_ptr<TaskProcess> processor;

  if (auto hw = std::dynamic_pointer_cast<LabTask>(task)) {
    processor = std::make_shared<LabProcess>();
  } else if (auto proj = std::dynamic_pointer_cast<ProjectTask>(task)) {
    processor = std::make_shared<ProjectProcess>();
  } else if (auto exam = std::dynamic_pointer_cast<ExamTask>(task)) {
    processor = std::make_shared<ExamProcess>();
  } else {
    processor = std::make_shared<LabProcess>();
  }

  processor->processTask(task, submission);
}

void subscribeToTask() {
  std::cout << "\n===== Subscribe to Task Updates =====\n";

  auto subject = selectSubject();
  if (!subject)
    return;

  auto tasks = subject->getTasks();

  if (tasks.empty()) {
    std::cout << "No tasks for this subject.\n";
    return;
  }

  std::cout << "Tasks for " << subject->getName() << ":\n";
  for (size_t i = 0; i < tasks.size(); ++i) {
    auto &task = tasks[i];
    std::cout << i + 1 << ". " << task->getTitle() << " (" << task->getType()
              << ")\n";
  }

  int choice = getIntInput("Select an task to subscribe to (1-" +
                               std::to_string(tasks.size()) + "): ",
                           1, tasks.size());
  auto task = tasks[choice - 1];

  std::shared_ptr<ObservableTask> observableTask =
      std::make_shared<ObservableTask>();

  std::cout << "Creating an observable wrapper for this task..." << std::endl;

  std::cout << "Select observer type:\n";
  std::cout << "1. Deadline Observer (deadline warnings)\n";
  std::cout << "2. Grade Observer (grade updates)\n";
  std::cout << "3. Progress Observer (progress updates)\n";

  int observerType = getIntInput("Enter observer type (1-4): ", 1, 4);

  std::shared_ptr<Observer> observer;

  switch (observerType) {
  case 1:
    if (!deadlineObserver) {
      int days = getIntInput("Days before deadline to notify: ", 1, 14);
      deadlineObserver = std::make_shared<DeadlineObserver>(days);
    }
    observer = deadlineObserver;
    break;
  case 2:
    observer = std::make_shared<GradeObserver>();
    break;
  case 3:
    observer = std::make_shared<ProgressObserver>();
    break;
  default:
    observer = deadlineObserver;
  }

  observableTask->attach(observer);

  std::cout << "Observer attached successfully. You will now receive "
               "notifications for this task."
            << std::endl;

  std::cout << "\nSending test notification..." << std::endl;
  observableTask->stateChanged(task ? task->getStateName() : "Unknown");
}

void markTaskAsCompleted();
void decorateTask();
void createCompositeTask();
void submitTaskToGradingSystem();
void switchGradingSystem();
void saveApplicationState();
void restoreApplicationState();
void updateTaskState();
void processTaskWithTemplate();
void subscribeToTask();

void showMainMenu() {
  std::cout << "\n===== Academic Progress Tracker =====\n";
  std::cout << "1. Create a new subject\n";
  std::cout << "2. Create a new task\n";
  std::cout << "3. View subjects\n";
  std::cout << "4. View tasks\n";
  std::cout << "5. View task details\n";
  std::cout << "6. Check notifications\n";
  std::cout << "7. View all notifications\n";
  std::cout << "8. Mark task as completed\n";
  std::cout << "0. Exit\n";
  std::cout << "=================================\n";
}

int main() {
  initializeApplication();

  bool running = true;
  while (running) {
    showMainMenu();
    int choice = getIntInput("Enter your choice: ", 0, 17);

    switch (choice) {
    case 0:
      running = false;
      break;
    case 1:
      createSubject();
      break;
    case 2:
      createTask();
      break;
    case 3:
      listSubjects();
      break;
    case 4:
      listTasks();
      break;
    case 5:
      viewTaskDetails();
      break;
    case 6:
      checkNotifications();
      break;
    case 7:
      viewAllNotifications();
      break;
    case 8:
      markTaskAsCompleted();
      break;
    case 9:
      decorateTask();
      break;
    case 10:
      createCompositeTask();
      break;
    case 11:
      submitTaskToGradingSystem();
      break;
    case 12:
      switchGradingSystem();
      break;
    case 13:
      saveApplicationState();
      break;
    case 14:
      restoreApplicationState();
      break;
    case 15:
      updateTaskState();
      break;
    case 16:
      processTaskWithTemplate();
      break;
    case 17:
      subscribeToTask();
      break;
    default:
      std::cout << "Invalid choice. Please try again.\n";
    }
  }

  return 0;
}

void markTaskAsCompleted() {
  std::cout << "\n===== Mark Task as Completed =====\n";

  auto subject = selectSubject();
  if (!subject)
    return;

  auto tasks = subject->getTasks();

  if (tasks.empty()) {
    std::cout << "No tasks for this subject.\n";
    return;
  }

  std::cout << "Tasks for " << subject->getName() << ":\n";
  for (size_t i = 0; i < tasks.size(); ++i) {
    auto &task = tasks[i];

    auto time = std::chrono::system_clock::to_time_t(task->getDeadline());
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M");

    std::cout << i + 1 << ". " << task->getTitle() << " (" << task->getType()
              << ")" << " - Due: " << ss.str()
              << (task->isCompleted() ? " - Completed" : " - Not completed")
              << "\n";
  }

  int choice = getIntInput(
      "Select an task (1-" + std::to_string(tasks.size()) + "): ", 1,
      tasks.size());
  auto task = tasks[choice - 1];

  bool isCompleted =
      getIntInput("Mark as completed? (1: Yes, 0: No): ", 0, 1) == 1;

  if (isCompleted) {
    int marks = getIntInput("Enter marks received (0-100): ", 0, 100);
    task->setCompleted(true);
    task->setMarks(marks);
    std::cout << "Task marked as completed with " << marks << " marks.\n";

    std::string grade = gradingSystemFacade->getGradeRepresentation(marks);
    std::cout << "Grade in " << gradingSystemFacade->getActiveSystemName()
              << ": " << grade << "\n";
  } else {
    task->setCompleted(false);
    task->setMarks(0);
    std::cout << "Task marked as not completed.\n";
  }
}

void decorateTask() {
  std::cout << "\n===== Decorate Task =====\n";

  auto subject = selectSubject();
  if (!subject)
    return;

  auto tasks = subject->getTasks();

  if (tasks.empty()) {
    std::cout << "No tasks for this subject.\n";
    return;
  }

  std::cout << "Tasks for " << subject->getName() << ":\n";
  for (size_t i = 0; i < tasks.size(); ++i) {
    auto &task = tasks[i];
    std::cout << i + 1 << ". " << task->getTitle() << " (" << task->getType()
              << ")\n";
  }

  int choice = getIntInput("Select an task to decorate (1-" +
                               std::to_string(tasks.size()) + "): ",
                           1, tasks.size());
  auto baseTask = tasks[choice - 1];

  std::cout << "Select decoration type:\n";
  std::cout << "1. Add priority level\n";
  std::cout << "2. Add tags\n";
  std::cout << "3. Add color coding\n";
  int decorationType = getIntInput("Enter decoration type (1-3): ", 1, 3);

  std::shared_ptr<Task> decoratedTask;

  switch (decorationType) {
  case 1: {
    int priorityLevel =
        getIntInput("Enter priority level (1-5, with 5 being highest): ", 1, 5);
    decoratedTask =
        std::make_shared<PriorityTaskDecorator>(baseTask, priorityLevel);
    std::cout << "Priority level " << priorityLevel << " added to task.\n";
    break;
  }
  case 2: {
    std::vector<std::string> tags;
    std::string tag;
    int tagCount = getIntInput("How many tags do you want to add? ", 1, 10);

    for (int i = 0; i < tagCount; ++i) {
      tag = getStringInput("Enter tag " + std::to_string(i + 1) + ": ");
      if (!tag.empty()) {
        tags.push_back(tag);
      }
    }

    decoratedTask = std::make_shared<TaggedTaskDecorator>(baseTask, tags);
    std::cout << tags.size() << " tags added to task.\n";
    break;
  }
  case 3: {
    std::cout << "Available colors:\n";
    std::cout << "1. Red (Urgent)\n";
    std::cout << "2. Yellow (Important)\n";
    std::cout << "3. Green (Normal)\n";
    std::cout << "4. Blue (Low priority)\n";
    std::cout << "5. Purple (Special)\n";

    int colorChoice = getIntInput("Select a color (1-5): ", 1, 5);
    ColoredTaskDecorator::Color color;

    switch (colorChoice) {
    case 1:
      color = ColoredTaskDecorator::Color::RED;
      break;
    case 2:
      color = ColoredTaskDecorator::Color::YELLOW;
      break;
    case 3:
      color = ColoredTaskDecorator::Color::GREEN;
      break;
    case 4:
      color = ColoredTaskDecorator::Color::BLUE;
      break;
    case 5:
      color = ColoredTaskDecorator::Color::PURPLE;
      break;
    default:
      color = ColoredTaskDecorator::Color::DEFAULT;
    }

    decoratedTask = std::make_shared<ColoredTaskDecorator>(baseTask, color);
    std::cout << "Color added to task.\n";
    break;
  }
  }

  if (decoratedTask) {
    subject->removeTask(baseTask->getTitle());
    subject->addTask(decoratedTask);

    std::cout << "Task successfully decorated!\n";
    decoratedTask->displayInfo();
  }
}

void createCompositeTask() {
  std::cout << "\n===== Create Composite Task =====\n";

  auto subject = selectSubject();
  if (!subject)
    return;

  std::string title = getStringInput("Enter composite task title: ");
  if (title.empty()) {
    std::cout << "Task title cannot be empty.\n";
    return;
  }

  std::string description =
      getStringInput("Enter composite task description (optional): ");
  DateTime deadline = getDateTimeInput("Enter composite task deadline");

  auto compositeTask =
      std::make_shared<CompositeTask>(title, deadline, description);

  bool addingChildren = true;
  while (addingChildren) {
    std::cout << "\nAdd child task to " << title << ":\n";
    std::cout << "1. Create new child task\n";
    std::cout << "2. Done adding child tasks\n";

    int choice = getIntInput("Enter your choice (1-2): ", 1, 2);

    if (choice == 2) {
      addingChildren = false;
      continue;
    }

    std::string childTitle = getStringInput("Enter child task title: ");
    if (childTitle.empty()) {
      std::cout << "Task title cannot be empty. Skipping this child.\n";
      continue;
    }

    std::string childDescription =
        getStringInput("Enter child task description (optional): ");

    std::cout << "Select child task type:\n";
    std::cout << "1. Lab\n";
    std::cout << "2. Project\n";
    std::cout << "3. Exam\n";
    int typeChoice = getIntInput("Enter your choice (1-3): ", 1, 3);

    std::shared_ptr<Task> childTask;

    switch (typeChoice) {
    case 1:
      childTask =
          std::make_shared<LabTask>(childTitle, deadline, childDescription);
      break;
    case 2:
      childTask =
          std::make_shared<ProjectTask>(childTitle, deadline, childDescription);
      break;
    case 3:
      childTask =
          std::make_shared<ExamTask>(childTitle, deadline, childDescription);
      break;
    }

    compositeTask->addChildTask(childTask);
    std::cout << "Child task added: " << childTitle << "\n";
  }

  subject->addTask(compositeTask);

  std::cout << "Composite task created with "
            << compositeTask->getChildTasks().size() << " child tasks.\n";
}

void submitTaskToGradingSystem() {
  std::cout << "\n===== Submit Task to Grading System =====\n";
  std::cout << "Using grading system: "
            << gradingSystemFacade->getActiveSystemName() << "\n";

  auto subject = selectSubject();
  if (!subject)
    return;

  auto tasks = subject->getTasks();

  if (tasks.empty()) {
    std::cout << "No tasks for this subject.\n";
    return;
  }

  std::cout << "Tasks for " << subject->getName() << ":\n";
  for (size_t i = 0; i < tasks.size(); ++i) {
    auto &task = tasks[i];

    auto time = std::chrono::system_clock::to_time_t(task->getDeadline());
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M");

    std::cout << i + 1 << ". " << task->getTitle() << " (" << task->getType()
              << ")" << " - Due: " << ss.str() << "\n";
  }

  int choice = getIntInput(
      "Select an task to submit (1-" + std::to_string(tasks.size()) + "): ", 1,
      tasks.size());
  auto task = tasks[choice - 1];

  std::string submissionContent =
      getStringInput("Enter submission content (e.g., file path or text): ");

  try {
    gradingSystemFacade->submitTask(task, submissionContent);

    if (gradingSystemFacade->isCompleted(task)) {
      int score = gradingSystemFacade->getScore(task);
      task->setCompleted(true);
      task->setMarks(score);

      std::string grade = gradingSystemFacade->getGradeRepresentation(score);

      std::cout << "Task submitted and graded automatically.\n";
      std::cout << "Score: " << score << " out of 100\n";
      std::cout << "Grade: " << grade << "\n";
    } else {
      std::cout << "Task submitted but not yet graded.\n";
    }
  } catch (const std::exception &e) {
    std::cout << "Error submitting task: " << e.what() << "\n";
  }
}

void switchGradingSystem() {
  std::cout << "\n===== Switch Grading System =====\n";

  auto availableSystems = gradingSystemFacade->getAvailableGradingSystems();

  if (availableSystems.empty()) {
    std::cout << "No grading systems available.\n";
    return;
  }

  std::cout << "Available grading systems:\n";
  for (size_t i = 0; i < availableSystems.size(); ++i) {
    std::cout << i + 1 << ". " << availableSystems[i] << "\n";
  }

  int choice = getIntInput("Select a grading system (1-" +
                               std::to_string(availableSystems.size()) + "): ",
                           1, availableSystems.size());

  try {
    gradingSystemFacade->setActiveGradingSystem(availableSystems[choice - 1]);
    std::cout << "Switched to grading system: "
              << gradingSystemFacade->getActiveSystemName() << "\n";
  } catch (const std::exception &e) {
    std::cout << "Error switching grading system: " << e.what() << "\n";
  }
}
