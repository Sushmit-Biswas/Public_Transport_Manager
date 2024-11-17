#ifndef AUTH_H
#define AUTH_H

#ifdef __cplusplus
extern "C" {
#endif

// Function declarations for authentication
int authenticate(void);
int admin_login(void);
int client_login(void); 
void client_register(void);
int validate_client(char *username, char *password);
void save_client(char *username, char *password);

#ifdef __cplusplus
}
#endif

#endif // AUTH_H
