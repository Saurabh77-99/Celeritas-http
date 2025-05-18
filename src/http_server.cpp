#include <fstream>
#include "http.hpp" 
#include "router.hpp"
#include "mime.hpp"
#include "http_server.hpp"
#include <iostream>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <sstream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <functional>
using namespace std;

Router router; 

class ThreadPool {
private:
    vector<thread> workers;
    queue<int> tasks; // client_fds
    mutex queue_mutex;
    condition_variable condition;
    bool stop = false;

public:
    ThreadPool(size_t);
    ~ThreadPool();
    void enqueue(int client_fd);
};

void handle_client(int client_fd);
ThreadPool::ThreadPool(size_t threads) {
    for (size_t i = 0; i < threads; ++i) {
        workers.emplace_back([this]() {
            while (true) {
                int client_fd;
                {
                    unique_lock<mutex> lock(this->queue_mutex);
                    this->condition.wait(lock, [this] { return this->stop || !this->tasks.empty(); });
                    if (this->stop && this->tasks.empty())
                        return;
                    client_fd = this->tasks.front();
                    this->tasks.pop();
                }
                handle_client(client_fd);
            }
        });
    }
}

ThreadPool::~ThreadPool() {
    {
        unique_lock<mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for (thread &worker : workers)
        worker.join();
}

void ThreadPool::enqueue(int client_fd) {
    {
        unique_lock<mutex> lock(queue_mutex);
        tasks.push(client_fd);
    }
    condition.notify_one();
}

void handle_client(int client_fd){
    char buffer[4096] = {0};
    int bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
    if (bytes_received < 0){
        perror("recv failed");
        close(client_fd);
        return;
    }

    string raw_request(buffer, bytes_received);
    HttpRequest req = parse_http_request(raw_request);

    cout << "[Request] " << req.method << " " << req.path <<endl;

    string response_body;
    int status_code = 200;
    string status_text = "OK";
    string content_type = "text/html"; 

    if (req.method == "GET" && req.path.rfind("/static/", 0) == 0) {
        // string file_path = "static/" + req.path.substr(8);
        string file_path = "/mnt/c/Users/HP/OneDrive/Desktop/hft-http-server/static/" + req.path.substr(8);
        cout << "Serving file: " << file_path << endl;

        ifstream file(file_path, ios::binary);
        if (file.good()) {
            ostringstream ss;
            ss << file.rdbuf();
            response_body = ss.str();
            content_type = get_mime_type(file_path);
        } else {
            response_body = "404 Not Found (static file)";
            status_code = 404;
            status_text = "Not Found";
            cerr << "❌ File not found: " << file_path << endl;
        }
    } else {
        response_body = router.route(req);
        if (response_body == "404 Not Found!") {
            status_code = 404;
            status_text = "Not Found";
        }
    }

    ostringstream response;
    response << "HTTP/1.1 " << status_code << " " << status_text << "\r\n"
             << "Content-Length: " << response_body.length() << "\r\n"
             << "Content-Type: " << content_type << "\r\n\r\n"
             << response_body;

    send(client_fd, response.str().c_str(), response.str().length(), 0);
    close(client_fd);
}    

void start_server(int port){
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1){
        perror("socket failed");
        return;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(server_fd, (sockaddr *)&addr, sizeof(addr)) < 0){
        perror("bind failed");
        close(server_fd);
        return;
    }

    if (listen(server_fd, 10) < 0){
        perror("listen failed");
        close(server_fd);
        return;
    }

    cout << "Server listening on port " << port << endl;

    ThreadPool pool(4);  // create thread pool with 4 workers

    router.add_route("GET", "/hello", [](const HttpRequest& req) {
        return "Hello Route!";
    });

    router.add_route("GET", "/status", [](const HttpRequest& req) {
        return "Server is up!";
    });

    router.add_route("POST", "/echo", [](const HttpRequest& req) {
        return "Echo from POST: " + req.body;
    });

    while (true){
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (sockaddr *)&client_addr, &client_len);
        if (client_fd < 0){
            perror("accept failed");
            continue;
        }
        pool.enqueue(client_fd);
    }
    close(server_fd);
}
