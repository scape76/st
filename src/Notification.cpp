#include "Notification.h"
#include "Course.h"
#include "Assignment.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <algorithm>

// Helper function to format DateTime
std::string formatDateTimeNotif(const DateTime& dt) {
    auto time = std::chrono::system_clock::to_time_t(dt);
    std::tm tm_local = {};
    
    #ifdef _WIN32
    localtime_s(&tm_local, &time);
    #else
    tm_local = *std::localtime(&time);
    #endif
    
    // Use local time to preserve user input
    std::stringstream ss;
    ss << std::put_time(&tm_local, "%Y-%m-%d %H:%M");
    return ss.str();
}

// Initialize static members of NotificationManager
std::unique_ptr<NotificationManager, std::function<void(NotificationManager*)>> 
    NotificationManager::instance{nullptr, [](NotificationManager* p) {}};
std::mutex NotificationManager::mutex;

// Base Notification implementation
Notification::Notification(const std::string& message, 
                         const DateTime& triggerTime,
                         std::shared_ptr<Assignment> assignment)
    : message(message), triggerTime(triggerTime), assignment(assignment) {}

std::string Notification::getMessage() const {
    return message;
}

DateTime Notification::getTriggerTime() const {
    return triggerTime;
}

std::shared_ptr<Assignment> Notification::getAssignment() const {
    return assignment.lock(); // Convert weak_ptr to shared_ptr
}

void Notification::setMessage(const std::string& message) {
    this->message = message;
}

void Notification::setTriggerTime(const DateTime& triggerTime) {
    this->triggerTime = triggerTime;
}

bool Notification::shouldTrigger(const DateTime& currentTime) const {
    return currentTime >= triggerTime;
}

void Notification::display() const {
    std::cout << "NOTIFICATION: " << message << std::endl;
    
    auto assignmentPtr = assignment.lock();
    if (assignmentPtr) {
        std::cout << "For assignment: " << assignmentPtr->getTitle() << std::endl;
        std::cout << "Due: " << formatDateTimeNotif(assignmentPtr->getDeadline()) << std::endl;
    }
    
    std::cout << "Trigger time: " << formatDateTimeNotif(triggerTime) << std::endl;
}

// DeadlineNotification implementation
DeadlineNotification::DeadlineNotification(const std::string& message,
                                         std::shared_ptr<Assignment> assignment,
                                         int daysBeforeDeadline)
    : Notification(message, 
                 std::chrono::system_clock::time_point(), // Will be calculated
                 assignment),
      daysBeforeDeadline(daysBeforeDeadline) {
    
    // Calculate the trigger time based on the assignment deadline
    auto deadline = assignment->getDeadline();
    auto triggerTime = deadline - std::chrono::hours(24 * daysBeforeDeadline);
    setTriggerTime(triggerTime);
}

int DeadlineNotification::getDaysBeforeDeadline() const {
    return daysBeforeDeadline;
}

void DeadlineNotification::setDaysBeforeDeadline(int days) {
    daysBeforeDeadline = days;
    
    // Recalculate trigger time
    auto assignmentPtr = getAssignment();
    if (assignmentPtr) {
        auto deadline = assignmentPtr->getDeadline();
        auto triggerTime = deadline - std::chrono::hours(24 * daysBeforeDeadline);
        setTriggerTime(triggerTime);
    }
}

bool DeadlineNotification::shouldTrigger(const DateTime& currentTime) const {
    auto assignmentPtr = getAssignment();
    if (!assignmentPtr) {
        return false; // Assignment no longer exists
    }
    
    // Get the current time and deadline
    auto deadline = assignmentPtr->getDeadline();
    auto timeToDeadline = std::chrono::duration_cast<std::chrono::hours>(
        deadline - currentTime).count();
    
    // Calculate the range for triggering (between daysBeforeDeadline and daysBeforeDeadline-1)
    auto upperBound = daysBeforeDeadline * 24;
    auto lowerBound = upperBound - 24; // One day window for notifications
    
    // Trigger if we're within the notification window
    return (timeToDeadline <= upperBound && timeToDeadline > lowerBound);
}

void DeadlineNotification::display() const {
    auto assignmentPtr = getAssignment();
    if (!assignmentPtr) {
        std::cout << "NOTIFICATION: Assignment no longer exists." << std::endl;
        return;
    }
    
    std::cout << "DEADLINE REMINDER: " << getMessage() << std::endl;
    std::cout << "Assignment: " << assignmentPtr->getTitle() << std::endl;
    std::cout << "Due in " << daysBeforeDeadline << " days on " 
              << formatDateTimeNotif(assignmentPtr->getDeadline()) << std::endl;
    
    if (auto course = assignmentPtr->getCourse()) {
        std::cout << "Course: " << course->getName() << " (" << course->getCode() << ")" << std::endl;
    }
}

// NotificationManager implementation
NotificationManager& NotificationManager::getInstance() {
    std::lock_guard<std::mutex> lock(mutex);
    if (instance == nullptr) {
        instance.reset(new NotificationManager());
    }
    return *instance.get();
}

void NotificationManager::addNotification(std::shared_ptr<Notification> notification) {
    notifications.push_back(notification);
    
    // Also add to the assignment's notifications list
    if (auto assignment = notification->getAssignment()) {
        assignment->addNotification(notification);
    }
}

void NotificationManager::removeNotification(size_t index) {
    if (index < notifications.size()) {
        // Remove the notification from its assignment too
        if (auto assignment = notifications[index]->getAssignment()) {
            auto assignmentNotifs = assignment->getNotifications();
            auto it = std::find(assignmentNotifs.begin(), assignmentNotifs.end(), notifications[index]);
            if (it != assignmentNotifs.end()) {
                assignment->removeNotification(std::distance(assignmentNotifs.begin(), it));
            }
        }
        
        notifications.erase(notifications.begin() + index);
    } else {
        std::cout << "Invalid notification index." << std::endl;
    }
}

void NotificationManager::checkNotifications() {
    auto now = std::chrono::system_clock::now();
    bool foundNotifications = false;
    
    std::cout << "Current time: " << formatDateTimeNotif(now) << std::endl;
    
    for (const auto& notification : notifications) {
        if (notification->shouldTrigger(now)) {
            notification->display();
            foundNotifications = true;
        }
    }
    
    if (!foundNotifications) {
        if (notifications.empty()) {
            std::cout << "You have no notifications set up." << std::endl;
        } else {
            std::cout << "No notifications are due at this time." << std::endl;
            
            // Display when the next notification would be due
            std::cout << "You have " << notifications.size() << " notification(s) scheduled." << std::endl;
            
            if (auto deadlineNotif = std::dynamic_pointer_cast<DeadlineNotification>(notifications[0])) {
                if (auto assignment = deadlineNotif->getAssignment()) {
                    std::cout << "Next deadline: " << assignment->getTitle() 
                              << " on " << formatDateTimeNotif(assignment->getDeadline()) << std::endl;
                }
            }
        }
    }
}

std::vector<std::shared_ptr<Notification>> NotificationManager::getNotifications() const {
    return notifications;
}

std::vector<std::shared_ptr<Notification>> NotificationManager::getNotificationsForAssignment(
    std::shared_ptr<Assignment> assignment) const {
    
    std::vector<std::shared_ptr<Notification>> result;
    
    for (const auto& notification : notifications) {
        if (auto notifAssignment = notification->getAssignment()) {
            if (notifAssignment == assignment) {
                result.push_back(notification);
            }
        }
    }
    
    return result;
}