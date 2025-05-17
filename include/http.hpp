#ifndef HTTP_HPP
#define HTTP_HPP

#include <string>
#include <unordered_map>
using namespace std;

struct HttpRequest{
    string method;    
    string path;    
    string version;    
    unordered_map<string,string> headers;
    string body;    
};

HttpRequest parse_http_request(const string& raw_request);

#endif