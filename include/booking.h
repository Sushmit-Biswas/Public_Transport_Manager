#ifndef BOOKING_H
#define BOOKING_H

#define MAX_BOOKINGS 1000
#define MAX_USERNAME 50

// Structure definition for Booking
struct Booking {
    int id;
    char username[MAX_USERNAME];
    int route_id;
    int schedule_id;
    int num_seats;    // For bus bookings
    int num_taxis;    // For taxi bookings
    float total_fare;
    int is_active;    // 1 for active, 0 for cancelled
};

// Function declarations for booking management
void make_booking(char* username);  // Creates a new booking for the given user
void cancel_booking(char* username); // Cancels an active booking for the user
void view_bookings(char* username); // Displays all bookings for the user
void save_bookings_to_file(void);   // Saves bookings data to file
void load_bookings_from_file(void); // Loads bookings data from file
void update_fare(int is_admin);     // Updates fare for a route (admin only)

// Global variables
extern struct Booking bookings[MAX_BOOKINGS]; // Array to store all bookings
extern int booking_count;                     // Current number of bookings

#endif // BOOKING_H
