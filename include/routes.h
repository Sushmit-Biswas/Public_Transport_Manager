#ifndef ROUTES_H
#define ROUTES_H

#define MAX_ROUTE_NAME 50
#define MAX_VEHICLE_TYPE 20

// Route structure definition
struct Route {
    int id;
    char start_point[MAX_ROUTE_NAME];
    char end_point[MAX_ROUTE_NAME];
    char vehicle_type[MAX_VEHICLE_TYPE]; // "AC Bus", "Non-AC Bus", "AC Taxi", "Non-AC Taxi"
    int available_seats;  // For buses
    int available_vehicles; // For taxis
    float fare;
    int is_active;
    struct Route* next;
};

// Function declarations for route management
void add_route(int is_admin); // Only admins can add routes
void delete_route(int is_admin); // Only admins can delete routes
void update_fare(int is_admin); // Only admins can update fares
void display_routes(void); // Both admins and clients can view routes
void save_routes_to_file(void);
void load_routes_from_file(void);

// Global variable for linked list
extern struct Route* head;
extern int route_count;

#endif // ROUTES_H