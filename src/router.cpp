#include "router.hpp"
using namespace std;

void Router::add_route(const string& method, const string& path, HandlerFunc handler) {
    routes[method][path] = handler;
}

string Router::route(const HttpRequest& request) const {
    auto method_it = routes.find(request.method);
    if (method_it != routes.end()) {
        auto path_it = method_it->second.find(request.path);
        if (path_it != method_it->second.end()) {
            return path_it->second(request);
        }
    }
    return "404 Not Found!";
}