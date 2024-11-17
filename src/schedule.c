#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "schedule.h"

#define MAX_SCHEDULES 100

// Using Schedule struct defined in schedule.h
struct Schedule schedules[MAX_SCHEDULES];
int schedule_count = 0;

void save_schedules_to_file() {
    FILE* file = fopen("schedules.txt", "w");
    if (file == NULL) {
        printf("Error opening file!\n");
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
        printf("Maximum schedule limit reached!\n");
        return;
    }

    struct Schedule new_schedule;
    new_schedule.id = schedule_count + 1;
    new_schedule.is_active = 1;

    printf("Enter route ID: ");
    scanf("%d", &new_schedule.route_id);
    
    printf("Enter departure time (24-hour format)\n");
    printf("Hour (0-23): ");
    scanf("%d", &new_schedule.departure_hour);
    printf("Minute (0-59): ");
    scanf("%d", &new_schedule.departure_minute);

    if (new_schedule.departure_hour < 0 || new_schedule.departure_hour > 23 ||
        new_schedule.departure_minute < 0 || new_schedule.departure_minute > 59) {
        printf("Invalid time format!\n");
        return;
    }

    schedules[schedule_count++] = new_schedule;
    save_schedules_to_file();
    printf("Schedule added successfully!\n");
}

void delete_schedule() {
    int id;
    printf("Enter schedule ID to delete: ");
    scanf("%d", &id);

    for (int i = 0; i < schedule_count; i++) {
        if (schedules[i].id == id && schedules[i].is_active) {
            schedules[i].is_active = 0;
            save_schedules_to_file();
            printf("Schedule deleted successfully!\n");
            return;
        }
    }
    printf("Schedule not found!\n");
}

void display_schedules() {
    printf("\nSchedule List:\n");
    printf("ID\tRoute ID\tDeparture Time\tStatus\n");
    printf("----------------------------------------\n");
    
    for (int i = 0; i < schedule_count; i++) {
        if (schedules[i].is_active) {
            printf("%d\t%d\t\t%02d:%02d\t\tActive\n", 
                schedules[i].id,
                schedules[i].route_id,
                schedules[i].departure_hour,
                schedules[i].departure_minute);
        }
    }
}