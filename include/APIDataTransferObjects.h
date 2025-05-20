#ifndef API_DATA_TRANSFER_OBJECTS_H
#define API_DATA_TRANSFER_OBJECTS_H

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <optional>
#include <nlohmann/json.hpp>

namespace api {
namespace dto {

// Forward declarations
struct CourseDTO;
struct AssignmentDTO;
struct NotificationDTO;
struct StateDTO;
struct GradingSystemDTO;
struct MementoDTO;

// Course Data Transfer Object
struct CourseDTO {
    std::string id;
    std::string name;
    std::string code;
    std::string description;
    std::vector<std::string> assignmentIds;

    // JSON conversion methods
    static CourseDTO fromJSON(const nlohmann::json& json);
    nlohmann::json toJSON() const;
};

// Assignment Data Transfer Object
struct AssignmentDTO {
    std::string id;
    std::string title;
    std::string description;
    std::string type;  // "Homework", "Project", "Exam", or "Composite"
    std::string deadline;  // ISO 8601 format
    std::string courseId;
    std::string stateName;
    std::string stateDescription;
    bool completed;
    int marks;
    float progress;
    std::vector<std::string> notificationIds;
    std::vector<std::string> childAssignmentIds;  // For composite assignments
    std::optional<std::string> parentId;  // For child assignments

    // JSON conversion methods
    static AssignmentDTO fromJSON(const nlohmann::json& json);
    nlohmann::json toJSON() const;
};

// Notification Data Transfer Object
struct NotificationDTO {
    std::string id;
    std::string message;
    std::string triggerTime;  // ISO 8601 format
    std::string assignmentId;
    std::string type;  // "Deadline", "Grade", "Progress", etc.
    bool triggered;

    // JSON conversion methods
    static NotificationDTO fromJSON(const nlohmann::json& json);
    nlohmann::json toJSON() const;
};

// Assignment State Data Transfer Object
struct StateDTO {
    std::string name;  // "Not Started", "In Progress", "Completed", "Overdue"
    std::string description;
    std::string color;
    
    // JSON conversion methods
    static StateDTO fromJSON(const nlohmann::json& json);
    nlohmann::json toJSON() const;
};

// Grading System Data Transfer Object
struct GradingSystemDTO {
    std::string name;
    std::string description;
    bool isActive;

    // JSON conversion methods
    static GradingSystemDTO fromJSON(const nlohmann::json& json);
    nlohmann::json toJSON() const;
};

// Memento (Saved State) Data Transfer Object
struct MementoDTO {
    std::string id;
    std::string timestamp;
    std::string description;
    int numCourses;
    int numAssignments;

    // JSON conversion methods
    static MementoDTO fromJSON(const nlohmann::json& json);
    nlohmann::json toJSON() const;
};

// Observer Data Transfer Object
struct ObserverDTO {
    std::string id;
    std::string type;  // "Student", "Deadline", "Grade", "Progress"
    std::string targetId;  // Assignment or course ID
    
    // JSON conversion methods
    static ObserverDTO fromJSON(const nlohmann::json& json);
    nlohmann::json toJSON() const;
};

// Decorator Data Transfer Object
struct DecoratorDTO {
    std::string type;  // "Priority", "Tagged", "Colored"
    std::string assignmentId;
    nlohmann::json properties;  // Type-specific properties
    
    // JSON conversion methods
    static DecoratorDTO fromJSON(const nlohmann::json& json);
    nlohmann::json toJSON() const;
};

// Composite Assignment Data Transfer Object
struct CompositeDTO {
    AssignmentDTO parent;
    std::vector<AssignmentDTO> children;
    
    // JSON conversion methods
    static CompositeDTO fromJSON(const nlohmann::json& json);
    nlohmann::json toJSON() const;
};

// Assignment Submission Data Transfer Object
struct SubmissionDTO {
    std::string assignmentId;
    std::string courseId;
    std::string content;
    std::string submissionTime;  // ISO 8601 format
    std::optional<int> grade;
    std::optional<std::string> feedback;
    
    // JSON conversion methods
    static SubmissionDTO fromJSON(const nlohmann::json& json);
    nlohmann::json toJSON() const;
};

// Error Response Data Transfer Object
struct ErrorDTO {
    int code;
    std::string message;
    std::optional<std::string> details;
    
    // JSON conversion methods
    static ErrorDTO fromJSON(const nlohmann::json& json);
    nlohmann::json toJSON() const;
};

// Pagination Data Transfer Object
struct PaginationDTO {
    int page;
    int pageSize;
    int totalItems;
    int totalPages;
    
    // JSON conversion methods
    static PaginationDTO fromJSON(const nlohmann::json& json);
    nlohmann::json toJSON() const;
};

// Generic List Response Data Transfer Object
template<typename T>
struct ListDTO {
    std::vector<T> items;
    PaginationDTO pagination;
    
    // JSON conversion methods
    static ListDTO<T> fromJSON(const nlohmann::json& json);
    nlohmann::json toJSON() const;
};

} // namespace dto
} // namespace api

#endif // API_DATA_TRANSFER_OBJECTS_H