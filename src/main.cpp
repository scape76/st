#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <limits>

#include "../include/Course.h"
#include "../include/Assignment.h"
#include "../include/AssignmentBuilder.h"
#include "../include/Notification.h"

// Global containers to store courses and assignments
std::vector<std::shared_ptr<Course>> courses;

// Helper functions
void clearInputBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int getIntInput(const std::string& prompt, int min = std::numeric_limits<int>::min(), int max = std::numeric_limits<int>::max()) {
    int value;
    bool validInput = false;
    
    do {
        std::cout << prompt;
        if (std::cin >> value) {
            if (value >= min && value <= max) {
                validInput = true;
            } else {
                std::cout << "Value must be between " << min << " and " << max << "." << std::endl;
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

std::string getStringInput(const std::string& prompt) {
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

DateTime getDateTimeInput(const std::string& prompt) {
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
    
    // Convert std::tm to system_clock::time_point
    std::time_t time = std::mktime(&timeinfo);
    return std::chrono::system_clock::from_time_t(time);
}

// Function to create a new course
void createCourse() {
    std::cout << "\n===== Create New Course =====\n";
    
    std::string name, code, description;
    
    do {
        name = getStringInput("Enter course name: ");
        if (name.empty()) {
            std::cout << "Course name cannot be empty. Please try again." << std::endl;
        }
    } while (name.empty());
    
    do {
        code = getStringInput("Enter course code: ");
        if (code.empty()) {
            std::cout << "Course code cannot be empty. Please try again." << std::endl;
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
    } catch (const std::exception& e) {
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
        std::cout << i + 1 << ". " << courses[i]->getName() << " (" << courses[i]->getCode() << ")\n";
    }
}

// Function to select a course from the list
std::shared_ptr<Course> selectCourse() {
    if (courses.empty()) {
        std::cout << "No courses available. Create a course first.\n";
        return nullptr;
    }
    
    listCourses();
    int choice = getIntInput("Select a course (1-" + std::to_string(courses.size()) + "): ", 1, courses.size());
    return courses[choice - 1];
}

// Function to create a new assignment
void createAssignment() {
    std::cout << "\n===== Create New Assignment =====\n";
    
    auto course = selectCourse();
    if (!course) return;
    
    std::string title;
    do {
        title = getStringInput("Enter assignment title: ");
        if (title.empty()) {
            std::cout << "Assignment title cannot be empty. Please try again." << std::endl;
        }
    } while (title.empty());
    
    std::string description = getStringInput("Enter assignment description (optional): ");
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
        builder.setTitle(title)
               .setDescription(description)
               .setDeadline(deadline);
        
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
            int days = getIntInput("How many days before the deadline do you want to be notified? ", 1, 30);
            std::string message = "Reminder for " + title;
            
            auto notification = std::make_shared<DeadlineNotification>(
                message, assignment, days
            );
            
            // Using the Singleton pattern to access the NotificationManager
            NotificationManager::getInstance().addNotification(notification);
            std::cout << "Notification set for " << days << " days before the deadline.\n";
        }
        
        std::cout << "Assignment created successfully!\n";
    } catch (const std::exception& e) {
        std::cout << "Error creating assignment: " << e.what() << std::endl;
    }
}

// Function to list assignments for a course
void listAssignments() {
    std::cout << "\n===== Assignments =====\n";
    
    auto course = selectCourse();
    if (!course) return;
    
    auto assignments = course->getAssignments();
    
    if (assignments.empty()) {
        std::cout << "No assignments for this course.\n";
        return;
    }
    
    std::cout << "Assignments for " << course->getName() << ":\n";
    for (size_t i = 0; i < assignments.size(); ++i) {
        auto& assignment = assignments[i];
        
        auto time = std::chrono::system_clock::to_time_t(assignment->getDeadline());
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M");
        
        std::cout << i + 1 << ". " << assignment->getTitle() 
                  << " (" << assignment->getType() << ")"
                  << " - Due: " << ss.str() << "\n";
    }
}

// Function to view assignment details
void viewAssignmentDetails() {
    std::cout << "\n===== Assignment Details =====\n";
    
    auto course = selectCourse();
    if (!course) return;
    
    auto assignments = course->getAssignments();
    
    if (assignments.empty()) {
        std::cout << "No assignments for this course.\n";
        return;
    }
    
    std::cout << "Assignments for " << course->getName() << ":\n";
    for (size_t i = 0; i < assignments.size(); ++i) {
        std::cout << i + 1 << ". " << assignments[i]->getTitle() << "\n";
    }
    
    int choice = getIntInput("Select an assignment (1-" + std::to_string(assignments.size()) + "): ", 1, assignments.size());
    auto assignment = assignments[choice - 1];
    
    std::cout << "\n";
    assignment->displayInfo();
    
    // Show notifications for this assignment
    auto& notificationManager = NotificationManager::getInstance();
    auto notifications = notificationManager.getNotificationsForAssignment(assignment);
    
    if (!notifications.empty()) {
        std::cout << "\nNotifications for this assignment:\n";
        for (size_t i = 0; i < notifications.size(); ++i) {
            auto& notification = notifications[i];
            
            if (auto deadlineNotif = std::dynamic_pointer_cast<DeadlineNotification>(notification)) {
                std::cout << i + 1 << ". " << deadlineNotif->getDaysBeforeDeadline() 
                          << " days before deadline: " << deadlineNotif->getMessage() << "\n";
            } else {
                auto time = std::chrono::system_clock::to_time_t(notification->getTriggerTime());
                std::stringstream ss;
                ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M");
                
                std::cout << i + 1 << ". At " << ss.str() << ": " << notification->getMessage() << "\n";
            }
        }
    } else {
        std::cout << "\nNo notifications set for this assignment.\n";
    }
}

// Function to check for notifications
void checkNotifications() {
    std::cout << "\n===== Checking Notifications =====\n";
    
    auto& notificationManager = NotificationManager::getInstance();
    notificationManager.checkNotifications();
}

// Main menu function
void showMainMenu() {
    std::cout << "\n===== Academic Progress Tracker =====\n";
    std::cout << "1. Create a new course\n";
    std::cout << "2. Create a new assignment\n";
    std::cout << "3. View courses\n";
    std::cout << "4. View assignments\n";
    std::cout << "5. View assignment details\n";
    std::cout << "6. Check notifications\n";
    std::cout << "0. Exit\n";
    std::cout << "=================================\n";
}

int main() {
    std::cout << "Welcome to the Academic Progress Tracker!\n";
    
    bool running = true;
    while (running) {
        showMainMenu();
        int choice = getIntInput("Enter your choice: ", 0, 6);
        
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
            default:
                std::cout << "Invalid choice. Please try again.\n";
        }
    }
    
    std::cout << "Thank you for using the Academic Progress Tracker!\n";
    return 0;
}