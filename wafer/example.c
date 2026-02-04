#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// Forward declarations
typedef struct Request Request;
typedef struct Response Response;

// String helper
char* str_concat(const char* a, const char* b) {
    char* result = malloc(strlen(a) + strlen(b) + 1);
    strcpy(result, a);
    strcat(result, b);
    return result;
}

char* int_to_str(int n) {
    char* result = malloc(32);
    sprintf(result, "%d", n);
    return result;
}

// Struct definitions
struct Request {
    char* uri;
    char* method;
    int fd;
};

struct Response {
    int fd;
    int headersSent;
};

// Function prototypes
Request* Request_new(char* u, char* m, int socket);
Response* Response_new(int socket);
int fib(int n);
void respond(int fd, char* text);
void sendHeaders(int fd);
void sendNotFound(int fd);
void handleHome(int fd);
void handleFib(int fd);
void handleHello(int fd);
void handleRequest(int fd, char* buffer);
void handleClient(int clientfd);

Request* Request_new(char* u, char* m, int socket) {
    Request* self = malloc(sizeof(Request));
    self->uri = u;
    self->method = m;
    self->fd = socket;
    return self;
}

Response* Response_new(int socket) {
    Response* self = malloc(sizeof(Response));
    self->fd = socket;
    self->headersSent = 0;
    return self;
}

int fib(int n) {
    return (((n <= 1)) ? (n) : ((fib((n - 1)) + fib((n - 2)))));
}

void respond(int fd, char* text) {
    send(fd, text, strlen(text), 0);
}

void sendHeaders(int fd) {
    send(fd, "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n", 44, 0);
}

void sendNotFound(int fd) {
    send(fd, "HTTP/1.0 404 Not Found\r\nContent-Type: text/html\r\n\r\n<h1>404</h1>", 63, 0);
}

void handleHome(int fd) {
    sendHeaders(fd);
    respond(fd, "<html><body><h1>WAFer-Rix</h1>");
    respond(fd, "<p>A web server written in Rix</p>");
    respond(fd, "<ul><li><a href='/fib'>Fibonacci</a></li>");
    respond(fd, "<li><a href='/hello'>Hello</a></li></ul>");
    respond(fd, "</body></html>");
}

void handleFib(int fd) {
    sendHeaders(fd);
    respond(fd, "<html><body><h1>Fibonacci</h1><pre>");
    int i = 1;
    while ((i <= 15)) {
        int result = fib(i);
        respond(fd, str_concat(str_concat(str_concat(str_concat("F(", int_to_str(i)), ") = "), int_to_str(result)), "\n"));
        i = (i + 1);
    }
    respond(fd, "</pre><a href='/'>Back</a></body></html>");
}

void handleHello(int fd) {
    sendHeaders(fd);
    respond(fd, "<html><body><h1>Hello World!</h1>");
    respond(fd, "<a href='/'>Back</a></body></html>");
}

void handleRequest(int fd, char* buffer) {
    printf("%s\n", "Request received");
    if ((strncmp(buffer, "GET / ", 6) == 0)) {
        handleHome(fd);
    } else {
        if ((strncmp(buffer, "GET /fib", 8) == 0)) {
            handleFib(fd);
        } else {
            if ((strncmp(buffer, "GET /hello", 10) == 0)) {
                handleHello(fd);
            } else {
                sendNotFound(fd);
            }
        }
    }
}

void handleClient(int clientfd) {
    char* buffer = malloc(8192);
    int nbytes = recv(clientfd, buffer, 8191, 0);
    if ((nbytes > 0)) {
        handleRequest(clientfd, buffer);
    }
}

int main(int argc, char** argv) {
    printf("%s\n", "WAFer-Rix Server");
    printf("%s\n", "Starting on port 4242");
    int listenfd = socket(2, 1, 0);
    if ((listenfd < 0)) {
        printf("%s\n", "Error: socket failed");
    }
    printf("%s\n", "Waiting for connections...");
    while (true) {
        int clientfd = accept(listenfd, 0, 0);
        if ((clientfd >= 0)) {
            handleClient(clientfd);
            close(clientfd);
        }
    }
    return 0;
}