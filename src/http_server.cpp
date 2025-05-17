#include "http.hpp" 
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

    string request(buffer);
    istringstream request_stream(request);
    string method, path;
    request_stream >> method >> path;

    string response_body;

    if (method == "GET" && path == "/hello") {
        response_body = "Hello Route!";
    } else if (method == "GET" && path == "/status") {
        response_body = "Server is up!";
    } else {
        response_body = "Not Found!";
    }

    ostringstream response;
    response << "HTTP/1.1 200 OK\r\n"
             << "Content-Length: " << response_body.length() << "\r\n"
             << "Content-Type: text/plain\r\n\r\n"
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
