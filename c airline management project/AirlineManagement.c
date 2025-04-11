#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

#define DB_NAME "airline.db"

// Function to initialize the SQLite database
void initialize_database() {
    sqlite3 *db;
    char *err_msg = 0;
    int rc = sqlite3_open(DB_NAME, &db);
    
    if (rc != SQLITE_OK) {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }
    
    // Create tables for Users, Flights, and Bookings
    const char *sql = "CREATE TABLE IF NOT EXISTS Users (username TEXT PRIMARY KEY, password TEXT);"
                      "CREATE TABLE IF NOT EXISTS Flights (flight_no INTEGER PRIMARY KEY, source TEXT, destination TEXT, available_seats INTEGER, price REAL);"
                      "CREATE TABLE IF NOT EXISTS Bookings (username TEXT, flight_no INTEGER, passenger_name TEXT, PRIMARY KEY(username, flight_no));";
    
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        printf("SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    }
    sqlite3_close(db);
}

// Function to insert default flight data
void insert_default_flights() {
    sqlite3 *db;
    sqlite3_open(DB_NAME, &db);
    
    const char *sql = "INSERT OR IGNORE INTO Flights (flight_no, source, destination, available_seats, price) VALUES"
                      "(101, 'Delhi', 'Mumbai', 5, 5500.0),"
                      "(102, 'Bangalore', 'Chennai', 10, 3500.0),"
                      "(103, 'Kolkata', 'Hyderabad', 8, 6000.0),"
                      "(104, 'Pune', 'Jaipur', 7, 4500.0),"
                      "(105, 'Ahmedabad', 'Goa', 6, 5000.0);";
    
    char *err_msg = 0;
    sqlite3_exec(db, sql, 0, 0, &err_msg);
    sqlite3_close(db);
}

// Function to display available flights
void display_flights() {
    sqlite3 *db;
    sqlite3_open(DB_NAME, &db);
    
    const char *sql = "SELECT * FROM Flights;";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {

   printf("\n\n\n\t\tAvailable Flights:\n");
    printf("\n\n\t\tFlight No.\tSource\t\tDestination\tSeats Available\t   Price (INR)\n");
    printf("\t\t-------------------------------------------------------------------------------\n");


        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int flight_no = sqlite3_column_int(stmt, 0);
            const char *source = (const char *)sqlite3_column_text(stmt, 1);
            const char *destination = (const char *)sqlite3_column_text(stmt, 2);
            int available_seats = sqlite3_column_int(stmt, 3);
            double price = sqlite3_column_double(stmt, 4);

            printf("\t\t%-10d | \t%-10s | \t%-10s | \t%-15d | \t%.2f\n", 
                    flight_no, source, destination, available_seats, price);
        }
        sqlite3_finalize(stmt);
    } else {
        printf("Error retrieving flights!\n");
    }
    
    sqlite3_close(db);
}

// Function to register a user
void register_user() {
    sqlite3 *db;
    sqlite3_open(DB_NAME, &db);
    
    char username[50], password[50];
    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter password: ");
    scanf("%s", password);
    
    char sql[200];
    sprintf(sql, "INSERT INTO Users (username, password) VALUES ('%s', '%s');", username, password);
    
    if (sqlite3_exec(db, sql, 0, 0, NULL) == SQLITE_OK) {
        printf("User registered successfully!\n");
    } else {
        printf("Registration failed! Username might already exist.\n");
    }
    sqlite3_close(db);
}

// Function to log in a user
int login_user(char *username) {
    sqlite3 *db;
    sqlite3_open(DB_NAME, &db);
    
    char password[50], db_password[50];
    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter password: ");
    scanf("%s", password);
    
    char sql[200];
    sprintf(sql, "SELECT password FROM Users WHERE username='%s';", username);
    
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK && sqlite3_step(stmt) == SQLITE_ROW) {
        strcpy(db_password, (const char *)sqlite3_column_text(stmt, 0));
        if (strcmp(password, db_password) == 0) {
            printf("Login successful!\n");
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            return 1;
        }
    }
    printf("Invalid username or password!\n");
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0;
}

// Function to book a flight
void book_ticket(char *username) {
    sqlite3 *db;
    sqlite3_open(DB_NAME, &db);
    
    int flight_no, available_seats;
    char passenger_name[50];

    printf("Enter flight number: ");
    scanf("%d", &flight_no);
    printf("Enter passenger name: ");
    scanf("%s", passenger_name);

    // Check available seats
    char check_sql[200];
    sprintf(check_sql, "SELECT available_seats FROM Flights WHERE flight_no = %d;", flight_no);
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(db, check_sql, -1, &stmt, 0) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            available_seats = sqlite3_column_int(stmt, 0);
            if (available_seats <= 0) {
                printf("No seats available for this flight!\n");
                sqlite3_finalize(stmt);
                sqlite3_close(db);
                return;
            }
        }
    }
    sqlite3_finalize(stmt);

    // Book the ticket
    char book_sql[200];
    sprintf(book_sql, "INSERT INTO Bookings (username, flight_no, passenger_name) VALUES ('%s', %d, '%s');", username, flight_no, passenger_name);
    
    if (sqlite3_exec(db, book_sql, 0, 0, NULL) == SQLITE_OK) {
        // Reduce seat count after successful booking
        char update_sql[200];
        sprintf(update_sql, "UPDATE Flights SET available_seats = available_seats - 1 WHERE flight_no = %d;", flight_no);
        sqlite3_exec(db, update_sql, 0, 0, NULL);

        printf("Ticket booked successfully!\n");
    } else {
        printf("Booking failed!\n");
    }
    
    sqlite3_close(db);
}


// Function to cancel a ticket
void cancel_ticket(char *username) {
    sqlite3 *db;
    sqlite3_open(DB_NAME, &db);
    
    int flight_no;
    printf("Enter flight number to cancel booking: ");
    scanf("%d", &flight_no);

    // First, check if the booking exists
    char check_sql[200];
    sprintf(check_sql, "SELECT COUNT(*) FROM Bookings WHERE username='%s' AND flight_no=%d;", username, flight_no);
    
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, check_sql, -1, &stmt, 0) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW && sqlite3_column_int(stmt, 0) == 0) {
            printf("No booking found for this flight!\n");
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            return;
        }
    }
    sqlite3_finalize(stmt);

    // Delete the booking
    char delete_sql[200];
    sprintf(delete_sql, "DELETE FROM Bookings WHERE username='%s' AND flight_no=%d;", username, flight_no);

    if (sqlite3_exec(db, delete_sql, 0, 0, NULL) == SQLITE_OK) {
        // Update available seats count
        char update_sql[200];
        sprintf(update_sql, "UPDATE Flights SET available_seats = available_seats + 1 WHERE flight_no = %d;", flight_no);
        
        if (sqlite3_exec(db, update_sql, 0, 0, NULL) == SQLITE_OK) {
            printf("Ticket cancelled successfully!\n");
        } else {
            printf("Cancellation successful, but failed to update available seats!\n");
        }
    } else {
        printf("Cancellation failed!\n");
    }

    sqlite3_close(db);
}


void view_bookings(char *username) {
    sqlite3 *db;
    sqlite3_open(DB_NAME, &db);
    
    char sql[200];
    sprintf(sql, "SELECT flight_no, passenger_name FROM Bookings WHERE username='%s';", username);
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        printf("\nYour Bookings:\n");
        printf("Flight No. | Passenger Name\n");
        printf("---------------------------\n");
        
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int flight_no = sqlite3_column_int(stmt, 0);
            const char *passenger_name = (const char *)sqlite3_column_text(stmt, 1);
            printf("%-10d | %-15s\n", flight_no, passenger_name);
        }
        sqlite3_finalize(stmt);
    } else {
        printf("Error retrieving bookings!\n");
    }
    sqlite3_close(db);
}

int main() {
    initialize_database();
    insert_default_flights();
    
    int choice;
    char username[50];
    int logged_in = 0;
    
    while (1) {
    	printf("\n\n\t\t ********************************************************************");
        printf("\n\t\t                   WELCOME TO AIRLINE MANAGEMENT SYSTEM               ");
        printf("\n\t\t *******************************************************************");

        printf("\n1. Signup\n2. Login\n3. View Flights\n4. Book Ticket\n5. Cancel Ticket\n6. View Bookings\n7. Exit\nChoose an option: ");
        scanf("%d", &choice);
        
        switch (choice) {
            case 1: register_user(); break;
            case 2: logged_in = login_user(username); break;
            case 3: display_flights(); break;
            case 4: if (logged_in) book_ticket(username); else printf("Please log in first!\n"); break;
            case 5: if (logged_in) cancel_ticket(username); else printf("Please log in first!\n"); break;
            case 6: if (logged_in) view_bookings(username); else printf("Please log in first!\n"); break;
            case 7: exit(0);
            default: printf("Invalid choice!\n");
        }
    }
    return 0;
}












