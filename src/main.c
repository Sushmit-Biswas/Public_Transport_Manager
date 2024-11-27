#include <stdio.h>        // For input and output
#include <string.h>       // For string manipulation
#include <time.h>         // For time functions
#include "routes.h"       // For route functions
#include "schedule.h"     // For schedule functions
#include "auth.h"         // For authentication functions
#include "booking.h"      // For booking functions   

// ANSI color codes for text formatting in the console
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

// Global variable for storing the current user's name
char* current_user = NULL;  

// Variable to track if the current user is an admin
int is_admin = 0;           

// Function to display admin logs from a file
void view_admin_logs() {
    FILE* logfile = fopen("admin_logs.txt", "r"); // Open log file for reading
    
    // Check if the log file opened successfully
    if (logfile == NULL) {  
        printf("%s%sError opening log file!%s\n", BOLD, RED, RESET);
        return; // Exit the function if the file cannot be opened
    }

    // Print table header for logs
    printf("\n%s%sAdmin Activity Logs:%s\n", BOLD, YELLOW, RESET);
    printf("%s╔══════════════════════════╦══════════════════╦═══════════════════════════╗%s\n", BOLD, RESET);
    printf("%s║ %-24s ║ %-16s ║ %-25s ║%s\n", BOLD, "Timestamp", "Admin", "Action", RESET);
    printf("%s╠══════════════════════════╬══════════════════╬═══════════════════════════╣%s\n", BOLD, RESET);

    char line[256]; // Buffer for reading a line from the log
    int found = 0;  // Flag to check if any logs are found

    // Read file line by line
    while (fgets(line, sizeof(line), logfile)) {  
        char timestamp[50], admin[50], action[100]; // Variables for storing parsed log data
        
        // Extract timestamp from the log entry
        char *start = strchr(line, '[') + 1; // Find first '['
        char *end = strchr(line, ']');       // Find first ']'
        
        // Copy timestamp
        strncpy(timestamp, start, end - start); 
        timestamp[end - start] = '\0'; // Null-terminate string

        // Extract admin name from the log entry
        start = strstr(line, "Admin: ") + 7; // Start of the admin name
        end = strstr(line, " | Action: ");    // End of the admin name
        
        // Copy admin name
        strncpy(admin, start, end - start); 
        admin[end - start] = '\0'; // Null-terminate string

        // Extract action description from the log entry
        start = strstr(line, "Action: ") + 8; // Start of the action description
        strcpy(action, start); // Copy action description
        action[strlen(action)-1] = '\0'; // Remove newline

        // Print the log entry in table format
        printf("║ %-24s ║ %-16s ║ %-25s ║\n", timestamp, admin, action);
        found = 1; // Set found flag to true
    }

    // If no logs were found, print an appropriate message
    if (!found) {  
        printf("║ %s%-69s%s ║\n", BOLD, "No admin logs found", RESET);
    }

    // Print the closing row of the log table
    printf("%s╚══════════════════════════╩══════════════════╩═══════════════════════════╝%s\n", BOLD, RESET);
    
    fclose(logfile); // Close the log file
}

// Function to log admin actions to a file
void log_admin_action(const char* username, const char* action) {
    FILE* logfile = fopen("admin_logs.txt", "a"); // Open log file for appending

    // Check if the log file opened successfully
    if (logfile == NULL) {  
        printf("%s%sError opening log file!%s\n", BOLD, RED, RESET);
        return; // Exit the function if the file cannot be opened
    }

    time_t now; // Variable to hold current time
    time(&now); // Get current time
    
    struct tm *local_time = localtime(&now); // Convert to local time
    char date[50]; // Buffer for formatted date
    
    // Format date
    strftime(date, sizeof(date), "%a %b %d %H:%M:%S %Y", local_time); 

    // Use the username provided to log the action
    const char* admin_name = username && strlen(username) > 0 ? username : "Unknown";

    // Write log entry to the log file
    fprintf(logfile, "[%s] Admin: %s | Action: %s\n", date, admin_name, action); 
    fclose(logfile); // Close the log file
}

// Function to print the header of the console menu
void print_header(const char* title) {
    printf("\n%s%s", BOLD, BLUE);
    printf("╔═════════════════════════════════════════════════════════════╗\n");
    
    // Print title in the header
    printf("║     %-44s   ║\n", title);  
    printf("╚═════════════════════════════════════════════════════════════╝\n%s", RESET);
}

// Function to update vehicle capacity for a specific route
void update_vehicle_capacity() {
    int route_id; // Variable to store route ID entered by the user
    
    printf("\n%s%sEnter route ID to update capacity:%s ", BOLD, CYAN, RESET);
    scanf("%d", &route_id); // Read user input

    load_routes_from_file(); // Load routes from a file
    struct Route* current = head; // Start with the head of the route list
    int found = 0; // Flag to check if the route was found

    // Loop through the routes
    while (current != NULL) {  
        // If the current route matches the entered ID
        if (current->id == route_id) {  
            found = 1; // Set found flag to true
            
            // Print current details for the route
            printf("\nCurrent details for Route %d:\n", route_id);
            printf("Vehicle Type: %s\n", current->vehicle_type); 
            
            // Check if the vehicle type is a bus
            if (strstr(current->vehicle_type, "Bus")) {
                printf("Available Seats: %d\n", current->available_seats);
                printf("\nEnter new number of available seats: "); // Ask for new seat count
                
                int new_seats;
                scanf("%d", &new_seats); // Read new seat count
                while(getchar() != '\n'); // Clear input buffer

                // Check if the new seat count is negative
                if (new_seats < 0) {
                    printf("%s%sInvalid number of seats!%s\n", BOLD, RED, RESET);
                    return; // Exit if input is invalid
                }
                
                current->available_seats = new_seats; // Update available seats
            } else { // If not a bus, it's a taxi
                printf("Available Taxis: %d\n", current->available_vehicles);
                printf("\nEnter new number of available taxis: "); // Ask for new taxi count
                
                int new_taxis;
                scanf("%d", &new_taxis);  // Read new taxi count
                while(getchar() != '\n'); // Clear input buffer

                // Check if the new taxi count is negative
                if (new_taxis < 0) {
                    printf("%s%sInvalid number of taxis!%s\n", BOLD, RED, RESET);
                    return; // Exit if input is invalid
                }
                
                current->available_vehicles = new_taxis; // Update available taxis
            }
            break; // Exit the loop after updating
        }
        current = current->next; // Move to the next route
    }

    // If the route was not found, print an error message
    if (!found) {  
        printf("%s%sRoute not found!%s\n", BOLD, RED, RESET);
        return; // Exit if route not found
    }

    save_routes_to_file(); // Save updated routes back to the file
    printf("%s%sVehicle capacity updated successfully!%s\n", BOLD, GREEN, RESET); // Confirm success
}

// Function to display the admin menu options
void display_admin_menu() {
    print_header("Bengaluru Public Transport Management System (Admin) ");
    
    // Print the menu options available for admin users
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

// Function to display the client menu options
void display_client_menu() {
    print_header("Bengaluru Public Transport Management System (Client)");

    // Print the menu options available for client users
    printf("\n%s%s[1]%s Display All Routes\n", BOLD, CYAN, RESET);
    printf("%s%s[2]%s Filter Routes by Location\n", BOLD, YELLOW, RESET);
    printf("%s%s[3]%s Filter Routes by Vehicle Type\n", BOLD, BRIGHT_BLUE, RESET);
    printf("%s%s[4]%s Display Schedules\n", BOLD, CYAN, RESET);
    printf("%s%s[5]%s Book Trip\n", BOLD, GREEN, RESET);
    printf("%s%s[6]%s Cancel Trip\n", BOLD, RED, RESET);
    printf("%s%s[7]%s View My Bookings\n", BOLD, MAGENTA, RESET);
    printf("%s%s[0]%s Logout\n", BOLD, RED, RESET);
}

// Function to view all bookings made by users
void view_all_bookings() {
    printf("\n%s%sAll Bookings:%s\n", BOLD, YELLOW, RESET);
    
    // Print table header for the bookings
    printf("%s╔═════╦══════════════════╦═══════╦══════════╦═══════╦═══════╦══════════╗%s\n", BOLD, RESET);
    printf("%s║ ID  ║ Client Name      ║ Route ║ Schedule ║ Seats ║ Taxis ║ Fare(₹)  ║%s\n", BOLD, RESET);
    printf("%s╠═════╬══════════════════╬═══════╬══════════╬═══════╬═══════╬══════════╣%s\n", BOLD, RESET);

    load_bookings_from_file(); // Load bookings from a file
    int found = 0; // Flag to check if any bookings are found

    // Loop through bookings
    for (int i = 0; i < booking_count; i++) {  
        if (bookings[i].is_active) { // If the booking is active
            // Print booking details in formatted table
            printf("║ %-3d ║ %-16s ║ %-5d ║ %-8d ║ %-5d ║ %-5d ║ ₹%-7.2f ║\n",
                bookings[i].id,
                bookings[i].username,
                bookings[i].route_id,
                bookings[i].schedule_id,
                bookings[i].num_seats,
                bookings[i].num_taxis,
                bookings[i].total_fare);
            found = 1; // Set found flag to true
        }
    }

    // If no bookings found, print an appropriate message
    if (!found) {  
        printf("║ %s%-73s%s ║\n", BOLD, "No bookings found", RESET);
    }

    printf("%s╚═════╩══════════════════╩═══════╩══════════╩═══════╩═══════╩══════════╝%s\n", BOLD, RESET);
    
    log_admin_action(current_user, "Viewed all bookings"); // Log the action
}

// Function to filter routes by starting and ending location
void filter_routes_by_location() {
    char start[50], end[50]; // Buffers for starting and ending points
    
    printf("\n%s%sEnter starting point (or press Enter to skip):%s ", BOLD, CYAN, RESET);
    getchar(); // Clear buffer
    fgets(start, sizeof(start), stdin); // Read starting point
    start[strcspn(start, "\n")] = 0; // Remove newline from input

    printf("%s%sEnter destination point (or press Enter to skip):%s ", BOLD, CYAN, RESET);
    fgets(end, sizeof(end), stdin); // Read ending point
    end[strcspn(end, "\n")] = 0; // Remove newline from input

    int found = 0; // Flag to check if any routes are found
    
    printf("\n%s%sFiltered Routes:%s\n", BOLD, YELLOW, RESET);
    
    // Print table header for filtered routes
    printf("%s╔══════╦════════════════════╦════════════════════╦═══════════════╦══════════╦══════════╗%s\n", BOLD, RESET);
    printf("%s║ %-4s ║ %-18s ║ %-18s ║ %-13s ║ %-8s ║ %-8s ║%s\n", 
           BOLD, "ID", "From", "To", "Type", "Seats", "Fare", RESET);
    printf("%s╠══════╬════════════════════╬════════════════════╬═══════════════╬══════════╬══════════╣%s\n", BOLD, RESET);

    load_routes_from_file(); // Load routes from file
    struct Route* current = head; // Start checking from the head of the route list

    // Loop through the routes
    while (current != NULL) {  
        // Check if the route matches the provided start and end points
        if ((strlen(start) == 0 || strcasecmp(current->start_point, start) == 0) &&
            (strlen(end) == 0 || strcasecmp(current->end_point, end) == 0)) {
            // Print matching route details in formatted table
            printf("║ %-4d ║ %-18s ║ %-18s ║ %-13s ║ %-8d ║ ₹%-7.2f ║\n",
                current->id,
                current->start_point,
                current->end_point,
                current->vehicle_type,
                current->available_seats,
                current->fare);
            found = 1; // Set found flag to true
        }
        current = current->next; // Move to next route
    }
    
    // If no matching routes found, print an appropriate message
    if (!found) {  
        printf("║ %s%-85s%s║\n", BOLD, "No routes found matching the criteria", RESET);
    }

    printf("%s╚══════╩════════════════════╩════════════════════╩═══════════════╩══════════╩══════════╝%s\n", BOLD, RESET);
}

// Function to filter routes by vehicle type
void filter_routes_by_vehicle() {
    printf("\n%s%sFilter by:%s\n", BOLD, YELLOW, RESET);
    
    // Display filter options for the user
    printf("%s1.%s AC Bus\n", BOLD, RESET);
    printf("%s2.%s Non-AC Bus\n", BOLD, RESET);
    printf("%s3.%s AC Taxi\n", BOLD, RESET);
    printf("%s4.%s Non-AC Taxi\n", BOLD, RESET);
    printf("%s%sEnter choice:%s ", BOLD, CYAN, RESET);
    
    int choice; // Variable for storing user's choice
    scanf("%d", &choice); // Read user's choice

    char* filter; // Variable for the filter type
    
    // Check which filter the user selected
    switch(choice) {
        case 1: filter = "AC Bus"; break;   // Filter for AC Bus
        case 2: filter = "Non-AC Bus"; break; // Filter for Non-AC Bus
        case 3: filter = "AC Taxi"; break;   // Filter for AC Taxi
        case 4: filter = "Non-AC Taxi"; break; // Filter for Non-AC Taxi
        default: 
            printf("%s%sInvalid choice!%s\n", BOLD, RED, RESET); 
            return; // Exit if the choice is invalid
    }

    int found = 0; // Flag to check if any routes are found
    printf("\n%s%sFiltered Routes:%s\n", BOLD, YELLOW, RESET);
    
    // Print table header for filtered routes
    printf("%s╔══════╦════════════════════╦════════════════════╦═══════════════╦══════════╦══════════╗%s\n", BOLD, RESET);
    printf("%s║ %-4s ║ %-18s ║ %-18s ║ %-13s ║ %-8s ║ %-8s ║%s\n",
           BOLD, "ID", "From", "To", "Type", "Seats", "Fare", RESET);
    printf("%s╠══════╬════════════════════╬════════════════════╬═══════════════╬══════════╬══════════╣%s\n", BOLD, RESET);

    load_routes_from_file(); // Load routes from file
    struct Route* current = head; // Start checking from the head of the route list

    // Loop through the routes
    while (current != NULL) {  
        // Check if vehicle type matches the selected filter
        if (strcmp(current->vehicle_type, filter) == 0) {
            // Print matching route details in formatted table
            printf("║ %-4d ║ %-18s ║ %-18s ║ %-13s ║ %-8d ║ ₹%-7.2f ║\n",
                current->id,
                current->start_point,
                current->end_point,
                current->vehicle_type,
                current->available_seats,
                current->fare);
            found = 1; // Set found flag to true
        }
        current = current->next; // Move to next route
    }

    // If no matching routes found, print an appropriate message
    if (!found) {  
        printf("║ %s%-85s%s ║\n", BOLD, "No routes found matching the criteria", RESET);
    }

    printf("%s╚══════╩════════════════════╩════════════════════╩═══════════════╩══════════╩══════════╝%s\n", BOLD, RESET);
}

// Main function of the program
int main() {
    int user_type; // Variable for storing user type
    int attempts = 0; // Variable to count authentication attempts
    
    // Allow up to 2 attempts for authentication
    while (attempts < 2) {  
        user_type = authenticate(); // Call authentication function
        
        // Check if authentication was successful
        if (user_type == 1 || user_type == 2) {  
            break; // Exit the loop if successful
        }
        attempts++; // Increment attempts
        
        // Notify the user about the failed authentication attempt
        if (attempts < 2) {  
            printf("%s%sAuthentication failed. You have 1 more attempt.%s\n", BOLD, RED, RESET);
        }
    }
    
    // If maximum attempts exceeded, exit the program
    if (attempts == 2) {  
        printf("%s%sAuthentication failed. Maximum attempts exceeded. Exiting...%s\n", BOLD, RED, RESET);
        return 1; // Exit the program
    }

    int choice; // Variable for storing user menu choices
    
    // Infinite loop for showing menu
    while (1) {  
        if (user_type == 1) { // If user is admin
            display_admin_menu(); // Display admin menu
            
            printf("\n%s%sEnter your choice:%s ", BOLD, CYAN, RESET);
            
            // Read user's choice from the menu
            if (scanf("%d", &choice) != 1) {
                printf("%s%sInvalid input! Please enter a number.%s\n", BOLD, RED, RESET);
                
                while (getchar() != '\n'); // Clear input buffer if invalid
                continue; // Start over
            }
            
            // Handle user choice based on the admin menu
            switch (choice) {
                case 1:
                    add_route(1); // Function to add a route
                    log_admin_action(current_user, "Added new route"); // Log the action
                    break;
                case 2:
                    delete_route(1); // Function to delete a route
                    log_admin_action(current_user, "Deleted route"); // Log the action
                    break;
                case 3:
                    display_routes(); // Function to display all routes
                    log_admin_action(current_user, "Viewed all routes"); // Log the action
                    break;
                case 4:
                    add_schedule(current_user); // Function to add a schedule
                    log_admin_action(current_user, "Added new schedule"); // Log the action
                    break;
                case 5:
                    delete_schedule(current_user); // Function to delete a schedule
                    log_admin_action(current_user, "Deleted schedule"); // Log the action
                    break;
                case 6:
                    display_schedules(); // Function to display all schedules
                    log_admin_action(current_user, "Viewed all schedules"); // Log the action
                    break;
                case 7:
                    update_fare(1); // Function to update trip fares
                    log_admin_action(current_user, "Updated fare"); // Log the action
                    break;
                case 8:
                    update_vehicle_capacity(); // Function to update vehicle capacity
                    log_admin_action(current_user, "Updated vehicle capacity"); // Log the action
                    break;
                case 9:
                    view_all_bookings(); // Function to view all bookings
                    break;
                case 10:
                    create_admin(current_user); // Function to create a new admin account
                    log_admin_action(current_user, "Created new admin account"); // Log the action
                    break;
                case 11:
                    view_admin_logs(); // Function to view admin logs
                    log_admin_action(current_user, "Viewed admin logs"); // Log the action
                    break;
                case 0:
                    log_admin_action(current_user, "Logged out"); // Log logout action
                    printf("%s%sLogging out... Goodbye! Hope to see you again soon!%s\n", BOLD, GREEN, RESET);
                    return 0; // Exit the program
                default:
                    printf("%s%sInvalid choice. Please try again.%s\n", BOLD, RED, RESET);
            }
        } else { // If user is a client
            display_client_menu(); // Display client menu
            
            printf("\n%s%sEnter your choice:%s ", BOLD, CYAN, RESET);
            
            // Read user's choice from the menu
            if (scanf("%d", &choice) != 1) {
                printf("%s%sInvalid input! Please enter a number.%s\n", BOLD, RED, RESET);
                
                while (getchar() != '\n'); // Clear input buffer if invalid
                continue; // Start over
            }
            
            // Handle user choice based on the client menu
            switch (choice) {
                case 1:
                    display_routes(); // Function to display all routes
                    break;
                case 2:
                    filter_routes_by_location(); // Function to filter routes by location
                    break;
                case 3:
                    filter_routes_by_vehicle(); // Function to filter routes by vehicle type
                    break;
                case 4:
                    display_schedules(); // Function to display schedules
                    break;
                case 5:
                    make_booking(current_user); // Function to make a booking
                    break;
                case 6:
                    cancel_booking(current_user); // Function to cancel a booking
                    break;
                case 7:
                    view_bookings(current_user); // Function to view current user's bookings
                    break;
                case 0:
                    printf("%s%sLogging out... Goodbye! Hope to see you again soon!%s\n", BOLD, GREEN, RESET);
                    return 0; // Exit the program
                default:
                    printf("%s%sInvalid choice. Please try again.%s\n", BOLD, RED, RESET);
            }
        }
    }

    return 0; // End of the main function
}