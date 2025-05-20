#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <chrono>
#include <mutex>
#include <functional>

// Forward declaration
class Assignment;

// Using a type alias for date/time
using DateTime = std::chrono::system_clock::time_point;

// Base Notification class
class Notification {
private:
    std::string message;
    DateTime triggerTime;
    std::weak_ptr<Assignment> assignment; // Weak pointer to avoid circular reference

public:
    // Constructor
    Notification(const std::string& message, 
                 const DateTime& triggerTime,
                 std::shared_ptr<Assignment> assignment);
    
    // Virtual destructor for polymorphism
    virtual ~Notification() = default;
    
    // Getters
    std::string getMessage() const;
    DateTime getTriggerTime() const;
    std::shared_ptr<Assignment> getAssignment() const;
    
    // Setters
    void setMessage(const std::string& message);
    void setTriggerTime(const DateTime& triggerTime);
    
    // Check if notification should be triggered
    virtual bool shouldTrigger(const DateTime& currentTime) const;
    
    // Display notification
    virtual void display() const;
};

// Concrete notification types
class DeadlineNotification : public Notification {
private:
    int daysBeforeDeadline;
    
public:
    DeadlineNotification(const std::string& message,
                         std::shared_ptr<Assignment> assignment,
                         int daysBeforeDeadline);
    
    int getDaysBeforeDeadline() const;
    void setDaysBeforeDeadline(int days);
    
    bool shouldTrigger(const DateTime& currentTime) const override;
    void display() const override;
};

// NotificationManager class (Singleton Pattern)
class NotificationManager {
private:
    // Private constructor and destructor
    NotificationManager() = default;
    ~NotificationManager() = default;
    
    // Delete copy constructor and assignment operator
    NotificationManager(const NotificationManager&) = delete;
    NotificationManager& operator=(const NotificationManager&) = delete;
    
    // Singleton instance with custom deleter to handle private destructor
    static std::unique_ptr<NotificationManager, std::function<void(NotificationManager*)>> instance;
    static std::mutex mutex;
    
    // List of all notifications
    std::vector<std::shared_ptr<Notification>> notifications;

public:
    // Get singleton instance
    static NotificationManager& getInstance();
    
    // Add a notification
    void addNotification(std::shared_ptr<Notification> notification);
    
    // Remove a notification
    void removeNotification(size_t index);
    
    // Check and trigger notifications
    void checkNotifications();
    
    // Get all notifications
    std::vector<std::shared_ptr<Notification>> getNotifications() const;
    
    // Get notifications for a specific assignment
    std::vector<std::shared_ptr<Notification>> getNotificationsForAssignment(
        std::shared_ptr<Assignment> assignment) const;
};

#endif // NOTIFICATION_H