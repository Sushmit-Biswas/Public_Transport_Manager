#ifndef SCHEDULE_H
#define SCHEDULE_H

// Structure definition for Schedule
struct Schedule {
    int id;
    int route_id;
    int departure_hour;
    int departure_minute;
    int is_active;
};

// Function declarations for schedule management
void add_schedule();
void delete_schedule();
void display_schedules();
void save_schedules_to_file();
void load_schedules_from_file();

#endif // SCHEDULE_H