#include <stdio.h>        // For input and output
#include <stdlib.h>       // For exit function  
#include <string.h>       // For string manipulation
#include "schedule.h"     // Includes the schedule structure and function prototype

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

#define MAX_SCHEDULES 300   // Max no of schedules the system can store 

// Using Schedule struct defined in schedule.h
struct Schedule schedules[MAX_SCHEDULES];   // Schedule[ MAX_SCHEDULES] indicates an array of schedules with elements defined in the schedule.h
int schedule_count = 0;                     // Counter for total schedules currently in memory

// Function to save schedules array to file
void save_schedules_to_file() {
    FILE* file = fopen("schedules.txt", "w");                   // Opens the schedules.txt file in writing mode 
    if (file == NULL) {
        printf("%s%sError opening file!%s\n", BOLD, RED, RESET);// Handles the case where the file can't be opened and exits the function if cannot be opened.
        return;
    }
// Defined a for loop to add the details of the schedule in the file only actilve schedules 
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
    fclose(file);   // Closes the file.
}

// Function to load schedules from file
void load_schedules_from_file() {
    FILE* file = fopen("schedules.txt", "r");   // Opens the schedule.txt in read mode 
    if (file == NULL) {
        return; // File doesn't exist yet
    }

    char line[256];
    schedule_count = 0;
    // Written while loop for reading each line from the file and ensuring number of schedules no exceeding and fgets reads the line from file and stores it in the line buffer 
    while (fgets(line, sizeof(line), file) && schedule_count < MAX_SCHEDULES) {
        sscanf(line, "%d|%d|%d|%d|%d",
            &schedules[schedule_count].id,
            &schedules[schedule_count].route_id,
            &schedules[schedule_count].departure_hour,
            &schedules[schedule_count].departure_minute,
            &schedules[schedule_count].is_active);
        schedule_count++;
	// sscanf: it extracts the data and it is stored in the schedules array at the position indicated by schedule_count
    }
    fclose(file);   // Closes the file

}

// Function to add a new schedule
void add_schedule() {
    if (schedule_count >= MAX_SCHEDULES) {
        printf("%s%sMaximum schedule limit reached!%s\n", BOLD, RED, RESET);
        return;
    }   // Checks if the max scedules reached 
// Created a new schedule to hold the input data
    struct Schedule new_schedule;
    new_schedule.id = schedule_count + 1;   // Assigns the scedule ID
    new_schedule.is_active = 1;            // Marks the new schedule as active
// Prompt the user to enter the route ID 
    printf("%s%sEnter route ID:%s ", BOLD, CYAN, RESET);
    if (scanf("%d", &new_schedule.route_id) != 1) {// Validates the input of route ID if route ID is invalid it displays the error message 
        printf("%s%sInvalid input for route ID!%s\n", BOLD, RED, RESET);
        while (getchar() != '\n'); // Clear input buffer
        return;
    }
    getchar(); // Clear newline
// Prompt the user to enter the departure time in 24 hr format
    printf("%s%sEnter departure time (24-hour format)%s\n", BOLD, CYAN, RESET);
    printf("%s%sHour (0-23):%s ", BOLD, CYAN, RESET);
    if (scanf("%d", &new_schedule.departure_hour) != 1) {   // Validates the input for hour and displays an error message 
        printf("%s%sInvalid input for hour!%s\n", BOLD, RED, RESET);
        while (getchar() != '\n'); // Clears the input buffer
        return;
    }
    printf("%s%sMinute (0-59):%s ", BOLD, CYAN, RESET);
    if (scanf("%d", &new_schedule.departure_minute) != 1) {// Validates the input for minutes
        printf("%s%sInvalid input for minute!%s\n", BOLD, RED, RESET);
        while (getchar() != '\n'); // Clears the input buffer
        return;
    }
    getchar(); // Clears the newline

    if (new_schedule.departure_hour < 0 || new_schedule.departure_hour > 23 ||
        new_schedule.departure_minute < 0 || new_schedule.departure_minute > 59) {  // If any of invalid cases found to be true, displays the invalid format 
        printf("%s%sInvalid time format!%s\n", BOLD, RED, RESET);
        return;
    }
    // Adds the schedule to the global array and increments the schedule count 
    schedules[schedule_count++] = new_schedule;
    save_schedules_to_file();// Calls the save schedule to file function to store the new schedule 
    printf("%s%sSchedule added successfully!%s\n", BOLD, GREEN, RESET);
}

// Function to delete a schedule
void delete_schedule() {
    int id;   // Variable to store the id of scedule to be deleted
    printf("%s%sEnter schedule ID to delete:%s ", BOLD, CYAN, RESET);
    if (scanf("%d", &id) != 1) {    // Checks if the id is valid or not 
        printf("%s%sInvalid input for schedule ID!%s\n", BOLD, RED, RESET);
        while (getchar() != '\n');  // Clears the input buffer
        return;
    }
    getchar(); // Clear newline

    for (int i = 0; i < schedule_count; i++) {
        if (schedules[i].id == id && schedules[i].is_active) {  // Checks the schedule is acive or not if active makes it de active or exits the loop and prints the error message 
            schedules[i].is_active = 0;
            save_schedules_to_file();   // Function call to save the data 
            printf("%s%sSchedule deleted successfully!%s\n", BOLD, GREEN, RESET);
            return;
        }
    }
    printf("%s%sSchedule not found!%s\n", BOLD, RED, RESET);
}

// Function to display schedules
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
            schedules[i].is_active ? "Active" : "Inactive");    // Checks the status of schedule if it is active prints active else in active
        found_schedules++;
    }

    if (found_schedules == 0) {
        printf("║ %s%-44s%s ║\n", BOLD, "No schedules available!", RESET);
    }
    printf("%s╚══════╩═══════════╩════════════════╩══════════╝%s\n\n", BOLD, RESET);
}
