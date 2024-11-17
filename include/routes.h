#ifndef ROUTES_H
#define ROUTES_H

#define MAX_ROUTE_NAME 50

// Route structure definition
struct Route {
    int id;
    char name[MAX_ROUTE_NAME];
    char start_point[MAX_ROUTE_NAME];
    char end_point[MAX_ROUTE_NAME];
    int is_active;
    struct Route* next;
};

// Function declarations for route management
void add_route(void);
void delete_route(void);
void display_routes(void);
void save_routes_to_file(void);
void load_routes_from_file(void);

#endif // ROUTES_H