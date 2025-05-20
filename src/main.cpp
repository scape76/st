#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "../include/Assignment.h"
#include "../include/AssignmentBuilder.h"
#include "../include/AssignmentDecorator.h"
#include "../include/AssignmentProcess.h"
#include "../include/AssignmentState.h"
#include "../include/CompositeAssignment.h"
#include "../include/Course.h"
#include "../include/GradingSystemAdapter.h"
#include "../include/Memento.h"
#include "../include/MockExternalSystems.h"
#include "../include/Notification.h"
#include "../include/Observer.h"

// Forward declaration for ObservableAssignment
class ObservableAssignment;

// Global containers to store courses and assignments
std::vector<std::shared_ptr<Course>> courses;

// Global grading system facade with mock implementations
std::shared_ptr<GradingSystemFacade> gradingSystemFacade;

// Global MementoCaretaker for save/restore functionality
MementoCaretaker caretaker;

// Global observers
std::shared_ptr<StudentObserver> studentObserver;
std::shared_ptr<DeadlineObserver> deadlineObserver;

// Initialize application components
void initializeApplication() {
  // Create mock external systems
  auto universitySystem = std::make_shared<MockUniversityGradingSystem>();
  auto onlinePlatformSystem =
      std::make_shared<MockOnlinePlatformGradingSystem>();

  // Create adapters
  auto universityAdapter =
      std::make_shared<UniversityGradingSystemAdapter>(universitySystem);
  auto onlinePlatformAdapter =
      std::make_shared<OnlinePlatformGradingSystemAdapter>(
          onlinePlatformSystem);

  // Create the facade with a default student ID
  gradingSystemFacade = std::make_shared<GradingSystemFacade>("S12345");

  // Register the grading systems with the facade
  gradingSystemFacade->registerGradingSystem("University", universityAdapter);
  gradingSystemFacade->registerGradingSystem("OnlinePlatform",
                                             onlinePlatformAdapter);

  // Set the university system as active by default
  gradingSystemFacade->setActiveGradingSystem("University");

  // Initialize observers
  studentObserver =
      std::make_shared<StudentObserver>("S12345", "Default Student");
  deadlineObserver = std::make_shared<DeadlineObserver>(3); // 3 days warning
}

// Helper functions
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

  // Clear the newline character left in the buffer after reading an int
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  return value;
}

std::string getStringInput(const std::string &prompt) {
  std::string input;
  std::cout << prompt;
  std::getline(std::cin, input);

  // Trim leading and trailing whitespace
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

  // Save the original hours and minutes that the user entered
  int original_hour = timeinfo.tm_hour;
  int original_min = timeinfo.tm_min;

  // Convert std::tm to system_clock::time_point
  std::time_t time = std::mktime(&timeinfo);
  auto time_point = std::chrono::system_clock::from_time_t(time);

  // Convert back to check if hours were changed due to timezone
  auto time_t_check = std::chrono::system_clock::to_time_t(time_point);
  std::tm *timeinfo_check = std::localtime(&time_t_check);

  // If hours changed, compensate to preserve user input
  if (timeinfo_check->tm_hour != original_hour ||
      timeinfo_check->tm_min != original_min) {
    int hour_diff = original_hour - timeinfo_check->tm_hour;
    int min_diff = original_min - timeinfo_check->tm_min;

    // Add the difference to preserve original time
    time_point +=
        std::chrono::hours(hour_diff) + std::chrono::minutes(min_diff);
  }

  return time_point;
}

// Function to create a new course
void createCourse() {
  std::cout << "\n===== Create New Course =====\n";

  std::string name, code, description;

  do {
    name = getStringInput("Enter course name: ");
    if (name.empty()) {
      std::cout << "Course name cannot be empty. Please try again."
                << std::endl;
    }
  } while (name.empty());

  do {
    code = getStringInput("Enter course code: ");
    if (code.empty()) {
      std::cout << "Course code cannot be empty. Please try again."
                << std::endl;
    }
  } while (code.empty());

  description = getStringInput("Enter course description (optional): ");

  // Using the Builder pattern to create the course
  try {
    auto course = CourseBuilder()
                      .setName(name)
                      .setCode(code)
                      .setDescription(description)
                      .build();

    courses.push_back(course);
    std::cout << "Course created successfully!\n";
  } catch (const std::exception &e) {
    std::cout << "Error creating course: " << e.what() << std::endl;
  }
}

// Function to list all courses
void listCourses() {
  std::cout << "\n===== Courses =====\n";

  if (courses.empty()) {
    std::cout << "No courses available. Create a course first.\n";
    return;
  }

  for (size_t i = 0; i < courses.size(); ++i) {
    std::cout << i + 1 << ". " << courses[i]->getName() << " ("
              << courses[i]->getCode() << ")\n";
  }
}

// Function to select a course from the list
std::shared_ptr<Course> selectCourse() {
  if (courses.empty()) {
    std::cout << "No courses available. Create a course first.\n";
    return nullptr;
  }

  listCourses();
  int choice = getIntInput(
      "Select a course (1-" + std::to_string(courses.size()) + "): ", 1,
      courses.size());
  return courses[choice - 1];
}

// Function to create a new assignment
void createAssignment() {
  std::cout << "\n===== Create New Assignment =====\n";

  auto course = selectCourse();
  if (!course)
    return;

  std::string title;
  do {
    title = getStringInput("Enter assignment title: ");
    if (title.empty()) {
      std::cout << "Assignment title cannot be empty. Please try again."
                << std::endl;
    }
  } while (title.empty());

  std::string description =
      getStringInput("Enter assignment description (optional): ");
  DateTime deadline = getDateTimeInput("Enter assignment deadline");

  // Show assignment type options
  std::cout << "Select assignment type:\n";
  std::cout << "1. Homework\n";
  std::cout << "2. Project\n";
  std::cout << "3. Exam\n";
  int typeChoice = getIntInput("Enter your choice (1-3): ", 1, 3);

  // Using the Builder pattern and Factory method to create the assignment
  try {
    AssignmentBuilder builder;
    builder.setTitle(title).setDescription(description).setDeadline(deadline);

    switch (typeChoice) {
    case 1:
      builder.asHomework();
      break;
    case 2:
      builder.asProject();
      break;
    case 3:
      builder.asExam();
      break;
    }

    auto assignment = builder.build();
    course->addAssignment(assignment);

    // Ask if the user wants to add a notification
    std::cout << "Do you want to add a notification for this assignment?\n";
    std::cout << "1. Yes\n";
    std::cout << "2. No\n";
    int notifChoice = getIntInput("Enter your choice (1-2): ", 1, 2);

    if (notifChoice == 1) {
      int days = getIntInput(
          "How many days before the deadline do you want to be notified? ", 1,
          30);
      std::string message = "Reminder for " + title;

      auto notification =
          std::make_shared<DeadlineNotification>(message, assignment, days);

      // Using the Singleton pattern to access the NotificationManager
      NotificationManager::getInstance().addNotification(notification);
      std::cout << "Notification set for " << days
                << " days before the deadline.\n";
    }

    std::cout << "Assignment created successfully!\n";
  } catch (const std::exception &e) {
    std::cout << "Error creating assignment: " << e.what() << std::endl;
  }
}

// Function to list assignments for a course
void listAssignments() {
  std::cout << "\n===== Assignments =====\n";

  auto course = selectCourse();
  if (!course)
    return;

  auto assignments = course->getAssignments();

  if (assignments.empty()) {
    std::cout << "No assignments for this course.\n";
    return;
  }

  std::cout << "Assignments for " << course->getName() << ":\n";
  for (size_t i = 0; i < assignments.size(); ++i) {
    auto &assignment = assignments[i];

    auto time = std::chrono::system_clock::to_time_t(assignment->getDeadline());
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M");

    std::cout << i + 1 << ". " << assignment->getTitle() << " ("
              << assignment->getType() << ")"
              << " - Due: " << ss.str() << "\n";
  }
}

// Function to view assignment details
void viewAssignmentDetails() {
  std::cout << "\n===== Assignment Details =====\n";

  auto course = selectCourse();
  if (!course)
    return;

  auto assignments = course->getAssignments();

  if (assignments.empty()) {
    std::cout << "No assignments for this course.\n";
    return;
  }

  std::cout << "Assignments for " << course->getName() << ":\n";
  for (size_t i = 0; i < assignments.size(); ++i) {
    std::cout << i + 1 << ". " << assignments[i]->getTitle() << "\n";
  }

  int choice = getIntInput("Select an assignment (1-" +
                               std::to_string(assignments.size()) + "): ",
                           1, assignments.size());
  auto assignment = assignments[choice - 1];

  std::cout << "\n";
  assignment->displayInfo();

  // Show notifications for this assignment
  auto &notificationManager = NotificationManager::getInstance();
  auto notifications =
      notificationManager.getNotificationsForAssignment(assignment);

  if (!notifications.empty()) {
    std::cout << "\nNotifications for this assignment:\n";
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
    std::cout << "\nNo notifications set for this assignment.\n";
  }
}

// Function to check for notifications
void checkNotifications() {
  std::cout << "\n===== Checking Notifications =====\n";

  auto &notificationManager = NotificationManager::getInstance();
  notificationManager.checkNotifications();
}

// Function to view all notifications
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
      if (auto assignment = deadlineNotif->getAssignment()) {
        auto time =
            std::chrono::system_clock::to_time_t(assignment->getDeadline());
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M");

        std::cout << i + 1 << ". " << assignment->getTitle()
                  << " - Due: " << ss.str()
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

// Function to save the current application state
void saveApplicationState() {
  std::cout << "\n===== Save Application State =====\n";

  std::string description =
      getStringInput("Enter a description for this save point: ");
  auto memento = std::make_shared<AcademicProgressMemento>(description);

  // Save courses
  for (const auto &course : courses) {
    memento->addCourseMemento(course->createMemento());

    // Save assignments for this course
    for (const auto &assignment : course->getAssignments()) {
      memento->addAssignmentMemento(assignment->createMemento());
    }
  }

  caretaker.addMemento(memento);

  // Save to file
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

// Function to restore a previously saved application state
void restoreApplicationState() {
  std::cout << "\n===== Restore Application State =====\n";

  // First check if we have any saved states
  if (caretaker.getMementoCount() == 0) {
    std::cout << "No saved states available. Checking for save files..."
              << std::endl;

    // Try to load from file
    std::string filename =
        getStringInput("Enter the name of the save file to load: ");
    bool success = caretaker.loadFromFile(filename);

    if (!success) {
      std::cout << "Failed to load save file: " << filename << std::endl;
      return;
    }

    std::cout << "Save file loaded successfully." << std::endl;
  }

  // Display available saved states
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

  // Clear current state
  courses.clear();

  // First restore courses
  std::map<std::string, std::shared_ptr<Course>> courseMap;
  for (const auto &courseMemento : selectedMemento->getCourses()) {
    auto course = std::make_shared<Course>("", "", "");
    course->restoreFromMemento(courseMemento);
    courses.push_back(course);
    courseMap[course->getCode()] = course;
  }

  // Then restore assignments and link them to courses
  for (const auto &assignmentMemento : selectedMemento->getAssignments()) {
    // Create the appropriate type of assignment
    std::shared_ptr<Assignment> assignment;
    std::string assignmentType = assignmentMemento->getState();
    if (assignmentType == "Homework") {
      assignment = std::make_shared<HomeworkAssignment>(
          "", std::chrono::system_clock::now(), "");
    } else if (assignmentType == "Project") {
      assignment = std::make_shared<ProjectAssignment>(
          "", std::chrono::system_clock::now(), "");
    } else if (assignmentType == "Exam") {
      assignment = std::make_shared<ExamAssignment>(
          "", std::chrono::system_clock::now(), "");
    } else {
      assignment = std::make_shared<HomeworkAssignment>(
          "", std::chrono::system_clock::now(), "");
    }

    // Restore assignment state
    assignment->restoreFromMemento(assignmentMemento);

    // Link to course
    std::string courseCode = assignmentMemento->getCourseCode();
    if (courseMap.find(courseCode) != courseMap.end()) {
        courseMap[courseCode]->addAssignment(assignment);
    }
  }

  std::cout << "Application state restored successfully." << std::endl;
  std::cout << "Restored " << courses.size() << " courses." << std::endl;
}

// Function to update an assignment's state
void updateAssignmentState() {
  std::cout << "\n===== Update Assignment State =====\n";

  auto course = selectCourse();
  if (!course)
    return;

  auto assignments = course->getAssignments();

  if (assignments.empty()) {
    std::cout << "No assignments for this course.\n";
    return;
  }

  std::cout << "Assignments for " << course->getName() << ":\n";
  for (size_t i = 0; i < assignments.size(); ++i) {
    auto &assignment = assignments[i];
    std::cout << i + 1 << ". " << assignment->getTitle()
              << " - Current state: " << assignment->getStateName() << "\n";
  }

  int choice = getIntInput("Select an assignment (1-" +
                               std::to_string(assignments.size()) + "): ",
                           1, assignments.size());
  auto assignment = assignments[choice - 1];

  std::cout << "Current state: " << assignment->getStateName() << std::endl;
  std::cout << "State description: " << assignment->getStateDescription()
            << std::endl;

  std::cout << "Select action:\n";
  std::cout << "1. Start assignment\n";
  std::cout << "2. Update progress\n";
  std::cout << "3. Complete assignment\n";
  std::cout << "4. Check deadline\n";

  int action = getIntInput("Enter your choice (1-4): ", 1, 4);

  switch (action) {
  case 1:
    assignment->startAssignment();
    std::cout << "Assignment started. New state: " << assignment->getStateName()
              << std::endl;
    break;
  case 2: {
    float progress = getIntInput("Enter progress percentage (0-100): ", 0, 100);
    assignment->updateProgress(progress);
    std::cout << "Progress updated. New state: " << assignment->getStateName()
              << std::endl;
    break;
  }
  case 3: {
    int marks = getIntInput("Enter marks achieved (0-100): ", 0, 100);
    assignment->getState()->complete(marks);
    std::cout << "Assignment completed. New state: "
              << assignment->getStateName() << std::endl;
    break;
  }
  case 4:
    assignment->checkDeadline();
    std::cout << "Deadline checked. Current state: "
              << assignment->getStateName() << std::endl;
    break;
  default:
    std::cout << "Invalid action." << std::endl;
  }
}

// Function to process an assignment using the Template Method pattern
void processAssignmentWithTemplate() {
  std::cout << "\n===== Process Assignment with Template Method =====\n";

  auto course = selectCourse();
  if (!course)
    return;

  auto assignments = course->getAssignments();

  if (assignments.empty()) {
    std::cout << "No assignments for this course.\n";
    return;
  }

  std::cout << "Assignments for " << course->getName() << ":\n";
  for (size_t i = 0; i < assignments.size(); ++i) {
    auto &assignment = assignments[i];
    std::cout << i + 1 << ". " << assignment->getTitle() << " ("
              << assignment->getType() << ")\n";
  }

  int choice = getIntInput("Select an assignment to process (1-" +
                               std::to_string(assignments.size()) + "): ",
                           1, assignments.size());
  auto assignment = assignments[choice - 1];

  std::string submission = getStringInput("Enter submission content: ");

  // Create the appropriate processor based on assignment type
  std::shared_ptr<AssignmentProcess> processor;

  if (auto hw = std::dynamic_pointer_cast<HomeworkAssignment>(assignment)) {
    processor = std::make_shared<HomeworkProcess>();
  } else if (auto proj =
                 std::dynamic_pointer_cast<ProjectAssignment>(assignment)) {
    processor = std::make_shared<ProjectProcess>();
  } else if (auto exam =
                 std::dynamic_pointer_cast<ExamAssignment>(assignment)) {
    processor = std::make_shared<ExamProcess>();
  } else {
    // Default to homework processor
    processor = std::make_shared<HomeworkProcess>();
  }

  // Process the assignment using the template method
  processor->processAssignment(assignment, submission);
}

// Function to subscribe to assignment updates using the Observer pattern
void subscribeToAssignment() {
  std::cout << "\n===== Subscribe to Assignment Updates =====\n";

  // Make sure the student observer is set up
  if (!studentObserver) {
    std::string name = getStringInput("Enter your name: ");
    std::string id = getStringInput("Enter your student ID: ");
    studentObserver = std::make_shared<StudentObserver>(id, name);
  }

  auto course = selectCourse();
  if (!course)
    return;

  auto assignments = course->getAssignments();

  if (assignments.empty()) {
    std::cout << "No assignments for this course.\n";
    return;
  }

  std::cout << "Assignments for " << course->getName() << ":\n";
  for (size_t i = 0; i < assignments.size(); ++i) {
    auto &assignment = assignments[i];
    std::cout << i + 1 << ". " << assignment->getTitle() << " ("
              << assignment->getType() << ")\n";
  }

  int choice = getIntInput("Select an assignment to subscribe to (1-" +
                             std::to_string(assignments.size()) + "): ",
                           1, assignments.size());
  auto assignment = assignments[choice - 1];

  // We'll use a simple ObservableAssignment for this demo
  std::shared_ptr<ObservableAssignment> observableAssignment = 
      std::make_shared<ObservableAssignment>();
  
  std::cout << "Creating an observable wrapper for this assignment..." << std::endl;
  // Here we would create an adapter or wrapper to make the assignment observable
  // For this demo, we'll just use a new mock ObservableAssignment
  
  std::cout << "Select observer type:\n";
  std::cout << "1. Student Observer (general updates)\n";
  std::cout << "2. Deadline Observer (deadline warnings)\n";
  std::cout << "3. Grade Observer (grade updates)\n";
  std::cout << "4. Progress Observer (progress updates)\n";
  
  int observerType = getIntInput("Enter observer type (1-4): ", 1, 4);

  std::shared_ptr<Observer> observer;

  switch (observerType) {
  case 1:
    observer = studentObserver;
    break;
  case 2:
    if (!deadlineObserver) {
      int days = getIntInput("Days before deadline to notify: ", 1, 14);
      deadlineObserver = std::make_shared<DeadlineObserver>(days);
    }
    observer = deadlineObserver;
    break;
  case 3:
    observer = std::make_shared<GradeObserver>();
    break;
  case 4:
    observer = std::make_shared<ProgressObserver>();
    break;
  default:
    observer = studentObserver;
  }

  // Attach the observer
  observableAssignment->attach(observer);

  std::cout << "Observer attached successfully. You will now receive "
               "notifications for this assignment."
            << std::endl;

  // Trigger a test notification
  std::cout << "\nSending test notification..." << std::endl;
  // Use the assignment state name for the notification
  observableAssignment->stateChanged(assignment ? assignment->getStateName() : "Unknown");
}

// Function prototypes
void markAssignmentAsCompleted();
void decorateAssignment();
void createCompositeAssignment();
void submitAssignmentToGradingSystem();
void switchGradingSystem();
void saveApplicationState();
void restoreApplicationState();
void updateAssignmentState();
void processAssignmentWithTemplate();
void subscribeToAssignment();

// Main menu function
void showMainMenu() {
  std::cout << "\n===== Academic Progress Tracker =====\n";
  std::cout << "1. Create a new course\n";
  std::cout << "2. Create a new assignment\n";
  std::cout << "3. View courses\n";
  std::cout << "4. View assignments\n";
  std::cout << "5. View assignment details\n";
  std::cout << "6. Check notifications\n";
  std::cout << "7. View all notifications\n";
  std::cout << "8. Mark assignment as completed\n";
  std::cout << "9. Decorate assignment (Add priority/tags/color)\n";
  std::cout << "10. Create composite assignment\n";
  std::cout << "11. Submit assignment to grading system\n";
  std::cout << "12. Switch grading system\n";
  std::cout << "13. Save application state\n";
  std::cout << "14. Restore application state\n";
  std::cout << "15. Update assignment state\n";
  std::cout << "16. Process assignment with template method\n";
  std::cout << "17. Subscribe to assignment updates\n";
  std::cout << "0. Exit\n";
  std::cout << "=================================\n";
}

int main() {
  std::cout << "Welcome to the Academic Progress Tracker!\n";

  // Initialize the application
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
      createCourse();
      break;
    case 2:
      createAssignment();
      break;
    case 3:
      listCourses();
      break;
    case 4:
      listAssignments();
      break;
    case 5:
      viewAssignmentDetails();
      break;
    case 6:
      checkNotifications();
      break;
    case 7:
      viewAllNotifications();
      break;
    case 8:
      markAssignmentAsCompleted();
      break;
    case 9:
      decorateAssignment();
      break;
    case 10:
      createCompositeAssignment();
      break;
    case 11:
      submitAssignmentToGradingSystem();
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
      updateAssignmentState();
      break;
    case 16:
      processAssignmentWithTemplate();
      break;
    case 17:
      subscribeToAssignment();
      break;
    default:
      std::cout << "Invalid choice. Please try again.\n";
    }
  }

  std::cout << "Thank you for using the Academic Progress Tracker!\n";
  return 0;
}

// Function to mark an assignment as completed
void markAssignmentAsCompleted() {
  std::cout << "\n===== Mark Assignment as Completed =====\n";

  auto course = selectCourse();
  if (!course)
    return;

  auto assignments = course->getAssignments();

  if (assignments.empty()) {
    std::cout << "No assignments for this course.\n";
    return;
  }

  std::cout << "Assignments for " << course->getName() << ":\n";
  for (size_t i = 0; i < assignments.size(); ++i) {
    auto &assignment = assignments[i];

    auto time = std::chrono::system_clock::to_time_t(assignment->getDeadline());
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M");

    std::cout << i + 1 << ". " << assignment->getTitle() << " ("
              << assignment->getType() << ")"
              << " - Due: " << ss.str()
              << (assignment->isCompleted() ? " - Completed"
                                            : " - Not completed")
              << "\n";
  }

  int choice = getIntInput("Select an assignment (1-" +
                               std::to_string(assignments.size()) + "): ",
                           1, assignments.size());
  auto assignment = assignments[choice - 1];

  bool isCompleted =
      getIntInput("Mark as completed? (1: Yes, 0: No): ", 0, 1) == 1;

  if (isCompleted) {
    int marks = getIntInput("Enter marks received (0-100): ", 0, 100);
    assignment->setCompleted(true);
    assignment->setMarks(marks);
    std::cout << "Assignment marked as completed with " << marks << " marks.\n";

    // Show grade representation from the grading system
    std::string grade = gradingSystemFacade->getGradeRepresentation(marks);
    std::cout << "Grade in " << gradingSystemFacade->getActiveSystemName()
              << ": " << grade << "\n";
  } else {
    assignment->setCompleted(false);
    assignment->setMarks(0);
    std::cout << "Assignment marked as not completed.\n";
  }
}

// Function to decorate an assignment with priority, tags, or color
void decorateAssignment() {
  std::cout << "\n===== Decorate Assignment =====\n";

  auto course = selectCourse();
  if (!course)
    return;

  auto assignments = course->getAssignments();

  if (assignments.empty()) {
    std::cout << "No assignments for this course.\n";
    return;
  }

  std::cout << "Assignments for " << course->getName() << ":\n";
  for (size_t i = 0; i < assignments.size(); ++i) {
    auto &assignment = assignments[i];
    std::cout << i + 1 << ". " << assignment->getTitle() << " ("
              << assignment->getType() << ")\n";
  }

  int choice = getIntInput("Select an assignment to decorate (1-" +
                               std::to_string(assignments.size()) + "): ",
                           1, assignments.size());
  auto baseAssignment = assignments[choice - 1];

  std::cout << "Select decoration type:\n";
  std::cout << "1. Add priority level\n";
  std::cout << "2. Add tags\n";
  std::cout << "3. Add color coding\n";
  int decorationType = getIntInput("Enter decoration type (1-3): ", 1, 3);

  std::shared_ptr<Assignment> decoratedAssignment;

  switch (decorationType) {
  case 1: {
    int priorityLevel =
        getIntInput("Enter priority level (1-5, with 5 being highest): ", 1, 5);
    decoratedAssignment = std::make_shared<PriorityAssignmentDecorator>(
        baseAssignment, priorityLevel);
    std::cout << "Priority level " << priorityLevel
              << " added to assignment.\n";
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

    decoratedAssignment =
        std::make_shared<TaggedAssignmentDecorator>(baseAssignment, tags);
    std::cout << tags.size() << " tags added to assignment.\n";
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
    ColoredAssignmentDecorator::Color color;

    switch (colorChoice) {
    case 1:
      color = ColoredAssignmentDecorator::Color::RED;
      break;
    case 2:
      color = ColoredAssignmentDecorator::Color::YELLOW;
      break;
    case 3:
      color = ColoredAssignmentDecorator::Color::GREEN;
      break;
    case 4:
      color = ColoredAssignmentDecorator::Color::BLUE;
      break;
    case 5:
      color = ColoredAssignmentDecorator::Color::PURPLE;
      break;
    default:
      color = ColoredAssignmentDecorator::Color::DEFAULT;
    }

    decoratedAssignment =
        std::make_shared<ColoredAssignmentDecorator>(baseAssignment, color);
    std::cout << "Color added to assignment.\n";
    break;
  }
  }

  if (decoratedAssignment) {
    // Replace the assignment in the course with the decorated version
    course->removeAssignment(baseAssignment->getTitle());
    course->addAssignment(decoratedAssignment);

    std::cout << "Assignment successfully decorated!\n";
    decoratedAssignment->displayInfo();
  }
}

// Function to create a composite assignment (with sub-assignments)
void createCompositeAssignment() {
  std::cout << "\n===== Create Composite Assignment =====\n";

  auto course = selectCourse();
  if (!course)
    return;

  std::string title = getStringInput("Enter composite assignment title: ");
  if (title.empty()) {
    std::cout << "Assignment title cannot be empty.\n";
    return;
  }

  std::string description =
      getStringInput("Enter composite assignment description (optional): ");
  DateTime deadline = getDateTimeInput("Enter composite assignment deadline");

  // Create the composite assignment
  auto compositeAssignment =
      std::make_shared<CompositeAssignment>(title, deadline, description);

  // Add child assignments
  bool addingChildren = true;
  while (addingChildren) {
    std::cout << "\nAdd child assignment to " << title << ":\n";
    std::cout << "1. Create new child assignment\n";
    std::cout << "2. Done adding child assignments\n";

    int choice = getIntInput("Enter your choice (1-2): ", 1, 2);

    if (choice == 2) {
      addingChildren = false;
      continue;
    }

    // Create a child assignment
    std::string childTitle = getStringInput("Enter child assignment title: ");
    if (childTitle.empty()) {
      std::cout << "Assignment title cannot be empty. Skipping this child.\n";
      continue;
    }

    std::string childDescription =
        getStringInput("Enter child assignment description (optional): ");

    // For simplicity, child assignments inherit parent's deadline
    // But you could also get a separate deadline if needed

    // Show assignment type options
    std::cout << "Select child assignment type:\n";
    std::cout << "1. Homework\n";
    std::cout << "2. Project\n";
    std::cout << "3. Exam\n";
    int typeChoice = getIntInput("Enter your choice (1-3): ", 1, 3);

    std::shared_ptr<Assignment> childAssignment;

    switch (typeChoice) {
    case 1:
      childAssignment = std::make_shared<HomeworkAssignment>(
          childTitle, deadline, childDescription);
      break;
    case 2:
      childAssignment = std::make_shared<ProjectAssignment>(
          childTitle, deadline, childDescription);
      break;
    case 3:
      childAssignment = std::make_shared<ExamAssignment>(childTitle, deadline,
                                                         childDescription);
      break;
    }

    // Add the child to the composite
    compositeAssignment->addChildAssignment(childAssignment);
    std::cout << "Child assignment added: " << childTitle << "\n";
  }

  // Add the composite assignment to the course
  course->addAssignment(compositeAssignment);

  std::cout << "Composite assignment created with "
            << compositeAssignment->getChildAssignments().size()
            << " child assignments.\n";
}

// Function to submit an assignment to the grading system
void submitAssignmentToGradingSystem() {
  std::cout << "\n===== Submit Assignment to Grading System =====\n";
  std::cout << "Using grading system: "
            << gradingSystemFacade->getActiveSystemName() << "\n";

  auto course = selectCourse();
  if (!course)
    return;

  auto assignments = course->getAssignments();

  if (assignments.empty()) {
    std::cout << "No assignments for this course.\n";
    return;
  }

  std::cout << "Assignments for " << course->getName() << ":\n";
  for (size_t i = 0; i < assignments.size(); ++i) {
    auto &assignment = assignments[i];

    auto time = std::chrono::system_clock::to_time_t(assignment->getDeadline());
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M");

    std::cout << i + 1 << ". " << assignment->getTitle() << " ("
              << assignment->getType() << ")"
              << " - Due: " << ss.str() << "\n";
  }

  int choice = getIntInput("Select an assignment to submit (1-" +
                               std::to_string(assignments.size()) + "): ",
                           1, assignments.size());
  auto assignment = assignments[choice - 1];

  std::string submissionContent =
      getStringInput("Enter submission content (e.g., file path or text): ");

  try {
    // Submit the assignment using the facade
    gradingSystemFacade->submitAssignment(assignment, submissionContent);

    // Check if the submission was successful and update the assignment status
    if (gradingSystemFacade->isCompleted(assignment)) {
      int score = gradingSystemFacade->getScore(assignment);
      assignment->setCompleted(true);
      assignment->setMarks(score);

      std::string grade = gradingSystemFacade->getGradeRepresentation(score);

      std::cout << "Assignment submitted and graded automatically.\n";
      std::cout << "Score: " << score << " out of 100\n";
      std::cout << "Grade: " << grade << "\n";
    } else {
      std::cout << "Assignment submitted but not yet graded.\n";
    }
  } catch (const std::exception &e) {
    std::cout << "Error submitting assignment: " << e.what() << "\n";
  }
}

// Function to switch between grading systems
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
