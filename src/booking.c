#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "routes.h" // Add routes header for Route struct and head
#include "schedule.h" // Add schedule header for display_schedules()
#include "booking.h" // Add booking header for struct definition

// Global variables defined in header
struct Booking bookings[MAX_BOOKINGS];
int booking_count = 0;

void save_bookings_to_file() {
    FILE* file = fopen("bookings.txt", "w");
    if (file == NULL) {
        printf("Error opening bookings file!\n");
        return;
    }

    for (int i = 0; i < booking_count; i++) {
        if (bookings[i].is_active) {
            fprintf(file, "%d|%s|%d|%d|%d|%d|%d|%.2f\n",
                bookings[i].id,
                bookings[i].username,
                bookings[i].route_id,
                bookings[i].schedule_id,
                bookings[i].num_seats,
                bookings[i].num_taxis,
                bookings[i].is_active,
                bookings[i].total_fare);
        }
    }
    fclose(file);
}

void load_bookings_from_file() {
    FILE* file = fopen("bookings.txt", "r");
    if (file == NULL) {
        return; // File doesn't exist yet
    }

    char line[256];
    booking_count = 0;
    while (fgets(line, sizeof(line), file) && booking_count < MAX_BOOKINGS) {
        sscanf(line, "%d|%[^|]|%d|%d|%d|%d|%d|%f",
            &bookings[booking_count].id,
            bookings[booking_count].username,
            &bookings[booking_count].route_id,
            &bookings[booking_count].schedule_id,
            &bookings[booking_count].num_seats,
            &bookings[booking_count].num_taxis,
            &bookings[booking_count].is_active,
            &bookings[booking_count].total_fare);
        booking_count++;
    }
    fclose(file);
}

void make_booking(char* username) {
    if (username == NULL || strlen(username) == 0) {
        printf("Error: Invalid username\n");
        return;
    }

    // Load latest bookings and routes
    load_bookings_from_file();
    load_routes_from_file();

    if (booking_count >= MAX_BOOKINGS) {
        printf("Maximum booking limit reached!\n");
        return;
    }

    // Check if routes are loaded
    if (head == NULL) {
        printf("No routes available!\n");
        return;
    }

    struct Booking new_booking;
    new_booking.id = booking_count + 1;
    strncpy(new_booking.username, username, MAX_USERNAME - 1);
    new_booking.username[MAX_USERNAME - 1] = '\0';
    new_booking.is_active = 1;
    new_booking.num_seats = 0;
    new_booking.num_taxis = 0;
    new_booking.total_fare = 0;

    // Display available routes and schedules
    display_routes();
    display_schedules();

    printf("\nEnter route ID: ");
    if (scanf("%d", &new_booking.route_id) != 1) {
        printf("Invalid route ID!\n");
        while (getchar() != '\n'); // Clear input buffer
        return;
    }
    while (getchar() != '\n'); // Clear newline

    printf("Enter schedule ID: ");
    if (scanf("%d", &new_booking.schedule_id) != 1) {
        printf("Invalid schedule ID!\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    // Find the route
    struct Route* current = head;
    int route_found = 0;
    while (current != NULL) {
        if (current->id == new_booking.route_id && current->is_active) {
            route_found = 1;
            if (strstr(current->vehicle_type, "Bus") != NULL) {
                printf("Enter number of seats to book (max %d): ", current->available_seats);
                if (scanf("%d", &new_booking.num_seats) != 1) {
                    printf("Invalid number of seats!\n");
                    while (getchar() != '\n');
                    return;
                }
                while (getchar() != '\n');

                if (new_booking.num_seats <= 0 || new_booking.num_seats > current->available_seats) {
                    printf("Invalid number of seats!\n");
                    return;
                }
                current->available_seats -= new_booking.num_seats;
                new_booking.total_fare = current->fare * new_booking.num_seats;
            } else {
                printf("Enter number of taxis to book (max %d): ", current->available_vehicles);
                if (scanf("%d", &new_booking.num_taxis) != 1) {
                    printf("Invalid number of taxis!\n");
                    while (getchar() != '\n');
                    return;
                }
                while (getchar() != '\n');

                if (new_booking.num_taxis <= 0 || new_booking.num_taxis > current->available_vehicles) {
                    printf("Invalid number of taxis!\n");
                    return;
                }
                current->available_vehicles -= new_booking.num_taxis;
                new_booking.total_fare = current->fare * new_booking.num_taxis;
            }

            bookings[booking_count++] = new_booking;
            save_bookings_to_file();
            save_routes_to_file();
            printf("Booking successful! Total fare: ₹%.2f\n", new_booking.total_fare);
            return;
        }
        current = current->next;
    }
    
    if (!route_found) {
        printf("Route not found or inactive!\n");
    }
}

void cancel_booking(char* username) {
    if (username == NULL || strlen(username) == 0) {
        printf("Error: Invalid username\n");
        return;
    }

    // Load latest data
    load_bookings_from_file();
    load_routes_from_file();

    int booking_id;
    int found = 0;

    printf("\nYour current bookings:\n");
    printf("ID\tRoute\tSchedule\tSeats\tTaxis\tFare(₹)\n");
    printf("------------------------------------------------\n");

    for (int i = 0; i < booking_count; i++) {
        if (bookings[i].is_active && strcmp(bookings[i].username, username) == 0) {
            printf("%d\t%d\t%d\t\t%d\t%d\t%.2f\n",
                bookings[i].id,
                bookings[i].route_id,
                bookings[i].schedule_id,
                bookings[i].num_seats,
                bookings[i].num_taxis,
                bookings[i].total_fare);
            found = 1;
        }
    }

    if (!found) {
        printf("No active bookings found!\n");
        return;
    }

    printf("\nEnter booking ID to cancel: ");
    if (scanf("%d", &booking_id) != 1) {
        printf("Invalid booking ID!\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    for (int i = 0; i < booking_count; i++) {
        if (bookings[i].id == booking_id && bookings[i].is_active && 
            strcmp(bookings[i].username, username) == 0) {
            
            // Return seats/taxis to available pool
            struct Route* current = head;
            while (current != NULL) {
                if (current->id == bookings[i].route_id) {
                    if (bookings[i].num_seats > 0) {
                        current->available_seats += bookings[i].num_seats;
                    } else {
                        current->available_vehicles += bookings[i].num_taxis;
                    }
                    break;
                }
                current = current->next;
            }

            bookings[i].is_active = 0;
            save_bookings_to_file();
            save_routes_to_file();
            printf("Booking cancelled successfully!\n");
            return;
        }
    }
    printf("Booking not found or already cancelled!\n");
}

void view_bookings(char* username) {
    if (username == NULL || strlen(username) == 0) {
        printf("Error: Invalid username\n");
        return;
    }

    // Load latest bookings
    load_bookings_from_file();

    printf("\nYour Bookings:\n");
    printf("ID\tRoute\tSchedule\tSeats\tTaxis\tFare(₹)\tStatus\n");
    printf("----------------------------------------------------------\n");
    
    for (int i = 0; i < booking_count; i++) {
        if (strcmp(bookings[i].username, username) == 0) {
            printf("%d\t%d\t%d\t\t%d\t%d\t%.2f\t%s\n",
                bookings[i].id,
                bookings[i].route_id,
                bookings[i].schedule_id,
                bookings[i].num_seats,
                bookings[i].num_taxis,
                bookings[i].total_fare,
                bookings[i].is_active ? "Active" : "Cancelled");
        }
    }
}

void update_fare(int is_admin) {
    if (!is_admin) {
        printf("Only admins can update fares!\n");
        return;
    }

    int route_id;
    float new_fare;

    display_routes();
    printf("\nEnter route ID to update fare: ");
    scanf("%d", &route_id);
    getchar();

    printf("Enter new fare: ");
    scanf("%f", &new_fare);
    getchar();

    struct Route* current = head;
    while (current != NULL) {
        if (current->id == route_id) {
            current->fare = new_fare;
            save_routes_to_file();
            printf("Fare updated successfully!\n");
            return;
        }
        current = current->next;
    }
    printf("Route not found!\n");
}
