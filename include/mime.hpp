#pragma once
#include <string>

inline bool ends_with(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

inline std::string get_mime_type(const std::string& filename) {
    if (ends_with(filename, ".html")) return "text/html";
    if (ends_with(filename, ".css")) return "text/css";
    if (ends_with(filename, ".js")) return "application/javascript";
    if (ends_with(filename, ".png")) return "image/png";
    if (ends_with(filename, ".jpg") || ends_with(filename, ".jpeg")) return "image/jpeg";
    if (ends_with(filename, ".gif")) return "image/gif";
    if (ends_with(filename, ".svg")) return "image/svg+xml";
    if (ends_with(filename, ".json")) return "application/json";
    if (ends_with(filename, ".txt")) return "text/plain";
    return "application/octet-stream";
}
