#include "http.hpp"
#include <sstream>
#include <algorithm>
#include <iostream>
using namespace std;

HttpRequest parse_http_request(const string& raw_request){
    istringstream stream(raw_request);
    HttpRequest req;
    string line;

    if(getline(stream, line)) {
        istringstream line_stream(line);
        line_stream >> req.method >> req.path >> req.version;
    }

    while (getline(stream, line) && line != "\r") {
        size_t colon = line.find(':');
        if (colon != string::npos) {
            string key = line.substr(0, colon);
            string value = line.substr(colon + 1);

            value.erase(remove(value.begin(), value.end(), '\r'), value.end());
            key.erase(remove(key.begin(), key.end(), '\r'), key.end());
            key.erase(0, key.find_first_not_of(" "));
            key.erase(key.find_last_not_of(" ") + 1);
            value.erase(0, value.find_first_not_of(" "));
            value.erase(value.find_last_not_of(" ") + 1);

            req.headers[key] = value;
        }
    }

    string body;
    while (getline(stream, line)) {
        body += line + "\n";
    }
    if (!body.empty()) {
        body.erase(body.find_last_not_of("\n") + 1);
        req.body = body;
    }

    return req;
}