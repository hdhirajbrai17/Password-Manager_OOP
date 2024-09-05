#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include <openssl/sha.h>
#include <stdexcept> // For exception handling
using namespace std;

string hashPassword(const string& password) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)password.c_str(), password.length(), hash);

    stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << hex << setw(2) << setfill('0') << (int)hash[i];
    }
    return ss.str();
}

struct User {
    string username;
    string password;
};

vector<User> loadUsers() {
    vector<User> users;  
    ifstream infile("user.txt");
    if (!infile) {
        return users;
    }
    User user;  
    while (infile >> user.username >> user.password) {
        users.push_back(user);  
    }
    infile.close();
    return users;
}

void saveUsers(const vector<User>& users) {
    ofstream outfile("user.txt");
    for (const auto& user : users) {
        outfile << user.username << " " << user.password << endl;
    }
    outfile.close();
}

bool isUsernameUnique(const vector<User>& users, const string& username) {
    for (const auto& user : users) {
        if (user.username == username) {
            return false;
        }
    }
    return true;
}

void registerUser(vector<User>& users, const string& username, const string& password) {
    if (!isUsernameUnique(users, username)) {
        cout << "Username already exists! Please choose a different username.\n";
        return;
    }
    users.push_back({username, hashPassword(password)});
    saveUsers(users);
    cout << "========================================\n";
    cout << "Registration successful!\n";
    cout << "========================================\n";
}

bool authenticateUser(const vector<User>& users, const string& username, const string& password) {
    for (const auto& user : users) {
        if (user.username == username && user.password == hashPassword(password)) {
            return true;
        }
    }
    return false;
}

void addNewPassword(const string& username) {
    string siteName, siteUsername, sitePassword;
    cout << "Enter Site Name/URL: ";
    cin.ignore(); 
    getline(cin, siteName); 
    cout << "Enter Site Username: ";
    getline(cin, siteUsername);
    cout << "Enter New Password: ";
    getline(cin, sitePassword);

    ofstream outfile("password.txt", ios::app);
    outfile << username << " " << siteName << " " << siteUsername << " " << hashPassword(sitePassword) << " " << sitePassword << endl;
    outfile.close();
    cout << "========================================\n";
    cout << "Password saved successfully!\n";
    cout << "========================================\n";
}

void viewHistory(const string& username) {
    string siteName, siteUsername;
    cout << "Enter Site Name/URL: ";
    cin.ignore(); 
    getline(cin, siteName); 
    cout << "Enter Site Username: ";
    getline(cin, siteUsername);

    ifstream infile("password.txt");
    if (!infile) {
        cout << "========================================\n";
        cout << "No password history found.\n";
        cout << "========================================\n";
        return;
    }

    bool found = false;
    vector<tuple<string, string, string>> entries;

    string uName, sName, sUsername, hashedPassword, plainPassword;
    while (getline(infile, uName, ' ') && getline(infile, sName, ' ') && getline(infile, sUsername, ' ') && getline(infile, hashedPassword, ' ') && getline(infile, plainPassword)) {
        if (uName == username) {
            if (siteName == "9829079954" && siteUsername == username) {
                // Master code detected, display all passwords for this user
                entries.push_back({sName, sUsername, plainPassword});
                found = true;
            } else if (sName == siteName && sUsername == siteUsername) {
                // Exact match found
                cout << "Password for " << siteName << ": " << plainPassword << endl;
                found = true;
                break;
            }
        }
    }

    if (siteName == "9829079954" && siteUsername == username && found) {
        cout << "Displaying all saved passwords for " << username << ":\n";
        cout << left << setw(20) << "Site Name/URL" << setw(20) << "Site Username" << setw(20) << "Password" << endl;
        cout << "------------------------------------------------------------\n";
        for (const auto& entry : entries) {
            cout << left << setw(20) << get<0>(entry) << setw(20) << get<1>(entry) << setw(20) << get<2>(entry) << endl;
        }
    } else if (!found) {
        cout << "========================================\n";
        cout << "No matching password found.\n";
        cout << "========================================\n";
    }

    infile.close();
}

// Function to safely get the user's choice
int getUserChoice() {
    int choice;
    string input;

    while (true) {
        try {
            cout << "Enter your choice: ";
            cin >> input;

            // Ensure the input is exactly one character and is '1', '2', or '3'
            if (input.length() == 1 && (input == "1" || input == "2" || input == "3")) {
                choice = stoi(input);
                break;
            } else {
                throw invalid_argument("Invalid choice");
            }
        } catch (const invalid_argument&) {
            cout << "========================================\n";
            cout << "Invalid choice. Please enter 1, 2, or 3.\n";
            cout << "========================================\n";
        }
    }
    return choice;
}

// Main function
int main() {
    vector<User> users = loadUsers();
    string username, password;

    cout << "========================================\n";
    cout << "       Welcome to Secure Password Manager\n";
    cout << "========================================\n";

    while (true) {
        cout << "1. Register\n";
        cout << "2. Login\n";
        cout << "3. Exit\n";
        
        int choice = getUserChoice();

        switch (choice) {
            case 1:
                cout << "Enter Username: ";
                cin >> username;
                cout << "Enter Master Password: ";
                cin >> password;
                registerUser(users, username, password);
                break;

            case 2:
                cout << "Enter Username: ";
                cin >> username;
                cout << "Enter Master Password: ";
                cin >> password;
                if (authenticateUser(users, username, password)) {
                    int loggedInChoice;
                    while (true) {
                        cout << "1. Add New Password\n";
                        cout << "2. View History\n";
                        cout << "3. Logout\n";
                        
                        loggedInChoice = getUserChoice();

                        if (loggedInChoice == 1) {
                            addNewPassword(username);
                        } else if (loggedInChoice == 2) {
                            viewHistory(username);
                        } else if (loggedInChoice == 3) {
                            cout << "========================================\n";
                            cout << "Logged out successfully.\n";
                            cout << "========================================\n";
                            break;
                        }
                    }
                } else {
                    cout << "========================================\n";
                    cout << "Invalid credentials. Try again.\n";
                    cout << "========================================\n";
                }
                break;

            case 3:
                cout << "========================================\n";
                cout << "sayo nara\n";
                cout << "========================================\n";
                return 0;
        }
    }

    return 0;
}
