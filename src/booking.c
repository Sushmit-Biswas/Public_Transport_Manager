#include <stdio.h>      // For input and output
#include <stdlib.h>     // For exit function
#include <string.h>     // For string manipulation  
#include <unistd.h>     // For getchar function
#include "routes.h"     // Include routes header for Route struct and head
#include "schedule.h"   // Include schedule header for display_schedules()
#include "booking.h"    // Include booking header for struct definition
#include <termios.h>    // For password input (works on Linux)

// Global variables defined in header
struct Booking bookings[MAX_BOOKINGS]; // Array to store booking records
int booking_count = 0; // Count of current bookings

// Function to save bookings to file
void save_bookings_to_file() {
    // Open bookings file in write mode
    FILE* file = fopen("bookings.txt", "w");
    if (file == NULL) {
        printf("⚠️  Error opening bookings file!\n");
        return;
    }

    // Loop through existing bookings and write active ones to file
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
    fclose(file); // Close file after writing
}

// Function to load bookings from file
void load_bookings_from_file() {
    // Open bookings file in read mode
    FILE* file = fopen("bookings.txt", "r");
    if (file == NULL) {
        return; // File doesn't exist yet
    }

    char line[256];
    booking_count = 0; // Reset booking count
    // Read each line and parse booking details
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
        booking_count++; // Increment booking count
    }
    fclose(file); // Close file after reading
}

// Function to make a new booking
void make_booking(char* username) {
    printf("\n╔════════════════════════════════════╗\n");
    printf("║         📋 New Booking Form        ║\n");
    printf("╚════════════════════════════════════╝\n\n");

    char client_name[MAX_USERNAME]; // Buffer to store client's name
    while (getchar() != '\n'); // Clear any leftover input buffer
    printf("🤵 Please enter your full name (e.g. John Smith): ");
    if (fgets(client_name, MAX_USERNAME, stdin) == NULL) {
        printf("⚠️  Error reading name\n");
        return; // Error handling for reading name
    }
    client_name[strcspn(client_name, "\n")] = 0; // Remove newline

    if (strlen(client_name) == 0) {
        printf("⚠️  Name cannot be empty\n");
        return; // Validate name input
    }

    // Load latest bookings and routes
    load_bookings_from_file();
    load_routes_from_file();

    if (booking_count >= MAX_BOOKINGS) {
        printf("⚠️  Maximum booking limit reached!\n");
        return; // Check for maximum booking limit
    }

    // Check if routes are loaded
    if (head == NULL) {
        printf("⚠️  No routes available!\n");
        return; // Validate routes availability
    }

    struct Booking new_booking; // Create a new booking instance
    new_booking.id = booking_count + 1; // Assign an ID
    strncpy(new_booking.username, client_name, MAX_USERNAME - 1); // Copy username
    new_booking.username[MAX_USERNAME - 1] = '\0'; // Null terminate username
    new_booking.is_active = 1; // Set booking as active
    new_booking.num_seats = 0; // Initialize seat count
    new_booking.num_taxis = 0; // Initialize taxi count
    new_booking.total_fare = 0; // Initialize total fare

    printf("\n📍 Available Routes & Schedules:\n");
    printf("─────────────────────────────────\n");
    // Display available routes and schedules
    display_routes();

    printf("\n🔢 Enter route ID: "); // Prompt for route selection
    if (scanf("%d", &new_booking.route_id) != 1) {
        printf("⚠️  Invalid route ID!\n");
        while (getchar() != '\n'); // Clear input buffer
        return;
    }
    while (getchar() != '\n'); // Clear newline

    // Display schedules for the selected route
    printf("\nSchedules for Route %d:\n", new_booking.route_id);
    printf("╔══════╦═══════════╦════════════════╦══════════╗\n");
    printf("║ ID   ║ Route ID  ║ Departure Time ║ Status   ║\n");
    printf("╠══════╬═══════════╬════════════════╬══════════╣\n");

    FILE* schedule_file = fopen("schedules.txt", "r"); // Open schedule file
    if (schedule_file == NULL) {
        printf("⚠️  Error opening schedules file!\n");
        return; // Error opening the file
    }

    char line[256];
    int found_schedules = 0; // Flag for checking found schedules
    // Read schedules and filter by selected route
    while (fgets(line, sizeof(line), schedule_file)) {
        int id, route_id, hour, minute, active;
        sscanf(line, "%d|%d|%d|%d|%d", &id, &route_id, &hour, &minute, &active);
        if (route_id == new_booking.route_id) {
            printf("║ %-4d ║ %-9d ║ %02d:%02d          ║ %-8s ║\n",
                id, route_id, hour, minute, active ? "Active" : "Inactive");
            found_schedules = 1; // Mark schedule found
        }
    }
    printf("╚══════╩═══════════╩════════════════╩══════════╝\n");
    fclose(schedule_file); // Close schedule file

    if (!found_schedules) {
        printf("⚠️  No schedules found for this route!\n");
        return; // Handle no schedules found case
    }

    printf("\n🕒 Enter schedule ID: "); // Prompt for schedule selection
    if (scanf("%d", &new_booking.schedule_id) != 1) {
        printf("⚠️  Invalid schedule ID!\n");
        while (getchar() != '\n');
        return; // Validate schedule ID input
    }
    while (getchar() != '\n'); // Clear newline

    // Verify if selected schedule belongs to route and is active
    schedule_file = fopen("schedules.txt", "r");
    int valid_schedule = 0; // Flag for valid schedule
    int schedule_active = 0; // Flag for checking if schedule is active
    if (schedule_file != NULL) {
        while (fgets(line, sizeof(line), schedule_file)) {
            int id, route_id, hour, minute, active;
            sscanf(line, "%d|%d|%d|%d|%d", &id, &route_id, &hour, &minute, &active);
            if (id == new_booking.schedule_id) {
                if (route_id != new_booking.route_id) {
                    printf("⚠️  Selected schedule does not belong to route %d!\n", new_booking.route_id);
                    fclose(schedule_file);
                    return; // Validate schedule-route association
                }
                valid_schedule = 1; // Schedule is valid
                schedule_active = active; // Check status of schedule
                break; // Exit loop once found
            }
        }
        fclose(schedule_file); // Close schedule file
    }

    // Validate schedule ID
    if (!valid_schedule) {
        printf("⚠️  Invalid schedule ID!\n");
        return; // Handle invalid schedule case
    }

    // Check if the schedule is currently active
    if (!schedule_active) {
        printf("⚠️  Selected schedule is currently inactive!\n");
        return; // Handle schedule inactivity
    }

    // Find the selected route based on the route ID
    struct Route* current = head;
    int route_found = 0; // Flag for finding the route
    while (current != NULL) {
        if (current->id == new_booking.route_id && current->is_active) {
            route_found = 1; // Route found
            // Check if the vehicle type is Bus and prompt for the number of seats
            if (strstr(current->vehicle_type, "Bus") != NULL) {
                printf("\n🚌 Enter number of seats to book (max %d): ", current->available_seats);
                if (scanf("%d", &new_booking.num_seats) != 1) {
                    printf("⚠️  Invalid number of seats!\n");
                    while (getchar() != '\n');
                    return; // Validate seats input
                }
                while (getchar() != '\n'); // Clear newline

                // Validate number of seats against available seats
                if (new_booking.num_seats <= 0 || new_booking.num_seats > current->available_seats) {
                    printf("⚠️  Invalid number of seats!\n");
                    return; // Handle invalid seat count
                }
                new_booking.total_fare = current->fare * new_booking.num_seats; // Calculate fare
            } else { // If vehicle is not a bus, ask for taxis
                printf("\n🚕 Enter number of taxis to book (max %d): ", current->available_vehicles);
                if (scanf("%d", &new_booking.num_taxis) != 1) {
                    printf("⚠️  Invalid number of taxis!\n");
                    while (getchar() != '\n');
                    return; // Validate taxis input
                }
                while (getchar() != '\n'); // Clear newline

                // Validate number of taxis against available vehicles
                if (new_booking.num_taxis <= 0 || new_booking.num_taxis > current->available_vehicles) {
                    printf("⚠️  Invalid number of taxis!\n");
                    return; // Handle invalid taxi count
                }
                new_booking.total_fare = current->fare * new_booking.num_taxis; // Calculate fare
            }

            // Display total fare for confirmation
            printf("\n💰 Total fare: ₹%.2f\n", new_booking.total_fare);
            printf("\n🔒 Would you like to confirm and pay for this booking? (y/n): ");
            char confirm;
            scanf(" %c", &confirm); // Confirmation input
            while (getchar() != '\n'); // Clear newline

            if (confirm != 'y' && confirm != 'Y') {
                printf("❌ Booking cancelled.\n");
                return; // Handle booking cancellation
            }

            // Authorization process for payment
            printf("\n🔑 Please enter your full name again to authorize payment: ");
            char auth_name[MAX_USERNAME];
            struct termios old_term, new_term; // For password masking
            int i = 0; // Index for input
            char c; // Char for reading input

            // Set terminal attributes for hiding input
            if (tcgetattr(STDIN_FILENO, &old_term) != 0) {
                perror("tcgetattr");
                return;
            }
            new_term = old_term;
            new_term.c_lflag &= ~(ECHO | ICANON); // Disable echo
            if (tcsetattr(STDIN_FILENO, TCSANOW, &new_term) != 0) {
                perror("tcsetattr");
                return;
            }

            // Read authorization name and mask input
            while ((c = getchar()) != '\n' && i < MAX_USERNAME - 1) {
                auth_name[i] = c;
                printf("*"); // Print asterisks for each character
                i++;
            }
            auth_name[i] = '\0'; // Null terminate name
            printf("\n");

            // Restore terminal settings
            if (tcsetattr(STDIN_FILENO, TCSANOW, &old_term) != 0) {
                perror("tcsetattr");
                return;
            }

            // Validate authorization name
            if (strcmp(auth_name, client_name) != 0) {
                printf("⚠️  Authorization failed - name does not match!\n");
                return; // Handle authorization failure
            }

            // Process booking and update availability
            if (strstr(current->vehicle_type, "Bus") != NULL) {
                current->available_seats -= new_booking.num_seats; // Update available seats for bus
            } else {
                current->available_vehicles -= new_booking.num_taxis; // Update available vehicles for taxi
            }

            bookings[booking_count++] = new_booking; // Store new booking
            save_bookings_to_file(); // Save bookings to file
            save_routes_to_file(); // Save updated routes to file
            printf("\n✅ Booking confirmed and payment processed!\n");
            printf("🎫 Your booking ID is: %d\n", new_booking.id); // Show booking confirmation
            return; // Exit function after successful booking
        }
        current = current->next; // Move to the next route in the list
    }
    
    if (!route_found) {
        printf("⚠️  Route not found or currently inactive!\n");
    } // Handle route not found case
}

// Function to cancel a booking
void cancel_booking(char* username) {
    printf("\n╔════════════════════════════════════╗\n");
    printf("║        🚫 Cancel Booking           ║\n");
    printf("╚════════════════════════════════════╝\n\n");

    char client_name[MAX_USERNAME]; // Buffer to store client's name
    while (getchar() != '\n'); // Clear any leftover input buffer
    printf("🤵 Please enter your full name (e.g. John Smith): ");
    if (fgets(client_name, MAX_USERNAME, stdin) == NULL) {
        printf("⚠️  Error reading name\n");
        return; // Error handling for reading name
    }
    client_name[strcspn(client_name, "\n")] = 0; // Remove newline

    if (strlen(client_name) == 0) {
        printf("⚠️  Name cannot be empty\n");
        return; // Validate name input
    }

    // Load latest data
    load_bookings_from_file();
    load_routes_from_file();

    int booking_id; // Variable for booking to cancel
    int found = 0; // Flag to check for any active bookings

    printf("\n📋 Your current bookings:\n");
    printf("╔═════╦═══════╦══════════╦═══════╦═══════╦══════════╗\n");
    printf("║ ID  ║ Route ║ Schedule ║ Seats ║ Taxis ║ Fare(₹)  ║\n");
    printf("╠═════╬═══════╬══════════╬═══════╬═══════╬══════════╣\n");

    // Display active bookings for the user
    for (int i = 0; i < booking_count; i++) {
        if (bookings[i].is_active && strcmp(bookings[i].username, client_name) == 0) {
            printf("║ %-3d ║ %-5d ║ %-8d ║ %-5d ║ %-5d ║ %-8.2f ║\n",
                bookings[i].id,
                bookings[i].route_id,
                bookings[i].schedule_id,
                bookings[i].num_seats,
                bookings[i].num_taxis,
                bookings[i].total_fare);
            found = 1; // Mark active booking found
        }
    }
    printf("╚═════╩═══════╩══════════╩═══════╩═══════╩══════════╝\n");

    if (!found) {
        printf("\n⚠️  No active bookings found!\n");
        return; // Handle case where no active bookings exist
    }

    printf("\n🔢 Enter booking ID to cancel: "); // Prompt for booking ID to cancel
    if (scanf("%d", &booking_id) != 1) {
        printf("⚠️  Invalid booking ID!\n");
        while (getchar() != '\n');
        return; // Validate booking ID input
    }
    while (getchar() != '\n'); // Clear newline

    // Loop through bookings to find the one to cancel
    for (int i = 0; i < booking_count; i++) {
        // Check for matching booking ID and active status
        if (bookings[i].id == booking_id && bookings[i].is_active && 
            strcmp(bookings[i].username, client_name) == 0) {

            // Authorize cancellation
            printf("\n🔑 Please enter your full name again to authorize cancellation: ");
            char auth_name[MAX_USERNAME];
            struct termios old_term, new_term; // For password masking
            int j = 0; // Index for input
            char c; // Char for reading input

            // Set terminal attributes for hiding input
            if (tcgetattr(STDIN_FILENO, &old_term) != 0) {
                perror("tcgetattr");
                return;
            }
            new_term = old_term;
            new_term.c_lflag &= ~(ECHO | ICANON); // Disable echo
            if (tcsetattr(STDIN_FILENO, TCSANOW, &new_term) != 0) {
                perror("tcsetattr");
                return;
            }

            // Read authorization name and mask input
            while ((c = getchar()) != '\n' && j < MAX_USERNAME - 1) {
                auth_name[j] = c;
                printf("*"); // Print asterisks for each character
                j++;
            }
            auth_name[j] = '\0'; // Null terminate name
            printf("\n");

            // Restore terminal settings
            if (tcsetattr(STDIN_FILENO, TCSANOW, &old_term) != 0) {
                perror("tcsetattr");
                return;
            }

            // Validate authorization name
            if (strcmp(auth_name, client_name) != 0) {
                printf("⚠️  Authorization failed - name does not match!\n");
                return; // Handle authorization failure
            }
            
            // Return seats/taxis to available pool
            struct Route* current = head;
            while (current != NULL) {
                if (current->id == bookings[i].route_id) {
                    if (bookings[i].num_seats > 0) {
                        current->available_seats += bookings[i].num_seats; // Update available seats
                    } else {
                        current->available_vehicles += bookings[i].num_taxis; // Update available taxis
                    }
                    break; // Exit loop once found
                }
                current = current->next; // Move to next route
            }

            // Calculate cancellation fee (20% of total fare)
            float cancellation_fee = bookings[i].total_fare * 0.20;
            float refund_amount = bookings[i].total_fare - cancellation_fee; // Calculate refund amount

            bookings[i].is_active = 0; // Mark booking as cancelled
           
            save_bookings_to_file(); // Save updated bookings to file
            save_routes_to_file(); // Save updated routes to file
            printf("\n✅ Booking cancelled successfully!\n");
            printf("💰 Cancellation fee (20%%): ₹%.2f\n", cancellation_fee);
            printf("💰 Refunded amount: ₹%.2f\n", refund_amount); // Display refund information
            return; // Exit function after successful cancellation
        }
    }
    printf("\n⚠️  Booking not found or already cancelled!\n"); // Handle case where booking not found
}

// Function to view bookings
void view_bookings(char* username) {
    printf("\n╔════════════════════════════════════╗\n");
    printf("║         📋 View Bookings           ║\n");
    printf("╚════════════════════════════════════╝\n\n");

    char client_name[MAX_USERNAME]; // Buffer to store client's name
    while (getchar() != '\n'); // Clear any leftover input buffer
    printf("🤵 Please enter your full name (e.g. John Smith): ");
    if (fgets(client_name, MAX_USERNAME, stdin) == NULL) {
        printf("⚠️  Error reading name\n");
        return; // Error handling for reading name
    }
    client_name[strcspn(client_name, "\n")] = 0; // Remove newline

    if (strlen(client_name) == 0) {
        printf("⚠️  Name cannot be empty\n");
        return; // Validate name input
    }

    // Load latest bookings
    load_bookings_from_file();

    printf("\n📋 Your Bookings:\n");
    printf("╔═════╦═══════╦══════════╦═══════╦═══════╦══════════╦══════════╗\n");
    printf("║ ID  ║ Route ║ Schedule ║ Seats ║ Taxis ║ Fare(₹)  ║ Status   ║\n");
    printf("╠═════╬═══════╬══════════╬═══════╬═══════╬══════════╬══════════╣\n");
    
    // Display user's bookings
    for (int i = 0; i < booking_count; i++) {
        if (strcmp(bookings[i].username, client_name) == 0) {
            printf("║ %-3d ║ %-5d ║ %-8d ║ %-5d ║ %-5d ║ %-8.2f ║ %-8s ║\n",
                bookings[i].id,
                bookings[i].route_id,
                bookings[i].schedule_id,
                bookings[i].num_seats,
                bookings[i].num_taxis,
                bookings[i].total_fare,
                bookings[i].is_active ? "Active" : "Cancelled"); // Display status of booking
        }
    }
    printf("╚═════╩═══════╩══════════╩═══════╩═══════╩══════════╩══════════╝\n");
}

// Function to update fares
void update_fare(int is_admin) {
    if (!is_admin) {
        printf("⚠️  Only admins can update fares!\n");
        return; // Check for admin permissions
    }

    printf("\n╔════════════════════════════════════╗\n");
    printf("║         💰 Update Fares            ║\n");
    printf("╚════════════════════════════════════╝\n\n");

    int route_id; // Variable to hold route ID for fare update
    float new_fare; // Variable to hold new fare value

    display_routes(); // Display current routes
    printf("\n🔢 Enter route ID to update fare: ");
    scanf("%d", &route_id); // Input route ID for fare update
    getchar(); // Clear buffer

    printf("💰 Enter new fare: "); // Prompt for new fare
    scanf("%f", &new_fare); // Input new fare value
    getchar(); // Clear buffer

    struct Route* current = head; // Pointer to traverse the route list
    while (current != NULL) {
        if (current->id == route_id) { // Match route ID
            current->fare = new_fare; // Update fare
            save_routes_to_file(); // Save updated fares to file
            printf("\n✅ Fare updated successfully!\n");
            return; // Exit after updating fare
        }
        current = current->next; // Move to next route
    }
    printf("\n⚠️  Route not found!\n"); // Handle case where route ID doesn't match
}