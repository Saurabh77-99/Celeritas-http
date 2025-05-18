#pragma once

#include "http.hpp"
#include <functional>
#include <string>
#include <unordered_map>
using namespace std;

using HandlerFunc = function<string(const HttpRequest&)>;

class Router{
    public:
        void add_route(const string& method, const string& path, HandlerFunc handler);
        string route(const HttpRequest& request) const;

    private:
        unordered_map<string, unordered_map<string, HandlerFunc>> routes;
};