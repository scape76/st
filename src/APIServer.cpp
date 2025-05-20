#include <iostream>
#include <signal.h>
#include <functional>
#include <string>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <nlohmann/json.hpp>

#include "../include/WebServer.h"
#include "../include/APIFacade.h"
#include "../include/APICommands.h"
#include "../include/APIDataTransferObjects.h"

// Global variables for cleanup
std::unique_ptr<WebServer> server;
bool shouldShutdown = false;

// Signal handler for graceful shutdown
void signalHandler(int signal) {
    std::cout << "Received signal " << signal << ", shutting down..." << std::endl;
    shouldShutdown = true;
    if (server && server->isRunning()) {
        server->stop();
    }
}

// Middleware for request logging
bool logRequestMiddleware(HTTPRequest& request, HTTPResponse& response) {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
    std::cout << "[" << ss.str() << "] " << request.getMethod() << " " << request.getPath();
    
    // Continue processing the request
    return true;
}

// Middleware for CORS
bool corsMiddleware(HTTPRequest& request, HTTPResponse& response) {
    response.setHeader("Access-Control-Allow-Origin", "*");
    response.setHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS, PATCH");
    response.setHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");
    
    // If it's a preflight request, respond immediately
    if (request.getMethod() == "OPTIONS") {
        response.setStatusCode(204);
        return false; // Skip further processing
    }
    
    // Continue processing the request
    return true;
}

// Middleware for API key authentication
bool authMiddleware(HTTPRequest& request, HTTPResponse& response) {
    // Simple API key validation (in production, use a more secure approach)
    std::string apiKey = request.getHeader("X-API-Key");
    if (apiKey.empty()) {
        apiKey = request.getQueryParam("api_key");
    }
    
    // For this example, accept any non-empty key
    // In production, validate against stored keys
    if (apiKey.empty()) {
        response.setStatusCode(401);
        response.setStatusMessage("Unauthorized");
        response.setBody(R"({"error":"API key is required"})");
        return false; // Skip further processing
    }
    
    // Continue processing the request
    return true;
}

// Helper function to create a route handler for API endpoints
RouteHandler createAPIHandler(std::function<APIResponse(const APIRequest&)> handler) {
    return [handler](const HTTPRequest& request) -> HTTPResponse {
        // Convert HTTPRequest to APIRequest
        APIRequest apiRequest = request.toAPIRequest();
        
        try {
            // Call the API handler
            APIResponse apiResponse = handler(apiRequest);
            
            // Convert APIResponse to HTTPResponse
            return HTTPResponse::fromAPIResponse(apiResponse);
        } catch (const std::exception& e) {
            // Handle errors
            APIResponse errorResponse = APIResponse::error(500, e.what());
            return HTTPResponse::fromAPIResponse(errorResponse);
        }
    };
}

// Helper to parse and validate JSON body
nlohmann::json parseJSONBody(const HTTPRequest& request, HTTPResponse& response) {
    try {
        return nlohmann::json::parse(request.getBody());
    } catch (const std::exception& e) {
        response.setStatusCode(400);
        response.setStatusMessage("Bad Request");
        response.setBody(R"({"error":"Invalid JSON in request body"})");
        return nullptr;
    }
}

// Initialize all API routes
void setupRoutes(WebServer& server) {
    APIFacade& api = APIFacade::getInstance();
    
    // Course endpoints
    server.get("/api/courses", createAPIHandler([&api](const APIRequest& req) {
        return api.getAllCourses();
    }));
    
    server.get("/api/courses/:id", createAPIHandler([&api](const APIRequest& req) {
        std::string courseId = req.getParam<std::string>("id").value_or("");
        return api.getCourse(courseId);
    }));
    
    server.post("/api/courses", createAPIHandler([&api](const APIRequest& req) {
        return api.createCourse(req.getData());
    }));
    
    server.put("/api/courses/:id", createAPIHandler([&api](const APIRequest& req) {
        std::string courseId = req.getParam<std::string>("id").value_or("");
        return api.updateCourse(courseId, req.getData());
    }));
    
    server.del("/api/courses/:id", createAPIHandler([&api](const APIRequest& req) {
        std::string courseId = req.getParam<std::string>("id").value_or("");
        return api.deleteCourse(courseId);
    }));
    
    // Assignment endpoints
    server.get("/api/courses/:courseId/assignments", createAPIHandler([&api](const APIRequest& req) {
        std::string courseId = req.getParam<std::string>("courseId").value_or("");
        return api.getAllAssignments(courseId);
    }));
    
    server.get("/api/courses/:courseId/assignments/:id", createAPIHandler([&api](const APIRequest& req) {
        std::string courseId = req.getParam<std::string>("courseId").value_or("");
        std::string assignmentId = req.getParam<std::string>("id").value_or("");
        return api.getAssignment(courseId, assignmentId);
    }));
    
    server.post("/api/courses/:courseId/assignments", createAPIHandler([&api](const APIRequest& req) {
        std::string courseId = req.getParam<std::string>("courseId").value_or("");
        return api.createAssignment(courseId, req.getData());
    }));
    
    server.put("/api/courses/:courseId/assignments/:id", createAPIHandler([&api](const APIRequest& req) {
        std::string courseId = req.getParam<std::string>("courseId").value_or("");
        std::string assignmentId = req.getParam<std::string>("id").value_or("");
        return api.updateAssignment(courseId, assignmentId, req.getData());
    }));
    
    server.del("/api/courses/:courseId/assignments/:id", createAPIHandler([&api](const APIRequest& req) {
        std::string courseId = req.getParam<std::string>("courseId").value_or("");
        std::string assignmentId = req.getParam<std::string>("id").value_or("");
        return api.deleteAssignment(courseId, assignmentId);
    }));
    
    // Assignment state management
    server.patch("/api/courses/:courseId/assignments/:id/state", createAPIHandler([&api](const APIRequest& req) {
        std::string courseId = req.getParam<std::string>("courseId").value_or("");
        std::string assignmentId = req.getParam<std::string>("id").value_or("");
        std::string newState = req.getParam<std::string>("state").value_or("");
        return api.updateAssignmentState(courseId, assignmentId, newState);
    }));
    
    server.post("/api/courses/:courseId/assignments/:id/complete", createAPIHandler([&api](const APIRequest& req) {
        std::string courseId = req.getParam<std::string>("courseId").value_or("");
        std::string assignmentId = req.getParam<std::string>("id").value_or("");
        int marks = req.getParam<int>("marks").value_or(0);
        return api.markAssignmentAsCompleted(courseId, assignmentId, marks);
    }));
    
    // Notification endpoints
    server.get("/api/notifications", createAPIHandler([&api](const APIRequest& req) {
        return api.getAllNotifications();
    }));
    
    server.post("/api/notifications", createAPIHandler([&api](const APIRequest& req) {
        return api.createNotification(req.getData());
    }));
    
    server.del("/api/notifications/:id", createAPIHandler([&api](const APIRequest& req) {
        std::string notificationId = req.getParam<std::string>("id").value_or("");
        return api.deleteNotification(notificationId);
    }));
    
    // Memento operations (save/restore)
    server.post("/api/states", createAPIHandler([&api](const APIRequest& req) {
        std::string description = req.getParam<std::string>("description").value_or("");
        return api.saveApplicationState(description);
    }));
    
    server.get("/api/states", createAPIHandler([&api](const APIRequest& req) {
        return api.getSavedStates();
    }));
    
    server.post("/api/states/:id/restore", createAPIHandler([&api](const APIRequest& req) {
        std::string stateId = req.getParam<std::string>("id").value_or("");
        return api.restoreState(stateId);
    }));
    
    // Grading system operations
    server.get("/api/grading-systems", createAPIHandler([&api](const APIRequest& req) {
        return api.getAvailableGradingSystems();
    }));
    
    server.post("/api/grading-systems/:name/activate", createAPIHandler([&api](const APIRequest& req) {
        std::string systemName = req.getParam<std::string>("name").value_or("");
        return api.setActiveGradingSystem(systemName);
    }));
    
    server.post("/api/courses/:courseId/assignments/:id/submit", createAPIHandler([&api](const APIRequest& req) {
        std::string courseId = req.getParam<std::string>("courseId").value_or("");
        std::string assignmentId = req.getParam<std::string>("id").value_or("");
        std::string submissionContent = req.getParam<std::string>("content").value_or("");
        return api.submitAssignmentToGradingSystem(courseId, assignmentId, submissionContent);
    }));
    
    // Observer pattern operations
    server.post("/api/courses/:courseId/assignments/:id/subscribe", createAPIHandler([&api](const APIRequest& req) {
        std::string courseId = req.getParam<std::string>("courseId").value_or("");
        std::string assignmentId = req.getParam<std::string>("id").value_or("");
        std::string observerType = req.getParam<std::string>("type").value_or("student");
        return api.subscribeToAssignment(courseId, assignmentId, observerType);
    }));
    
    // Template method pattern operations
    server.post("/api/courses/:courseId/assignments/:id/process", createAPIHandler([&api](const APIRequest& req) {
        std::string courseId = req.getParam<std::string>("courseId").value_or("");
        std::string assignmentId = req.getParam<std::string>("id").value_or("");
        std::string submissionContent = req.getParam<std::string>("content").value_or("");
        return api.processAssignment(courseId, assignmentId, submissionContent);
    }));
    
    // Decorator pattern operations
    server.post("/api/courses/:courseId/assignments/:id/decorate", createAPIHandler([&api](const APIRequest& req) {
        std::string courseId = req.getParam<std::string>("courseId").value_or("");
        std::string assignmentId = req.getParam<std::string>("id").value_or("");
        std::string decorationType = req.getParam<std::string>("type").value_or("");
        return api.decorateAssignment(courseId, assignmentId, decorationType, req.getData());
    }));
    
    // Composite pattern operations
    server.post("/api/courses/:courseId/composite-assignments", createAPIHandler([&api](const APIRequest& req) {
        std::string courseId = req.getParam<std::string>("courseId").value_or("");
        return api.createCompositeAssignment(courseId, req.getData());
    }));
    
    server.post("/api/courses/:courseId/assignments/:id/children", createAPIHandler([&api](const APIRequest& req) {
        std::string courseId = req.getParam<std::string>("courseId").value_or("");
        std::string parentId = req.getParam<std::string>("id").value_or("");
        return api.addChildAssignment(courseId, parentId, req.getData());
    }));
    
    // Documentation
    server.serveStatic("/api/docs", "docs/api");
    
    // Frontend static files
    server.serveStatic("/", "frontend/build");
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    int port = 8080;
    std::string host = "localhost";
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--port" && i + 1 < argc) {
            port = std::stoi(argv[++i]);
        } else if (arg == "--host" && i + 1 < argc) {
            host = argv[++i];
        }
    }
    
    // Set up signal handlers
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    try {
        // Create the web server
        server = WebServerFactory::createServer();
        
        // Register middleware
        server->registerMiddleware(logRequestMiddleware);
        server->registerMiddleware(corsMiddleware);
        server->registerMiddleware(authMiddleware);
        
        // Configure CORS
        server->configureCORS(
            {"*"},  // Allow all origins
            {"GET", "POST", "PUT", "DELETE", "OPTIONS", "PATCH"},
            {"Content-Type", "Authorization", "X-API-Key"}
        );
        
        // Set up API routes
        setupRoutes(*server);
        
        // Start the server
        std::cout << "Starting API server on " << host << ":" << port << std::endl;
        server->start(port, host);
        
        // Main loop (will be exited when shutdown flag is set)
        while (!shouldShutdown) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}