#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <string>
#include <functional>
#include <memory>
#include <map>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <atomic>
#include <queue>

#include "APIFacade.h"

// Forward declarations
class HTTPRequest;
class HTTPResponse;

// HTTP Request class
class HTTPRequest {
private:
    std::string method;
    std::string path;
    std::string version;
    std::map<std::string, std::string> headers;
    std::string body;
    std::map<std::string, std::string> queryParams;
    std::map<std::string, std::string> pathParams;

public:
    HTTPRequest();
    HTTPRequest(const std::string& rawRequest);
    
    void parse(const std::string& rawRequest);
    
    // Getters
    std::string getMethod() const;
    std::string getPath() const;
    std::string getVersion() const;
    std::map<std::string, std::string> getHeaders() const;
    std::string getHeader(const std::string& name) const;
    std::string getBody() const;
    std::map<std::string, std::string> getQueryParams() const;
    std::string getQueryParam(const std::string& name) const;
    std::map<std::string, std::string> getPathParams() const;
    std::string getPathParam(const std::string& name) const;
    
    // Setters
    void setMethod(const std::string& method);
    void setPath(const std::string& path);
    void setVersion(const std::string& version);
    void setHeader(const std::string& name, const std::string& value);
    void setBody(const std::string& body);
    void setQueryParam(const std::string& name, const std::string& value);
    void setPathParam(const std::string& name, const std::string& value);
    
    // Convert to APIRequest
    APIRequest toAPIRequest() const;
};

// HTTP Response class
class HTTPResponse {
private:
    std::string version;
    int statusCode;
    std::string statusMessage;
    std::map<std::string, std::string> headers;
    std::string body;

public:
    HTTPResponse();
    HTTPResponse(const APIResponse& apiResponse);
    HTTPResponse(int statusCode, const std::string& statusMessage = "", const std::string& body = "");
    
    // Getters
    std::string getVersion() const;
    int getStatusCode() const;
    std::string getStatusMessage() const;
    std::map<std::string, std::string> getHeaders() const;
    std::string getHeader(const std::string& name) const;
    std::string getBody() const;
    
    // Setters
    void setVersion(const std::string& version);
    void setStatusCode(int statusCode);
    void setStatusMessage(const std::string& statusMessage);
    void setHeader(const std::string& name, const std::string& value);
    void setBody(const std::string& body);
    
    // Convert to raw HTTP response
    std::string toString() const;
    
    // Create from APIResponse
    static HTTPResponse fromAPIResponse(const APIResponse& apiResponse);
};

// HTTP Method enumeration
enum class HTTPMethod {
    GET,
    POST,
    PUT,
    DELETE,
    PATCH,
    OPTIONS,
    HEAD
};

// Middleware function type
using MiddlewareFunction = std::function<bool(HTTPRequest&, HTTPResponse&)>;

// Route handler function type
using RouteHandler = std::function<HTTPResponse(const HTTPRequest&)>;

// Route definition
struct Route {
    HTTPMethod method;
    std::string pathPattern;
    RouteHandler handler;
    std::vector<std::string> pathParamNames;
};

// Abstract WebServer interface
class WebServer {
public:
    virtual ~WebServer() = default;
    
    // Server lifecycle
    virtual void start(int port, const std::string& host = "localhost") = 0;
    virtual void stop() = 0;
    virtual bool isRunning() const = 0;
    
    // Route registration
    virtual void registerRoute(HTTPMethod method, const std::string& path, RouteHandler handler) = 0;
    
    // Middleware registration
    virtual void registerMiddleware(MiddlewareFunction middleware) = 0;
    
    // Helper methods for common HTTP methods
    void get(const std::string& path, RouteHandler handler);
    void post(const std::string& path, RouteHandler handler);
    void put(const std::string& path, RouteHandler handler);
    void del(const std::string& path, RouteHandler handler);
    void patch(const std::string& path, RouteHandler handler);
    void options(const std::string& path, RouteHandler handler);
    
    // Static file serving
    virtual void serveStatic(const std::string& urlPath, const std::string& filePath) = 0;
    
    // CORS configuration
    virtual void configureCORS(const std::vector<std::string>& allowedOrigins,
                              const std::vector<std::string>& allowedMethods,
                              const std::vector<std::string>& allowedHeaders) = 0;
};

// Simple in-memory web server implementation
class SimpleWebServer : public WebServer {
private:
    int port;
    std::string host;
    std::atomic<bool> running;
    std::thread serverThread;
    std::vector<Route> routes;
    std::vector<MiddlewareFunction> middlewares;
    std::map<std::string, std::string> staticMappings;
    
    // CORS configuration
    std::vector<std::string> corsAllowedOrigins;
    std::vector<std::string> corsAllowedMethods;
    std::vector<std::string> corsAllowedHeaders;
    
    // Connection handling
    std::queue<int> connectionQueue;
    std::mutex queueMutex;
    std::condition_variable queueCondition;
    std::vector<std::thread> workerThreads;
    
    // Private methods
    void serverLoop();
    void handleConnection(int clientSocket);
    bool matchRoute(const std::string& path, const std::string& pattern, 
                   std::map<std::string, std::string>& pathParams);
    Route* findRoute(const HTTPRequest& request);
    bool applyMiddlewares(HTTPRequest& request, HTTPResponse& response);
    HTTPResponse handleRequest(HTTPRequest& request);
    
public:
    SimpleWebServer();
    ~SimpleWebServer();
    
    // Server lifecycle
    void start(int port, const std::string& host = "localhost") override;
    void stop() override;
    bool isRunning() const override;
    
    // Route registration
    void registerRoute(HTTPMethod method, const std::string& path, RouteHandler handler) override;
    
    // Middleware registration
    void registerMiddleware(MiddlewareFunction middleware) override;
    
    // Static file serving
    void serveStatic(const std::string& urlPath, const std::string& filePath) override;
    
    // CORS configuration
    void configureCORS(const std::vector<std::string>& allowedOrigins,
                      const std::vector<std::string>& allowedMethods,
                      const std::vector<std::string>& allowedHeaders) override;
};

// Factory for creating web server instances
class WebServerFactory {
public:
    static std::unique_ptr<WebServer> createServer(const std::string& type = "simple");
};

#endif // WEB_SERVER_H