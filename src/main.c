#include <stdio.h>
#include <string.h>
#include "routes.h"
#include "schedule.h"
#include "auth.h"
#include "booking.h"

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

// Global variable for current user
char* current_user = NULL;
int is_admin = 0;

void print_header(const char* title) {
    printf("\n%s%s", BOLD, BLUE);
    printf("╔═════════════════════════════════════════════════════════════╗\n");
    printf("║     %-44s   ║\n", title);  
    printf("╚═════════════════════════════════════════════════════════════╝\n%s", RESET);
}

void display_admin_menu() {
    print_header("Bengaluru Public Transport Management System (Admin) ");
    printf("\n%s%s[1]%s Add Route\n", BOLD, GREEN, RESET);
    printf("%s%s[2]%s Delete Route\n", BOLD, RED, RESET);
    printf("%s%s[3]%s Display Routes\n", BOLD, CYAN, RESET);
    printf("%s%s[4]%s Add Schedule\n", BOLD, GREEN, RESET);
    printf("%s%s[5]%s Delete Schedule\n", BOLD, RED, RESET);
    printf("%s%s[6]%s Display Schedules\n", BOLD, CYAN, RESET);
    printf("%s%s[7]%s Update Trip Fares\n", BOLD, YELLOW, RESET);
    printf("%s%s[8]%s Create New Admin Account\n", BOLD, MAGENTA, RESET);
    printf("%s%s[9]%s Logout\n", BOLD, RED, RESET);
}

void display_client_menu() {
    print_header("Bengaluru Public Transport Management System (Client)");
    printf("\n%s%s[1]%s Display All Routes\n", BOLD, CYAN, RESET);
    printf("%s%s[2]%s Filter Routes by Location\n", BOLD, YELLOW, RESET);
    printf("%s%s[3]%s Filter Routes by Vehicle Type\n", BOLD, YELLOW, RESET);
    printf("%s%s[4]%s Display Schedules\n", BOLD, CYAN, RESET);
    printf("%s%s[5]%s Book Trip\n", BOLD, GREEN, RESET);
    printf("%s%s[6]%s Cancel Trip\n", BOLD, RED, RESET);
    printf("%s%s[7]%s View My Bookings\n", BOLD, MAGENTA, RESET);
    printf("%s%s[8]%s Logout\n", BOLD, RED, RESET);
}

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
    struct Route* current = head;
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
        printf("║ %s%-85s%s ║\n", BOLD, "No routes found matching the criteria", RESET);
    }

    printf("%s╚══════╩════════════════════╩════════════════════╩═══════════════╩══════════╩══════════╝%s\n", BOLD, RESET);
}

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
                    break;
                case 2:
                    delete_route(1);
                    break;
                case 3:
                    display_routes();
                    break;
                case 4:
                    add_schedule(current_user);
                    break;
                case 5:
                    delete_schedule(current_user);
                    break;
                case 6:
                    display_schedules();
                    break;
                case 7:
                    update_fare(1);
                    break;
                case 8:
                    create_admin(current_user);
                    break;
                case 9:
                    printf("%s%sLogging out... Goodbye!%s\n", BOLD, GREEN, RESET);
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
                case 8:
                    printf("%s%sLogging out... Goodbye!%s\n", BOLD, GREEN, RESET);
                    return 0;
                default:
                    printf("%s%sInvalid choice. Please try again.%s\n", BOLD, RED, RESET);
            }
        }
    }
    return 0;
}