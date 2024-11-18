#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // Add this line to include STDIN_FILENO
#include "routes.h" // Add routes header for Route struct and head
#include "schedule.h" // Add schedule header for display_schedules()
#include "booking.h" // Add booking header for struct definition
#include <termios.h>

// Global variables defined in header
struct Booking bookings[MAX_BOOKINGS];
int booking_count = 0;

void save_bookings_to_file() {
    FILE* file = fopen("bookings.txt", "w");
    if (file == NULL) {
        printf("⚠️  Error opening bookings file!\n");
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
    printf("\n╔════════════════════════════════════╗\n");
    printf("║         📋 New Booking Form        ║\n");
    printf("╚════════════════════════════════════╝\n\n");

    char client_name[MAX_USERNAME];
    while (getchar() != '\n'); // Clear any leftover input buffer
    printf("🤵 Please enter your full name (e.g. John Smith): ");
    if (fgets(client_name, MAX_USERNAME, stdin) == NULL) {
        printf("⚠️  Error reading name\n");
        return;
    }
    client_name[strcspn(client_name, "\n")] = 0; // Remove newline

    if (strlen(client_name) == 0) {
        printf("⚠️  Name cannot be empty\n");
        return;
    }

    // Load latest bookings and routes
    load_bookings_from_file();
    load_routes_from_file();

    if (booking_count >= MAX_BOOKINGS) {
        printf("⚠️  Maximum booking limit reached!\n");
        return;
    }

    // Check if routes are loaded
    if (head == NULL) {
        printf("⚠️  No routes available!\n");
        return;
    }

    struct Booking new_booking;
    new_booking.id = booking_count + 1;
    strncpy(new_booking.username, client_name, MAX_USERNAME - 1);
    new_booking.username[MAX_USERNAME - 1] = '\0';
    new_booking.is_active = 1;
    new_booking.num_seats = 0;
    new_booking.num_taxis = 0;
    new_booking.total_fare = 0;

    printf("\n📍 Available Routes & Schedules:\n");
    printf("─────────────────────────────────\n");
    // Display available routes and schedules
    display_routes();

    printf("\n🔢 Enter route ID: ");
    if (scanf("%d", &new_booking.route_id) != 1) {
        printf("⚠️  Invalid route ID!\n");
        while (getchar() != '\n'); // Clear input buffer
        return;
    }
    while (getchar() != '\n'); // Clear newline

    // Display only schedules for selected route
    printf("\nSchedules for Route %d:\n", new_booking.route_id);
    printf("╔══════╦═══════════╦════════════════╦══════════╗\n");
    printf("║ ID   ║ Route ID  ║ Departure Time ║ Status   ║\n");
    printf("╠══════╬═══════════╬════════════════╬══════════╣\n");

    FILE* schedule_file = fopen("schedules.txt", "r");
    if (schedule_file == NULL) {
        printf("⚠️  Error opening schedules file!\n");
        return;
    }

    char line[256];
    int found_schedules = 0;
    while (fgets(line, sizeof(line), schedule_file)) {
        int id, route_id, hour, minute, active;
        sscanf(line, "%d|%d|%d|%d|%d", &id, &route_id, &hour, &minute, &active);
        if (route_id == new_booking.route_id) {
            printf("║ %-4d ║ %-9d ║ %02d:%02d          ║ %-8s ║\n",
                id, route_id, hour, minute, active ? "Active" : "Inactive");
            found_schedules = 1;
        }
    }
    printf("╚══════╩═══════════╩════════════════╩══════════╝\n");
    fclose(schedule_file);

    if (!found_schedules) {
        printf("⚠️  No schedules found for this route!\n");
        return;
    }

    printf("\n🕒 Enter schedule ID: ");
    if (scanf("%d", &new_booking.schedule_id) != 1) {
        printf("⚠️  Invalid schedule ID!\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    // Verify schedule belongs to selected route and is active
    schedule_file = fopen("schedules.txt", "r");
    int valid_schedule = 0;
    int schedule_active = 0;
    if (schedule_file != NULL) {
        while (fgets(line, sizeof(line), schedule_file)) {
            int id, route_id, hour, minute, active;
            sscanf(line, "%d|%d|%d|%d|%d", &id, &route_id, &hour, &minute, &active);
            if (id == new_booking.schedule_id) {
                if (route_id != new_booking.route_id) {
                    printf("⚠️  Selected schedule does not belong to route %d!\n", new_booking.route_id);
                    fclose(schedule_file);
                    return;
                }
                valid_schedule = 1;
                schedule_active = active;
                break;
            }
        }
        fclose(schedule_file);
    }

    if (!valid_schedule) {
        printf("⚠️  Invalid schedule ID!\n");
        return;
    }

    if (!schedule_active) {
        printf("⚠️  Selected schedule is inactive!\n");
        return;
    }

    // Find the route
    struct Route* current = head;
    int route_found = 0;
    while (current != NULL) {
        if (current->id == new_booking.route_id && current->is_active) {
            route_found = 1;
            if (strstr(current->vehicle_type, "Bus") != NULL) {
                printf("\n🚌 Enter number of seats to book (max %d): ", current->available_seats);
                if (scanf("%d", &new_booking.num_seats) != 1) {
                    printf("⚠️  Invalid number of seats!\n");
                    while (getchar() != '\n');
                    return;
                }
                while (getchar() != '\n');

                if (new_booking.num_seats <= 0 || new_booking.num_seats > current->available_seats) {
                    printf("⚠️  Invalid number of seats!\n");
                    return;
                }
                new_booking.total_fare = current->fare * new_booking.num_seats;
            } else {
                printf("\n🚕 Enter number of taxis to book (max %d): ", current->available_vehicles);
                if (scanf("%d", &new_booking.num_taxis) != 1) {
                    printf("⚠️  Invalid number of taxis!\n");
                    while (getchar() != '\n');
                    return;
                }
                while (getchar() != '\n');

                if (new_booking.num_taxis <= 0 || new_booking.num_taxis > current->available_vehicles) {
                    printf("⚠️  Invalid number of taxis!\n");
                    return;
                }
                new_booking.total_fare = current->fare * new_booking.num_taxis;
            }

            printf("\n💰 Total fare: ₹%.2f\n", new_booking.total_fare);
            printf("\n🔒 Would you like to confirm and pay for this booking? (y/n): ");
            char confirm;
            scanf(" %c", &confirm);
            while (getchar() != '\n');

            if (confirm != 'y' && confirm != 'Y') {
                printf("❌ Booking cancelled.\n");
                return;
            }

            printf("\n🔑 Please enter your full name again to authorize payment: ");
            char auth_name[MAX_USERNAME];
            struct termios old_term, new_term;
            int i = 0;
            char c;

            if (tcgetattr(STDIN_FILENO, &old_term) != 0) {
                perror("tcgetattr");
                return;
            }
            new_term = old_term;
            new_term.c_lflag &= ~(ECHO | ICANON);
            if (tcsetattr(STDIN_FILENO, TCSANOW, &new_term) != 0) {
                perror("tcsetattr");
                return;
            }

            while ((c = getchar()) != '\n' && i < MAX_USERNAME - 1) {
                auth_name[i] = c;
                printf("*");
                i++;
            }
            auth_name[i] = '\0';
            printf("\n");

            if (tcsetattr(STDIN_FILENO, TCSANOW, &old_term) != 0) {
                perror("tcsetattr");
                return;
            }

            if (strcmp(auth_name, client_name) != 0) {
                printf("⚠️  Authorization failed - name does not match!\n");
                return;
            }

            if (strstr(current->vehicle_type, "Bus") != NULL) {
                current->available_seats -= new_booking.num_seats;
            } else {
                current->available_vehicles -= new_booking.num_taxis;
            }

            bookings[booking_count++] = new_booking;
            save_bookings_to_file();
            save_routes_to_file();
            printf("\n✅ Booking confirmed and payment processed!\n");
            printf("🎫 Your booking ID is: %d\n", new_booking.id);
            return;
        }
        current = current->next;
    }
    
    if (!route_found) {
        printf("⚠️  Route not found or inactive!\n");
    }
}

void cancel_booking(char* username) {
    printf("\n╔════════════════════════════════════╗\n");
    printf("║        🚫 Cancel Booking           ║\n");
    printf("╚════════════════════════════════════╝\n\n");

    char client_name[MAX_USERNAME];
    while (getchar() != '\n'); // Clear any leftover input buffer
    printf("🤵 Please enter your full name (e.g. John Smith): ");
    if (fgets(client_name, MAX_USERNAME, stdin) == NULL) {
        printf("⚠️  Error reading name\n");
        return;
    }
    client_name[strcspn(client_name, "\n")] = 0; // Remove newline

    if (strlen(client_name) == 0) {
        printf("⚠️  Name cannot be empty\n");
        return;
    }

    // Load latest data
    load_bookings_from_file();
    load_routes_from_file();

    int booking_id;
    int found = 0;

    printf("\n📋 Your current bookings:\n");
    printf("╔═════╦═══════╦══════════╦═══════╦═══════╦══════════╗\n");
    printf("║ ID  ║ Route ║ Schedule ║ Seats ║ Taxis ║ Fare(₹)  ║\n");
    printf("╠═════╬═══════╬══════════╬═══════╬═══════╬══════════╣\n");

    for (int i = 0; i < booking_count; i++) {
        if (bookings[i].is_active && strcmp(bookings[i].username, client_name) == 0) {
            printf("║ %-3d ║ %-5d ║ %-8d ║ %-5d ║ %-5d ║ %-8.2f ║\n",
                bookings[i].id,
                bookings[i].route_id,
                bookings[i].schedule_id,
                bookings[i].num_seats,
                bookings[i].num_taxis,
                bookings[i].total_fare);
            found = 1;
        }
    }
    printf("╚═════╩═══════╩══════════╩═══════╩═══════╩══════════╝\n");

    if (!found) {
        printf("\n⚠️  No active bookings found!\n");
        return;
    }

    printf("\n🔢 Enter booking ID to cancel: ");
    if (scanf("%d", &booking_id) != 1) {
        printf("⚠️  Invalid booking ID!\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    for (int i = 0; i < booking_count; i++) {
        if (bookings[i].id == booking_id && bookings[i].is_active && 
            strcmp(bookings[i].username, client_name) == 0) {

            printf("\n🔑 Please enter your full name again to authorize cancellation: ");
            char auth_name[MAX_USERNAME];
            struct termios old_term, new_term;
            int j = 0;
            char c;

            if (tcgetattr(STDIN_FILENO, &old_term) != 0) {
                perror("tcgetattr");
                return;
            }
            new_term = old_term;
            new_term.c_lflag &= ~(ECHO | ICANON);
            if (tcsetattr(STDIN_FILENO, TCSANOW, &new_term) != 0) {
                perror("tcsetattr");
                return;
            }

            while ((c = getchar()) != '\n' && j < MAX_USERNAME - 1) {
                auth_name[j] = c;
                printf("*");
                j++;
            }
            auth_name[j] = '\0';
            printf("\n");

            if (tcsetattr(STDIN_FILENO, TCSANOW, &old_term) != 0) {
                perror("tcsetattr");
                return;
            }

            if (strcmp(auth_name, client_name) != 0) {
                printf("⚠️  Authorization failed - name does not match!\n");
                return;
            }
            
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

            // Calculate cancellation fee (20% of total fare)
            float cancellation_fee = bookings[i].total_fare * 0.20;
            float refund_amount = bookings[i].total_fare - cancellation_fee;

            bookings[i].is_active = 0; // Mark booking as cancelled
           
            save_bookings_to_file();
            save_routes_to_file();
            printf("\n✅ Booking cancelled successfully!\n");
            printf("💰 Cancellation fee (20%%): ₹%.2f\n", cancellation_fee);
            printf("💰 Refunded amount: ₹%.2f\n", refund_amount);
            return;
        }
    }
    printf("\n⚠️  Booking not found or already cancelled!\n");
}

void view_bookings(char* username) {
    printf("\n╔════════════════════════════════════╗\n");
    printf("║         📋 View Bookings           ║\n");
    printf("╚════════════════════════════════════╝\n\n");

    char client_name[MAX_USERNAME];
    while (getchar() != '\n'); // Clear any leftover input buffer
    printf("🤵 Please enter your full name (e.g. John Smith): ");
    if (fgets(client_name, MAX_USERNAME, stdin) == NULL) {
        printf("⚠️  Error reading name\n");
        return;
    }
    client_name[strcspn(client_name, "\n")] = 0; // Remove newline

    if (strlen(client_name) == 0) {
        printf("⚠️  Name cannot be empty\n");
        return;
    }

    // Load latest bookings
    load_bookings_from_file();

    printf("\n📋 Your Bookings:\n");
    printf("╔═════╦═══════╦══════════╦═══════╦═══════╦══════════╦══════════╗\n");
    printf("║ ID  ║ Route ║ Schedule ║ Seats ║ Taxis ║ Fare(₹)  ║ Status   ║\n");
    printf("╠═════╬═══════╬══════════╬═══════╬═══════╬══════════╬══════════╣\n");
    
    for (int i = 0; i < booking_count; i++) {
        if (strcmp(bookings[i].username, client_name) == 0) {
            printf("║ %-3d ║ %-5d ║ %-8d ║ %-5d ║ %-5d ║ %-8.2f ║ %-8s ║\n",
                bookings[i].id,
                bookings[i].route_id,
                bookings[i].schedule_id,
                bookings[i].num_seats,
                bookings[i].num_taxis,
                bookings[i].total_fare,
                bookings[i].is_active ? "Active" : "Cancelled");
        }
    }
    printf("╚═════╩═══════╩══════════╩═══════╩═══════╩══════════╩══════════╝\n");
}

void update_fare(int is_admin) {
    if (!is_admin) {
        printf("⚠️  Only admins can update fares!\n");
        return;
    }

    printf("\n╔════════════════════════════════════╗\n");
    printf("║         💰 Update Fares            ║\n");
    printf("╚════════════════════════════════════╝\n\n");

    int route_id;
    float new_fare;

    display_routes();
    printf("\n🔢 Enter route ID to update fare: ");
    scanf("%d", &route_id);
    getchar();

    printf("💰 Enter new fare: ");
    scanf("%f", &new_fare);
    getchar();

    struct Route* current = head;
    while (current != NULL) {
        if (current->id == route_id) {
            current->fare = new_fare;
            save_routes_to_file();
            printf("\n✅ Fare updated successfully!\n");
            return;
        }
        current = current->next;
    }
    printf("\n⚠️  Route not found!\n");
}
