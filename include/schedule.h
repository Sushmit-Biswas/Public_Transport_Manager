#ifndef SCHEDULE_H               // If SCHEDULE_H is not defined
#define SCHEDULE_H               // Define SCHEDULE_H macro

// Structure definition for Schedule
struct Schedule {
    int id;
    int route_id;
    int departure_hour;               // Departure hour
    int departure_minute;             // Departure minute
    int is_active;                    // 1 for active, 0 for inactive
};

// Function declarations for schedule management
void add_schedule();             // Add a new schedule
void delete_schedule();          // Delete a schedule
void display_schedules();        // Display schedules
void save_schedules_to_file();   // Save schedules to file
void load_schedules_from_file(); // Load schedules from file

#endif // SCHEDULE_H