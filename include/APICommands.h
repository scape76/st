#ifndef API_COMMANDS_H
#define API_COMMANDS_H

#include <memory>
#include <string>
#include <unordered_map>
#include <functional>

#include "APIFacade.h"
#include "Course.h"
#include "Assignment.h"
#include "Notification.h"

// Command factory for creating command instances
class CommandFactory {
private:
    std::unordered_map<std::string, std::function<std::shared_ptr<APICommand>()>> registry;

public:
    static CommandFactory& getInstance();
    
    void registerCommand(const std::string& endpoint, std::function<std::shared_ptr<APICommand>()> creator);
    std::shared_ptr<APICommand> createCommand(const std::string& endpoint);
};

// Command dispatcher
class CommandDispatcher {
public:
    static APIResponse dispatch(const APIRequest& request);
};

// Base class for course commands
class CourseCommand : public APICommand {
protected:
    std::vector<std::shared_ptr<Course>>& getCourses();
};

// Concrete course commands
class GetAllCoursesCommand : public CourseCommand {
public:
    APIResponse execute(const APIRequest& request) override;
};

class GetCourseCommand : public CourseCommand {
public:
    APIResponse execute(const APIRequest& request) override;
};

class CreateCourseCommand : public CourseCommand {
public:
    APIResponse execute(const APIRequest& request) override;
};

class UpdateCourseCommand : public CourseCommand {
public:
    APIResponse execute(const APIRequest& request) override;
};

class DeleteCourseCommand : public CourseCommand {
public:
    APIResponse execute(const APIRequest& request) override;
};

// Base class for assignment commands
class AssignmentCommand : public APICommand {
protected:
    std::shared_ptr<Course> findCourse(const std::string& courseId);
    std::shared_ptr<Assignment> findAssignment(std::shared_ptr<Course> course, const std::string& assignmentId);
};

// Concrete assignment commands
class GetAllAssignmentsCommand : public AssignmentCommand {
public:
    APIResponse execute(const APIRequest& request) override;
};

class GetAssignmentCommand : public AssignmentCommand {
public:
    APIResponse execute(const APIRequest& request) override;
};

class CreateAssignmentCommand : public AssignmentCommand {
public:
    APIResponse execute(const APIRequest& request) override;
};

class UpdateAssignmentCommand : public AssignmentCommand {
public:
    APIResponse execute(const APIRequest& request) override;
};

class DeleteAssignmentCommand : public AssignmentCommand {
public:
    APIResponse execute(const APIRequest& request) override;
};

class UpdateAssignmentStateCommand : public AssignmentCommand {
public:
    APIResponse execute(const APIRequest& request) override;
};

class MarkAssignmentAsCompletedCommand : public AssignmentCommand {
public:
    APIResponse execute(const APIRequest& request) override;
};

// Notification commands
class GetAllNotificationsCommand : public APICommand {
public:
    APIResponse execute(const APIRequest& request) override;
};

class CreateNotificationCommand : public APICommand {
public:
    APIResponse execute(const APIRequest& request) override;
};

class DeleteNotificationCommand : public APICommand {
public:
    APIResponse execute(const APIRequest& request) override;
};

// Memento commands
class SaveApplicationStateCommand : public APICommand {
public:
    APIResponse execute(const APIRequest& request) override;
};

class GetSavedStatesCommand : public APICommand {
public:
    APIResponse execute(const APIRequest& request) override;
};

class RestoreStateCommand : public APICommand {
public:
    APIResponse execute(const APIRequest& request) override;
};

// Grading system commands
class GetAvailableGradingSystemsCommand : public APICommand {
public:
    APIResponse execute(const APIRequest& request) override;
};

class SetActiveGradingSystemCommand : public APICommand {
public:
    APIResponse execute(const APIRequest& request) override;
};

class SubmitAssignmentToGradingSystemCommand : public AssignmentCommand {
public:
    APIResponse execute(const APIRequest& request) override;
};

// Observer pattern commands
class SubscribeToAssignmentCommand : public AssignmentCommand {
public:
    APIResponse execute(const APIRequest& request) override;
};

// Template method pattern commands
class ProcessAssignmentCommand : public AssignmentCommand {
public:
    APIResponse execute(const APIRequest& request) override;
};

// Decorator pattern commands
class DecorateAssignmentCommand : public AssignmentCommand {
public:
    APIResponse execute(const APIRequest& request) override;
};

// Composite pattern commands
class CreateCompositeAssignmentCommand : public AssignmentCommand {
public:
    APIResponse execute(const APIRequest& request) override;
};

class AddChildAssignmentCommand : public AssignmentCommand {
public:
    APIResponse execute(const APIRequest& request) override;
};

#endif // API_COMMANDS_H