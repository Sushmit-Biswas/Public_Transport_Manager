#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "schedule.h"

// ANSI color codes
#define RESET   "\x1B[0m"
#define RED     "\x1B[31m"
#define GREEN   "\x1B[32m"
#define YELLOW  "\x1B[33m"
#define BLUE    "\x1B[34m"
#define MAGENTA "\x1B[35m"
#define CYAN    "\x1B[36m"
#define WHITE   "\x1B[37m"
#define BOLD    "\x1B[1m"

#define MAX_SCHEDULES 100

// Using Schedule struct defined in schedule.h
struct Schedule schedules[MAX_SCHEDULES];
int schedule_count = 0;

void save_schedules_to_file() {
    FILE* file = fopen("schedules.txt", "w");
    if (file == NULL) {
        printf("%s%sError opening file!%s\n", BOLD, RED, RESET);
        return;
    }

    for (int i = 0; i < schedule_count; i++) {
        if (schedules[i].is_active) {
            fprintf(file, "%d|%d|%d|%d|%d\n",
                schedules[i].id,
                schedules[i].route_id,
                schedules[i].departure_hour,
                schedules[i].departure_minute,
                schedules[i].is_active);
        }
    }
    fclose(file);
}

void load_schedules_from_file() {
    FILE* file = fopen("schedules.txt", "r");
    if (file == NULL) {
        return; // File doesn't exist yet
    }

    char line[256];
    schedule_count = 0;
    while (fgets(line, sizeof(line), file) && schedule_count < MAX_SCHEDULES) {
        sscanf(line, "%d|%d|%d|%d|%d",
            &schedules[schedule_count].id,
            &schedules[schedule_count].route_id,
            &schedules[schedule_count].departure_hour,
            &schedules[schedule_count].departure_minute,
            &schedules[schedule_count].is_active);
        schedule_count++;
    }
    fclose(file);
}

void add_schedule() {
    if (schedule_count >= MAX_SCHEDULES) {
        printf("%s%sMaximum schedule limit reached!%s\n", BOLD, RED, RESET);
        return;
    }

    struct Schedule new_schedule;
    new_schedule.id = schedule_count + 1;
    new_schedule.is_active = 1;

    printf("%s%sEnter route ID:%s ", BOLD, CYAN, RESET);
    if (scanf("%d", &new_schedule.route_id) != 1) {
        printf("%s%sInvalid input for route ID!%s\n", BOLD, RED, RESET);
        while (getchar() != '\n'); // Clear input buffer
        return;
    }
    getchar(); // Clear newline
    
    printf("%s%sEnter departure time (24-hour format)%s\n", BOLD, CYAN, RESET);
    printf("%s%sHour (0-23):%s ", BOLD, CYAN, RESET);
    if (scanf("%d", &new_schedule.departure_hour) != 1) {
        printf("%s%sInvalid input for hour!%s\n", BOLD, RED, RESET);
        while (getchar() != '\n'); // Clear input buffer
        return;
    }
    printf("%s%sMinute (0-59):%s ", BOLD, CYAN, RESET);
    if (scanf("%d", &new_schedule.departure_minute) != 1) {
        printf("%s%sInvalid input for minute!%s\n", BOLD, RED, RESET);
        while (getchar() != '\n'); // Clear input buffer
        return;
    }
    getchar(); // Clear newline

    if (new_schedule.departure_hour < 0 || new_schedule.departure_hour > 23 ||
        new_schedule.departure_minute < 0 || new_schedule.departure_minute > 59) {
        printf("%s%sInvalid time format!%s\n", BOLD, RED, RESET);
        return;
    }

    schedules[schedule_count++] = new_schedule;
    save_schedules_to_file();
    printf("%s%sSchedule added successfully!%s\n", BOLD, GREEN, RESET);
}

void delete_schedule() {
    int id;
    printf("%s%sEnter schedule ID to delete:%s ", BOLD, CYAN, RESET);
    if (scanf("%d", &id) != 1) {
        printf("%s%sInvalid input for schedule ID!%s\n", BOLD, RED, RESET);
        while (getchar() != '\n'); // Clear input buffer
        return;
    }
    getchar(); // Clear newline

    for (int i = 0; i < schedule_count; i++) {
        if (schedules[i].id == id && schedules[i].is_active) {
            schedules[i].is_active = 0;
            save_schedules_to_file();
            printf("%s%sSchedule deleted successfully!%s\n", BOLD, GREEN, RESET);
            return;
        }
    }
    printf("%s%sSchedule not found!%s\n", BOLD, RED, RESET);
}

void display_schedules() {
    // Load latest schedules from file
    load_schedules_from_file();
    
    printf("\n%s%sAll Schedules:%s\n", BOLD, YELLOW, RESET);
    printf("%s╔══════╦═══════════╦════════════════╦══════════╗%s\n", BOLD, RESET);
    printf("%s║ %-4s ║ %-9s ║ %-13s ║ %-8s ║%s\n",
           BOLD, "ID", "Route ID", "Departure Time", "Status", RESET);
    printf("%s╠══════╬═══════════╬════════════════╬══════════╣%s\n", BOLD, RESET);
    
    int found_schedules = 0;
    for (int i = 0; i < schedule_count; i++) {
        printf("║ %-4d ║ %-9d ║ %02d:%02d          ║ %-8s ║\n",
            schedules[i].id,
            schedules[i].route_id,
            schedules[i].departure_hour,
            schedules[i].departure_minute,
            schedules[i].is_active ? "Active" : "Inactive");
        found_schedules++;
    }

    if (found_schedules == 0) {
        printf("║ %s%-44s%s ║\n", BOLD, "No schedules available!", RESET);
    }
    printf("%s╚══════╩═══════════╩════════════════╩══════════╝%s\n\n", BOLD, RESET);
}