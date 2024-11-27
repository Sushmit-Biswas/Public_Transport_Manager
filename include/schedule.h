#ifndef SCHEDULE_H               // If SCHEDULE_H is not defined
#define SCHEDULE_H               // Define SCHEDULE_H macro

// Structure definition for Schedule
struct Schedule {
    int id;                           // Schedule ID
    int route_id;                     // Route ID
    int departure_hour;               // Departure hour
    int departure_minute;             // Departure minute
    int is_active;                    // 1 for active, 0 for inactive
};

// Function declarations for schedule management
void add_schedule();             // Function call to add a new schedule defined in schedule.c   
void delete_schedule();          // Function call to delete a schedule defined in schedule.c
void display_schedules();        // Function call to display schedules defined in schedule.c
void save_schedules_to_file();   // Function call to save schedules to file defined in schedule.c
void load_schedules_from_file(); // Function call to load schedules from file defined in schedule.c 

#endif // SCHEDULE_H