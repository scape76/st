#ifndef API_FACADE_H
#define API_FACADE_H

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <optional>
#include <nlohmann/json.hpp>

#include "Course.h"
#include "Assignment.h"
#include "Notification.h"
#include "AssignmentState.h"
#include "Observer.h"
#include "Memento.h"
#include "GradingSystemAdapter.h"

// Forward declarations for API-specific types
class APIRequest;
class APIResponse;

// Command pattern for request handling
class APICommand {
public:
    virtual ~APICommand() = default;
    virtual APIResponse execute(const APIRequest& request) = 0;
};

// API Request and Response structures
class APIRequest {
private:
    std::string endpoint;
    std::string method;
    nlohmann::json data;
    std::map<std::string, std::string> headers;

public:
    APIRequest(const std::string& endpoint, 
              const std::string& method, 
              const nlohmann::json& data = nlohmann::json({}),
              const std::map<std::string, std::string>& headers = {});
    
    std::string getEndpoint() const;
    std::string getMethod() const;
    nlohmann::json getData() const;
    std::map<std::string, std::string> getHeaders() const;
    
    template<typename T>
    std::optional<T> getParam(const std::string& key) const {
        if (data.contains(key)) {
            try {
                return data[key].get<T>();
            } catch (...) {
                return std::nullopt;
            }
        }
        return std::nullopt;
    }
};

class APIResponse {
private:
    int statusCode;
    nlohmann::json data;
    std::map<std::string, std::string> headers;
    std::string message;

public:
    APIResponse(int statusCode = 200, 
               const nlohmann::json& data = nlohmann::json({}),
               const std::string& message = "",
               const std::map<std::string, std::string>& headers = {});
    
    int getStatusCode() const;
    nlohmann::json getData() const;
    std::map<std::string, std::string> getHeaders() const;
    std::string getMessage() const;
    
    std::string toJSON() const;
    std::string toXML() const;
    
    static APIResponse success(const nlohmann::json& data = nlohmann::json({}), 
                              const std::string& message = "Success");
    static APIResponse error(int statusCode, 
                            const std::string& message, 
                            const nlohmann::json& data = nlohmann::json({}));
};

// Adapters for data transformation
class CourseAdapter {
public:
    static nlohmann::json toJSON(const Course& course);
    static nlohmann::json toJSON(const std::shared_ptr<Course>& course);
    static std::shared_ptr<Course> fromJSON(const nlohmann::json& json);
};

class AssignmentAdapter {
public:
    static nlohmann::json toJSON(const Assignment& assignment);
    static nlohmann::json toJSON(const std::shared_ptr<Assignment>& assignment);
    static std::shared_ptr<Assignment> fromJSON(const nlohmann::json& json);
};

class NotificationAdapter {
public:
    static nlohmann::json toJSON(const Notification& notification);
    static nlohmann::json toJSON(const std::shared_ptr<Notification>& notification);
    static std::shared_ptr<Notification> fromJSON(const nlohmann::json& json);
};

// Observer for real-time updates
class APIObserver : public Observer {
private:
    std::function<void(const std::string&, nlohmann::json)> callback;
    
public:
    APIObserver(std::function<void(const std::string&, nlohmann::json)> callback);
    void update(const std::string& message, void* subject) override;
};

// Response format strategy
class ResponseStrategy {
public:
    virtual ~ResponseStrategy() = default;
    virtual std::string format(const APIResponse& response) = 0;
};

class JSONResponseStrategy : public ResponseStrategy {
public:
    std::string format(const APIResponse& response) override;
};

class XMLResponseStrategy : public ResponseStrategy {
public:
    std::string format(const APIResponse& response) override;
};

// Main API Facade class
class APIFacade {
private:
    // Singleton instance
    static std::unique_ptr<APIFacade> instance;
    static std::mutex mutex;
    
    // Command registry
    std::map<std::string, std::shared_ptr<APICommand>> commands;
    
    // Response strategy
    std::shared_ptr<ResponseStrategy> responseStrategy;
    
    // Context data
    std::vector<std::shared_ptr<Course>> courses;
    std::shared_ptr<GradingSystemFacade> gradingSystem;
    MementoCaretaker mementoCaretaker;
    
    // Registered observers for real-time updates
    std::vector<std::shared_ptr<APIObserver>> observers;
    
    // Private constructor for singleton
    APIFacade();
    
public:
    // Get singleton instance
    static APIFacade& getInstance();
    
    // Process an API request
    APIResponse processRequest(const APIRequest& request);
    
    // Set response format strategy
    void setResponseStrategy(std::shared_ptr<ResponseStrategy> strategy);
    
    // Register observer for real-time updates
    void registerObserver(std::shared_ptr<APIObserver> observer);
    
    // Course operations
    APIResponse getAllCourses();
    APIResponse getCourse(const std::string& courseId);
    APIResponse createCourse(const nlohmann::json& courseData);
    APIResponse updateCourse(const std::string& courseId, const nlohmann::json& courseData);
    APIResponse deleteCourse(const std::string& courseId);
    
    // Assignment operations
    APIResponse getAllAssignments(const std::string& courseId);
    APIResponse getAssignment(const std::string& courseId, const std::string& assignmentId);
    APIResponse createAssignment(const std::string& courseId, const nlohmann::json& assignmentData);
    APIResponse updateAssignment(const std::string& courseId, const std::string& assignmentId, 
                                const nlohmann::json& assignmentData);
    APIResponse deleteAssignment(const std::string& courseId, const std::string& assignmentId);
    APIResponse updateAssignmentState(const std::string& courseId, const std::string& assignmentId, 
                                     const std::string& newState);
    APIResponse markAssignmentAsCompleted(const std::string& courseId, const std::string& assignmentId, 
                                         int marks);
    
    // Notification operations
    APIResponse getAllNotifications();
    APIResponse createNotification(const nlohmann::json& notificationData);
    APIResponse deleteNotification(const std::string& notificationId);
    
    // Memento operations (save/restore)
    APIResponse saveApplicationState(const std::string& description);
    APIResponse getSavedStates();
    APIResponse restoreState(const std::string& stateId);
    
    // Grading system operations
    APIResponse getAvailableGradingSystems();
    APIResponse setActiveGradingSystem(const std::string& systemName);
    APIResponse submitAssignmentToGradingSystem(const std::string& courseId, 
                                               const std::string& assignmentId,
                                               const std::string& submissionContent);
    
    // Observer pattern operations
    APIResponse subscribeToAssignment(const std::string& courseId, const std::string& assignmentId,
                                     const std::string& observerType);
    
    // Template method pattern operations
    APIResponse processAssignment(const std::string& courseId, const std::string& assignmentId,
                                 const std::string& submissionContent);
    
    // Decorator pattern operations
    APIResponse decorateAssignment(const std::string& courseId, const std::string& assignmentId,
                                  const std::string& decorationType, const nlohmann::json& decorationData);
    
    // Composite pattern operations
    APIResponse createCompositeAssignment(const std::string& courseId, 
                                         const nlohmann::json& compositeData);
    APIResponse addChildAssignment(const std::string& courseId, const std::string& parentId,
                                  const nlohmann::json& childData);
};

#endif // API_FACADE_H