#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

// Forward declarations
typedef struct Rectangle Rectangle;
typedef struct NamedRectangle NamedRectangle;

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
struct Rectangle {
    int w;
    int h;
};

struct NamedRectangle {
    Rectangle base;
    char* name;
};

// Function prototypes
Rectangle* Rectangle_new(int w, int h);
int Rectangle_area(Rectangle* self);
int Rectangle_perimeter(Rectangle* self);
NamedRectangle* NamedRectangle_new(int w, int h, char* name);
char* NamedRectangle_nameArea(NamedRectangle* self);
int fib(int n);

Rectangle* Rectangle_new(int w, int h) {
    Rectangle* self = malloc(sizeof(Rectangle));
    self->w = w;
    self->h = h;
    return self;
}

int Rectangle_area(Rectangle* self) {
    return (self->w * self->h);
}

int Rectangle_perimeter(Rectangle* self) {
    return ((self->w + self->h) * 2);
}

NamedRectangle* NamedRectangle_new(int w, int h, char* name) {
    NamedRectangle* self = malloc(sizeof(NamedRectangle));
    self->base.w = w;
    self->base.h = h;
    self->name = name;
    return self;
}

char* NamedRectangle_nameArea(NamedRectangle* self) {
    return str_concat(str_concat(str_concat("NamedRectangle ", self->name), " has an area of "), int_to_str(Rectangle_area(&self->base)));
}

int fib(int n) {
    return (((n <= 1)) ? (n) : ((fib((n - 1)) + fib((n - 2)))));
}

int main(int argc, char** argv) {
    Rectangle* r = Rectangle_new(5, 10);
    NamedRectangle* s = NamedRectangle_new(3, 4, "Great");
    printf("%s\n", s->name);
    printf("%s\n", str_concat(str_concat(int_to_str(Rectangle_area(r)), " "), int_to_str(Rectangle_perimeter(r))));
    printf("%s\n", str_concat(str_concat(s->name, " "), int_to_str(Rectangle_area(&s->base))));
    printf("%s\n", NamedRectangle_nameArea(s));
    int y = 5;
    y = (y + 2);
    if ((argc > 1)) {
        int N = atoi(argv[1]);
        for (int i = 1; i < (N + 1); i++) {
            printf("%s\n", str_concat(str_concat(int_to_str(i), ": "), int_to_str(fib(i))));
        }
    } else {
        printf("%s\n", str_concat(str_concat("Usage ", argv[0]), " N"));
    }
    return 0;
}