#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ROUTE_NAME 50

struct Route {
    int id;
    char start_point[MAX_ROUTE_NAME];
    char end_point[MAX_ROUTE_NAME];
    int is_active;
    struct Route* next;
};

struct Route* head = NULL;
int route_count = 0;

void save_routes_to_file() {
    FILE* file = fopen("routes.txt", "w");
    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }

    struct Route* current = head;
    while (current != NULL) {
        if (current->is_active) {
            fprintf(file, "%d|%s|%s|%d\n", 
                current->id,
                current->start_point,
                current->end_point,
                current->is_active);
        }
        current = current->next;
    }
    fclose(file);
}

void load_routes_from_file() {
    FILE* file = fopen("routes.txt", "r");
    if (file == NULL) {
        return; // File doesn't exist yet
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        struct Route* new_route = (struct Route*)malloc(sizeof(struct Route));
        sscanf(line, "%d|%[^|]|%[^|]|%d", 
            &new_route->id,
            new_route->start_point,
            new_route->end_point,
            &new_route->is_active);
        
        new_route->next = head;
        head = new_route;
        if (new_route->id > route_count) {
            route_count = new_route->id;
        }
    }
    fclose(file);
}

void add_route() {
    struct Route* new_route = (struct Route*)malloc(sizeof(struct Route));
    if (new_route == NULL) {
        printf("Memory allocation failed!\n");
        return;
    }

    new_route->id = ++route_count;
    new_route->is_active = 1;
    
    // Clear input buffer
    while (getchar() != '\n');
    
    printf("Enter start point:\n");
    if (fgets(new_route->start_point, MAX_ROUTE_NAME, stdin) == NULL) {
        printf("Error reading input!\n");
        free(new_route);
        return;
    }
    new_route->start_point[strcspn(new_route->start_point, "\n")] = 0;
    
    printf("Enter end point:\n");
    if (fgets(new_route->end_point, MAX_ROUTE_NAME, stdin) == NULL) {
        printf("Error reading input!\n");
        free(new_route);
        return;
    }
    new_route->end_point[strcspn(new_route->end_point, "\n")] = 0;

    new_route->next = head;
    head = new_route;
    
    save_routes_to_file();
    printf("Route added successfully!\n\n");
}

void delete_route() {
    int id;
    printf("Enter route ID to delete:\n");
    if (scanf("%d", &id) != 1) {
        printf("Invalid input!\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    struct Route* current = head;
    while (current != NULL) {
        if (current->id == id && current->is_active) {
            current->is_active = 0;
            save_routes_to_file();
            printf("Route deleted successfully!\n\n");
            return;
        }
        current = current->next;
    }
    printf("Route not found!\n\n");
}

void display_routes() {
    int active_routes = 0;
    printf("\nAvailable Routes:\n");
    printf("ID\tStart Point\tEnd Point\n");
    printf("----------------------------------------\n");
    
    struct Route* current = head;
    while (current != NULL) {
        if (current->is_active) {
            printf("%d\t%s\t%s\n", 
                current->id,
                current->start_point, 
                current->end_point);
            active_routes++;
        }
        current = current->next;
    }

    if (active_routes == 0) {
        printf("No routes available!\n");
    }
    printf("\n");
}