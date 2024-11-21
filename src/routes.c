#include <stdio.h>        // For input and output
#include <stdlib.h>       // For exit function
#include <string.h>       // For string manipulation

#define MAX_ROUTE_NAME 50
#define MAX_VEHICLE_TYPE 20

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

// Structure for a route
struct Route {
    int id;
    char start_point[MAX_ROUTE_NAME];
    char end_point[MAX_ROUTE_NAME];
    char vehicle_type[MAX_VEHICLE_TYPE]; // "AC Bus", "Non-AC Bus", "AC Taxi", "Non-AC Taxi"
    int available_seats; // For buses
    int available_vehicles; // For taxis
    float fare;
    int is_active;
    struct Route* next;
};

struct Route* head = NULL;
int route_count = 0;

// Function to save routes to file
void save_routes_to_file() {
    FILE* file = fopen("routes.txt", "w");
    if (file == NULL) {
        printf("%s%sError opening file!%s\n", BOLD, RED, RESET);
        return;
    }

    // Create a temporary array to store routes for sorting
    struct Route* routes[100]; // Assuming max 100 routes
    int count = 0;
    
    struct Route* current = head;
    while (current != NULL) {
        if (current->is_active) {
            routes[count++] = current;
        }
        current = current->next;
    }

    // Sort routes by ID in ascending order
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (routes[j]->id > routes[j + 1]->id) {
                struct Route* temp = routes[j];
                routes[j] = routes[j + 1];
                routes[j + 1] = temp;
            }
        }
    }

    // Write sorted routes to file
    for (int i = 0; i < count; i++) {
        fprintf(file, "%d|%s|%s|%s|%d|%d|%.2f|%d\n",
            routes[i]->id,
            routes[i]->start_point,
            routes[i]->end_point,
            routes[i]->vehicle_type,
            routes[i]->available_seats,
            routes[i]->available_vehicles,
            routes[i]->fare,
            routes[i]->is_active);
    }
    fclose(file);
}

// Function to load routes from file
void load_routes_from_file() {
    FILE* file = fopen("routes.txt", "r");
    if (file == NULL) {
        printf("%s%sError opening routes file!%s\n", BOLD, RED, RESET);
        return;
    }

    // Clear existing list
    while (head != NULL) {
        struct Route* temp = head;
        head = head->next;
        free(temp);
    }
    route_count = 0;

    char line[256];
    struct Route* prev = NULL;
    
    while (fgets(line, sizeof(line), file)) {
        struct Route* new_route = (struct Route*)malloc(sizeof(struct Route));
        if (new_route == NULL) {
            printf("%s%sMemory allocation failed!%s\n", BOLD, RED, RESET);
            fclose(file);
            return;
        }

        if (sscanf(line, "%d|%[^|]|%[^|]|%[^|]|%d|%d|%f|%d",
            &new_route->id,
            new_route->start_point,
            new_route->end_point,
            new_route->vehicle_type,
            &new_route->available_seats,
            &new_route->available_vehicles,
            &new_route->fare,
            &new_route->is_active) != 8) {
            
            free(new_route);
            continue;
        }

        // Insert in ascending order
        new_route->next = NULL;
        if (prev == NULL) {
            head = new_route;
        } else {
            prev->next = new_route;
        }
        prev = new_route;

        if (new_route->id > route_count) {
            route_count = new_route->id;
        }
    }
    fclose(file);
}

// Function to add a new route
void add_route() {
    // Load latest routes from file
    load_routes_from_file();
    
    struct Route* new_route = (struct Route*)malloc(sizeof(struct Route));
    if (new_route == NULL) {
        printf("%s%sMemory allocation failed!%s\n", BOLD, RED, RESET);
        return;
    }

    new_route->id = ++route_count;
    new_route->is_active = 1;
    
    // Clear input buffer
    while (getchar() != '\n');
    
    printf("%s%sEnter start point (within Bengaluru):%s\n", BOLD, CYAN, RESET);
    if (fgets(new_route->start_point, MAX_ROUTE_NAME, stdin) == NULL) {
        printf("%s%sError reading input!%s\n", BOLD, RED, RESET);
        free(new_route);
        return;
    }
    new_route->start_point[strcspn(new_route->start_point, "\n")] = 0;
    
    printf("%s%sEnter end point (within Bengaluru):%s\n", BOLD, CYAN, RESET);
    if (fgets(new_route->end_point, MAX_ROUTE_NAME, stdin) == NULL) {
        printf("%s%sError reading input!%s\n", BOLD, RED, RESET);
        free(new_route);
        return;
    }
    new_route->end_point[strcspn(new_route->end_point, "\n")] = 0;

    printf("%s%sSelect vehicle type:%s\n", BOLD, YELLOW, RESET);
    printf("%s1.%s AC Bus\n", BOLD, RESET);
    printf("%s2.%s Non-AC Bus\n", BOLD, RESET);
    printf("%s3.%s AC Taxi\n", BOLD, RESET);
    printf("%s4.%s Non-AC Taxi\n", BOLD, RESET);
    int choice;
    scanf("%d", &choice);
    while (getchar() != '\n');

    switch(choice) {
        case 1:
            strcpy(new_route->vehicle_type, "AC Bus");
            printf("%s%sEnter number of seats available:%s\n", BOLD, CYAN, RESET);
            scanf("%d", &new_route->available_seats);
            new_route->available_vehicles = 0;
            break;
        case 2:
            strcpy(new_route->vehicle_type, "Non-AC Bus");
            printf("%s%sEnter number of seats available:%s\n", BOLD, CYAN, RESET);
            scanf("%d", &new_route->available_seats);
            new_route->available_vehicles = 0;
            break;
        case 3:
            strcpy(new_route->vehicle_type, "AC Taxi");
            new_route->available_seats = 0;
            printf("%s%sEnter number of taxis available:%s\n", BOLD, CYAN, RESET);
            scanf("%d", &new_route->available_vehicles);
            break;
        case 4:
            strcpy(new_route->vehicle_type, "Non-AC Taxi");
            new_route->available_seats = 0;
            printf("%s%sEnter number of taxis available:%s\n", BOLD, CYAN, RESET);
            scanf("%d", &new_route->available_vehicles);
            break;
        default:
            printf("%s%sInvalid choice!%s\n", BOLD, RED, RESET);
            free(new_route);
            return;
    }

    printf("%s%sEnter fare amount:%s\n", BOLD, CYAN, RESET);
    scanf("%f", &new_route->fare);
    while (getchar() != '\n');

    // Insert in ascending order
    struct Route* current = head;
    struct Route* prev = NULL;

    while (current != NULL && current->id < new_route->id) {
        prev = current;
        current = current->next;
    }

    if (prev == NULL) {
        new_route->next = head;
        head = new_route;
    } else {
        new_route->next = current;
        prev->next = new_route;
    }
    
    save_routes_to_file();
    printf("%s%sRoute added successfully!%s\n\n", BOLD, GREEN, RESET);
}

// Function to delete a route
void delete_route() {
    // Load latest routes from file
    load_routes_from_file();
    
    int id;
    printf("%s%sEnter route ID to delete:%s\n", BOLD, CYAN, RESET);
    if (scanf("%d", &id) != 1) {
        printf("%s%sInvalid input!%s\n", BOLD, RED, RESET);
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    struct Route* current = head;
    while (current != NULL) {
        if (current->id == id && current->is_active) {
            current->is_active = 0;
            save_routes_to_file();
            printf("%s%sRoute deleted successfully!%s\n\n", BOLD, GREEN, RESET);
            return;
        }
        current = current->next;
    }
    printf("%s%sRoute not found!%s\n\n", BOLD, RED, RESET);
}

// Function to display available routes
void display_routes() {
    // Load latest routes from file
    load_routes_from_file();
    
    int active_routes = 0;
    printf("\n%s%sAvailable Routes in Bengaluru:%s\n", BOLD, YELLOW, RESET);
    printf("%s╔══════╦════════════════════╦════════════════════╦═══════════════╦════════════╦════════════╗%s\n", BOLD, RESET);
    printf("%s║ %-4s ║ %-18s ║ %-18s ║ %-13s ║ %-10s ║ %-10s ║%s\n",
           BOLD, "ID", "From", "To", "Type", "Available", "Fare(₹)   ", RESET);
    printf("%s╠══════╬════════════════════╬════════════════════╬═══════════════╬════════════╬════════════╣%s\n", BOLD, RESET);
    
    struct Route* current = head;
    while (current != NULL) {
        if (current->is_active) {
            printf("║ %-4d ║ %-18s ║ %-18s ║ %-13s ║ %-10d ║ ₹%-9.2f ║\n",
                current->id,
                current->start_point,
                current->end_point,
                current->vehicle_type,
                strstr(current->vehicle_type, "Bus") ? current->available_seats : current->available_vehicles,
                current->fare);
            active_routes++;
        }
        current = current->next;
    }

    if (active_routes == 0) {
        printf("║ %s%-89s%s ║\n", BOLD, "No routes available!", RESET);
    }
    printf("%s╚══════╩════════════════════╩════════════════════╩═══════════════╩════════════╩════════════╝%s\n", BOLD, RESET);
    printf("\n");
}