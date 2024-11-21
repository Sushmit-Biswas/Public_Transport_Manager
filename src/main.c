#include <stdio.h>        // For input and output
#include <string.h>       // For string manipulation
#include <time.h>         // For time functions
#include "routes.h"       // For route functions
#include "schedule.h"     // For schedule functions
#include "auth.h"         // For authentication functions
#include "booking.h"      // For booking functions   

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
#define BRIGHT_YELLOW "\x1B[93m"
#define BRIGHT_BLUE "\x1B[94m"

// Global variable for current user
char* current_user = NULL;
int is_admin = 0;

// Function to display admin logs
void view_admin_logs() {
    FILE* logfile = fopen("admin_logs.txt", "r");
    if (logfile == NULL) {
        printf("%s%sError opening log file!%s\n", BOLD, RED, RESET);
        return;
    }

    printf("\n%s%sAdmin Activity Logs:%s\n", BOLD, YELLOW, RESET);
    printf("%s╔══════════════════════════╦══════════════════╦═══════════════════════════╗%s\n", BOLD, RESET);
    printf("%s║ %-24s ║ %-16s ║ %-25s ║%s\n", BOLD, "Timestamp", "Admin", "Action", RESET);
    printf("%s╠══════════════════════════╬══════════════════╬═══════════════════════════╣%s\n", BOLD, RESET);

    char line[256];
    int found = 0;
    while (fgets(line, sizeof(line), logfile)) {
        char timestamp[50], admin[50], action[100];
        // Parse the log line [timestamp] Admin: admin_name | Action: action_desc
        char *start = strchr(line, '[') + 1;
        char *end = strchr(line, ']');
        strncpy(timestamp, start, end - start);
        timestamp[end - start] = '\0';

        start = strstr(line, "Admin: ") + 7;
        end = strstr(line, " | Action: ");
        strncpy(admin, start, end - start);
        admin[end - start] = '\0';

        start = strstr(line, "Action: ") + 8;
        strcpy(action, start);
        action[strlen(action)-1] = '\0';  // Remove newline

        printf("║ %-24s ║ %-16s ║ %-25s ║\n", timestamp, admin, action);
        found = 1;
    }

    if (!found) {
        printf("║ %s%-69s%s ║\n", BOLD, "No admin logs found", RESET);
    }

    printf("%s╚══════════════════════════╩══════════════════╩═══════════════════════════╝%s\n", BOLD, RESET);
    fclose(logfile);
}

// Function to log admin actions
void log_admin_action(const char* username, const char* action) {
    FILE* logfile = fopen("admin_logs.txt", "a");
    if (logfile == NULL) {
        printf("%s%sError opening log file!%s\n", BOLD, RED, RESET);
        return;
    }

    time_t now;
    time(&now);
    struct tm *local_time = localtime(&now);
    char date[50];
    strftime(date, sizeof(date), "%a %b %d %H:%M:%S %Y", local_time);

    // Use the actual username instead of "Unknown" if available
    const char* admin_name = username && strlen(username) > 0 ? username : "Unknown";
    fprintf(logfile, "[%s] Admin: %s | Action: %s\n", date, admin_name, action);
    fclose(logfile);
}

// Function to print header
void print_header(const char* title) {
    printf("\n%s%s", BOLD, BLUE);
    printf("╔═════════════════════════════════════════════════════════════╗\n");
    printf("║     %-44s   ║\n", title);  
    printf("╚═════════════════════════════════════════════════════════════╝\n%s", RESET);
}

// Function to update vehicle capacity
void update_vehicle_capacity() {
    int route_id;
    printf("\n%s%sEnter route ID to update capacity:%s ", BOLD, CYAN, RESET);
    scanf("%d", &route_id);
    while(getchar() != '\n');

    load_routes_from_file();
    struct Route* current = head;
    int found = 0;

    while (current != NULL) {
        if (current->id == route_id) {
            found = 1;
            printf("\nCurrent details for Route %d:\n", route_id);
            printf("Vehicle Type: %s\n", current->vehicle_type);
            if (strstr(current->vehicle_type, "Bus")) {
                printf("Available Seats: %d\n", current->available_seats);
                printf("\nEnter new number of available seats: ");
                int new_seats;
                scanf("%d", &new_seats);
                while(getchar() != '\n');

                if (new_seats < 0) {
                    printf("%s%sInvalid number of seats!%s\n", BOLD, RED, RESET);
                    return;
                }
                current->available_seats = new_seats;
            } else {
                printf("Available Taxis: %d\n", current->available_vehicles);
                printf("\nEnter new number of available taxis: ");
                int new_taxis;
                scanf("%d", &new_taxis);
                while(getchar() != '\n');

                if (new_taxis < 0) {
                    printf("%s%sInvalid number of taxis!%s\n", BOLD, RED, RESET);
                    return;
                }
                current->available_vehicles = new_taxis;
            }
            break;
        }
        current = current->next;
    }

    if (!found) {
        printf("%s%sRoute not found!%s\n", BOLD, RED, RESET);
        return;
    }

    save_routes_to_file();
    printf("%s%sVehicle capacity updated successfully!%s\n", BOLD, GREEN, RESET);
}

// Function to display admin menu
void display_admin_menu() {
    print_header("Bengaluru Public Transport Management System (Admin) ");
    printf("\n%s%s[1]%s Add Route\n", BOLD, MAGENTA, RESET);
    printf("%s%s[2]%s Delete Route\n", BOLD, RED, RESET);
    printf("%s%s[3]%s Display Routes\n", BOLD, CYAN, RESET);
    printf("%s%s[4]%s Add Schedule\n", BOLD, GREEN, RESET);
    printf("%s%s[5]%s Delete Schedule\n", BOLD, RED, RESET);
    printf("%s%s[6]%s Display Schedules\n", BOLD, CYAN, RESET);
    printf("%s%s[7]%s Update Trip Fares\n", BOLD, YELLOW, RESET);
    printf("%s%s[8]%s Update Bus seats/taxis availability\n", BOLD, BRIGHT_BLUE, RESET);
    printf("%s%s[9]%s View All Bookings\n", BOLD, MAGENTA, RESET);
    printf("%s%s[10]%s Create New Admin Account\n", BOLD, CYAN, RESET);
    printf("%s%s[11]%s View Admin Logs\n", BOLD, YELLOW, RESET);
    printf("%s%s[0]%s Logout\n", BOLD, RED, RESET);
}

// Function to display client menu
void display_client_menu() {
    print_header("Bengaluru Public Transport Management System (Client)");
    printf("\n%s%s[1]%s Display All Routes\n", BOLD, CYAN, RESET);
    printf("%s%s[2]%s Filter Routes by Location\n", BOLD, YELLOW, RESET);
    printf("%s%s[3]%s Filter Routes by Vehicle Type\n", BOLD, BRIGHT_BLUE, RESET);
    printf("%s%s[4]%s Display Schedules\n", BOLD, CYAN, RESET);
    printf("%s%s[5]%s Book Trip\n", BOLD, GREEN, RESET);
    printf("%s%s[6]%s Cancel Trip\n", BOLD, RED, RESET);
    printf("%s%s[7]%s View My Bookings\n", BOLD, MAGENTA, RESET);
    printf("%s%s[0]%s Logout\n", BOLD, RED, RESET);
}

// Function to view all bookings
void view_all_bookings() {
    printf("\n%s%sAll Bookings:%s\n", BOLD, YELLOW, RESET);
    printf("%s╔═════╦══════════════════╦═══════╦══════════╦═══════╦═══════╦══════════╗%s\n", BOLD, RESET);
    printf("%s║ ID  ║ Client Name      ║ Route ║ Schedule ║ Seats ║ Taxis ║ Fare(₹)  ║%s\n", BOLD, RESET);
    printf("%s╠═════╬══════════════════╬═══════╬══════════╬═══════╬═══════╬══════════╣%s\n", BOLD, RESET);

    load_bookings_from_file();
    int found = 0;
    for (int i = 0; i < booking_count; i++) {
        if (bookings[i].is_active) {
            printf("║ %-3d ║ %-16s ║ %-5d ║ %-8d ║ %-5d ║ %-5d ║ ₹%-7.2f ║\n",
                bookings[i].id,
                bookings[i].username,
                bookings[i].route_id,
                bookings[i].schedule_id,
                bookings[i].num_seats,
                bookings[i].num_taxis,
                bookings[i].total_fare);
            found = 1;
        }
    }

    if (!found) {
        printf("║ %s%-73s%s ║\n", BOLD, "No bookings found", RESET);
    }

    printf("%s╚═════╩══════════════════╩═══════╩══════════╩═══════╩═══════╩══════════╝%s\n", BOLD, RESET);
    log_admin_action(current_user, "Viewed all bookings");
}

// Function to filter routes by location
void filter_routes_by_location() {
    char start[50], end[50];
    printf("\n%s%sEnter starting point (or press Enter to skip):%s ", BOLD, CYAN, RESET);
    getchar();
    fgets(start, sizeof(start), stdin);
    start[strcspn(start, "\n")] = 0;

    printf("%s%sEnter destination point (or press Enter to skip):%s ", BOLD, CYAN, RESET);
    fgets(end, sizeof(end), stdin);
    end[strcspn(end, "\n")] = 0;

    int found = 0;
    printf("\n%s%sFiltered Routes:%s\n", BOLD, YELLOW, RESET);
    printf("%s╔══════╦════════════════════╦════════════════════╦═══════════════╦══════════╦══════════╗%s\n", BOLD, RESET);
    printf("%s║ %-4s ║ %-18s ║ %-18s ║ %-13s ║ %-8s ║ %-8s ║%s\n", 
           BOLD, "ID", "From", "To", "Type", "Seats", "Fare", RESET);
    printf("%s╠══════╬════════════════════╬════════════════════╬═══════════════╬══════════╬══════════╣%s\n", BOLD, RESET);

    load_routes_from_file(); // Load routes from file
    struct Route* current = head; // Initialize current pointer to head of the list
    while (current != NULL) {       
        if ((strlen(start) == 0 || strcasecmp(current->start_point, start) == 0) &&
            (strlen(end) == 0 || strcasecmp(current->end_point, end) == 0)) {
            printf("║ %-4d ║ %-18s ║ %-18s ║ %-13s ║ %-8d ║ ₹%-7.2f ║\n",
                current->id,
                current->start_point,
                current->end_point,
                current->vehicle_type,
                current->available_seats,
                current->fare);
            found = 1;
        }
        current = current->next;
    }
    
    if (!found) {
        printf("║ %s%-85s%s║\n", BOLD, "No routes found matching the criteria", RESET);
    }

    printf("%s╚══════╩════════════════════╩════════════════════╩═══════════════╩══════════╩══════════╝%s\n", BOLD, RESET);
}

// Function to filter routes by vehicle type
void filter_routes_by_vehicle() {
    printf("\n%s%sFilter by:%s\n", BOLD, YELLOW, RESET);
    printf("%s1.%s AC Bus\n", BOLD, RESET);
    printf("%s2.%s Non-AC Bus\n", BOLD, RESET);
    printf("%s3.%s AC Taxi\n", BOLD, RESET);
    printf("%s4.%s Non-AC Taxi\n", BOLD, RESET);
    printf("%s%sEnter choice:%s ", BOLD, CYAN, RESET);
    
    int choice;
    scanf("%d", &choice);

    char* filter;
    switch(choice) {
        case 1: filter = "AC Bus"; break;
        case 2: filter = "Non-AC Bus"; break;
        case 3: filter = "AC Taxi"; break;
        case 4: filter = "Non-AC Taxi"; break;
        default: printf("%s%sInvalid choice!%s\n", BOLD, RED, RESET); return;
    }

    int found = 0;
    printf("\n%s%sFiltered Routes:%s\n", BOLD, YELLOW, RESET);
    printf("%s╔══════╦════════════════════╦════════════════════╦═══════════════╦══════════╦══════════╗%s\n", BOLD, RESET);
    printf("%s║ %-4s ║ %-18s ║ %-18s ║ %-13s ║ %-8s ║ %-8s ║%s\n",
           BOLD, "ID", "From", "To", "Type", "Seats", "Fare", RESET);
    printf("%s╠══════╬════════════════════╬════════════════════╬═══════════════╬══════════╬══════════╣%s\n", BOLD, RESET);

    load_routes_from_file(); // Load routes from file
    struct Route* current = head;
    while (current != NULL) {
        if (strcmp(current->vehicle_type, filter) == 0) {
            printf("║ %-4d ║ %-18s ║ %-18s ║ %-13s ║ %-8d ║ ₹%-7.2f ║\n",
                current->id,
                current->start_point,
                current->end_point,
                current->vehicle_type,
                current->available_seats,
                current->fare);
            found = 1;
        }
        current = current->next;
    }

    if (!found) {
        printf("║ %s%-85s%s ║\n", BOLD, "No routes found matching the criteria", RESET);
    }

    printf("%s╚══════╩════════════════════╩════════════════════╩═══════════════╩══════════╩══════════╝%s\n", BOLD, RESET);
}

// Main function
int main() {
    int user_type;
    int attempts = 0;
    while (attempts < 2) {
        user_type = authenticate();
        if (user_type == 1 || user_type == 2) {
            break;
        }
        attempts++;
        if (attempts < 2) {
            printf("%s%sAuthentication failed. You have 1 more attempt.%s\n", BOLD, RED, RESET);
        }
    }
    
    if (attempts == 2) {
        printf("%s%sAuthentication failed. Maximum attempts exceeded. Exiting...%s\n", BOLD, RED, RESET);
        return 1;
    }

    int choice;
    while (1) {
        if (user_type == 1) { // Admin
            display_admin_menu();
            printf("\n%s%sEnter your choice:%s ", BOLD, CYAN, RESET);
            if (scanf("%d", &choice) != 1) {
                printf("%s%sInvalid input! Please enter a number.%s\n", BOLD, RED, RESET);
                while (getchar() != '\n'); // Clear input buffer
                continue;
            }
            switch (choice) {
                case 1:
                    add_route(1);
                    log_admin_action(current_user, "Added new route");
                    break;
                case 2:
                    delete_route(1);
                    log_admin_action(current_user, "Deleted route");
                    break;
                case 3:
                    display_routes();
                    log_admin_action(current_user, "Viewed all routes");
                    break;
                case 4:
                    add_schedule(current_user);
                    log_admin_action(current_user, "Added new schedule");
                    break;
                case 5:
                    delete_schedule(current_user);
                    log_admin_action(current_user, "Deleted schedule");
                    break;
                case 6:
                    display_schedules();
                    log_admin_action(current_user, "Viewed all schedules");
                    break;
                case 7:
                    update_fare(1);
                    log_admin_action(current_user, "Updated fare");
                    break;
                case 8:
                    update_vehicle_capacity();
                    log_admin_action(current_user, "Updated vehicle capacity");
                    break;
                case 9:
                    view_all_bookings();
                    break;
                case 10:
                    create_admin(current_user);
                    log_admin_action(current_user, "Created new admin account");
                    break;
                case 11:
                    view_admin_logs();
                    log_admin_action(current_user, "Viewed admin logs");
                    break;
                case 0:
                    log_admin_action(current_user, "Logged out");
                    printf("%s%sLogging out... Goodbye! Hope to see you again soon!%s\n", BOLD, GREEN, RESET);
                    return 0;
                default:
                    printf("%s%sInvalid choice. Please try again.%s\n", BOLD, RED, RESET);
            }
        } else { // Client
            display_client_menu();
            printf("\n%s%sEnter your choice:%s ", BOLD, CYAN, RESET);
            if (scanf("%d", &choice) != 1) {
                printf("%s%sInvalid input! Please enter a number.%s\n", BOLD, RED, RESET);
                while (getchar() != '\n'); // Clear input buffer
                continue;
            }
            switch (choice) {
                case 1:
                    display_routes();
                    break;
                case 2:
                    filter_routes_by_location();
                    break;
                case 3:
                    filter_routes_by_vehicle();
                    break;
                case 4:
                    display_schedules();
                    break;
                case 5:
                    make_booking(current_user);
                    break;
                case 6:
                    cancel_booking(current_user);
                    break;
                case 7:
                    view_bookings(current_user);
                    break;
                case 0:
                    printf("%s%sLogging out... Goodbye! Hope to see you again soon!%s\n", BOLD, GREEN, RESET);
                    return 0;
                default:
                    printf("%s%sInvalid choice. Please try again.%s\n", BOLD, RED, RESET);
            }
        }
    }
    return 0;
}