#ifndef ROUTES_H               // If ROUTES_H is not defined
#define ROUTES_H               // Define ROUTES_H macro

#define MAX_ROUTE_NAME 50       // Define maximum route name length
#define MAX_VEHICLE_TYPE 20     // Define maximum vehicle type length

// Route structure definition
struct Route {
    int id;                                     // Route ID
    char start_point[MAX_ROUTE_NAME];           // Start point
    char end_point[MAX_ROUTE_NAME];             // End point
    char vehicle_type[MAX_VEHICLE_TYPE];        // "AC Bus", "Non-AC Bus", "AC Taxi", "Non-AC Taxi"
    int available_seats;                        // Available seats (for buses)
    int available_vehicles;                     // Available vehicles (for taxis)
    float fare;                                 // Fare
    int is_active;                              // 1 for active, 0 for inactive    
    struct Route* next;                         // Pointer to the next route in the linked list
};

// Function declarations for route management
void add_route(int is_admin);       // Only admins can add routes
void delete_route(int is_admin);    // Only admins can delete routes
void update_fare(int is_admin);     // Only admins can update fares
void display_routes(void);          // Both admins and clients can view routes
void save_routes_to_file(void);     // Save routes to file
void load_routes_from_file(void);   // Load routes from file

// Global variable for linked list
extern struct Route* head; // Pointer to the head of the linked list
extern int route_count;

#endif // ROUTES_H