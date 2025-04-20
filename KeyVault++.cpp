#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <filesystem>
#include <limits>
using namespace std;

string encryptDecrypt(string password);

class passwordEntry
{
private:
    string appName, userName, password;

public:
    passwordEntry(string app, string user, string pass)
    {
        appName = app;
        userName = user;
        password = pass;
    }

    string getAppName() const
    {
        return appName;
    }
    string getUserName() const
    {
        return userName;
    }
    string getPassword() const
    {
        return password;
    }

    void display() const
    {
        cout << "AppName: " << appName << "\nUserName: " << userName << "\nPassword: " << password << "\n\n";
    }
};

class passwordManager
{
private:
    vector<passwordEntry> entries;
    string encryptionKey = "X";

public:
    friend class user_registration_login;
    string encryptDecrypt(const string &data)
    {
        string result = data;
        for (char &c : result)
        {
            c ^= encryptionKey[0];
        }
        return result;
    }
    void addEntry(const passwordEntry &entry)
    {
        entries.push_back(entry);
    }

    void displayAll() const
    {
        for (const auto &entry : entries)
        {
            entry.display();
        }
    }

    void searchByApp(const string &appName) const
    {
        bool found = false;
        for (const auto &entry : entries)
        {
            if (entry.getAppName() == appName)
            {
                entry.display();
                found = true;
                break;
            }
        }
        if (!found)
        {
            cout << "No entry found for: " << appName;
        }
    }

    bool isStrongPassword(const string &password)
    {
        return password.length() >= 8 &&
               any_of(password.begin(), password.end(), ::isdigit) &&
               any_of(password.begin(), password.end(), ::isupper) &&
               any_of(password.begin(), password.end(), ::islower) &&
               any_of(password.begin(), password.end(), ::ispunct);
    }

    void saveToFile(const string &filename)
    {
        ofstream fout(filename);
        for (const auto &entry : entries)
        {
            string encPass = encryptDecrypt(entry.getPassword());
            fout << entry.getUserName() << "," << entry.getAppName() << "," << encPass << "\n";
        }
        fout.close();
    }

    void loadFromFile(const string &filename)
    {
        fstream fin(filename);
        string app, user, pass;
        while (getline(fin, app, ',') && getline(fin, user, ',') && getline(fin, pass))
        {
            string decPass = encryptDecrypt(pass);
            entries.emplace_back(user, app, decPass);
        }
        fin.close();
    }
};

class user_registration_login
{
private:
    string username;
    string password;

public:
    friend class passwordManager;
    void registration()
    {
        cout << "Enter username: ";
        getline(cin, username);
        cout << "Enter password: ";
        getline(cin, password);
        ofstream file("auth.txt", ios::app);
        file << username << " " << encryptDecrypt(password) << endl;
        file.close();
    }
    void login()
    {
        string enter_user;
        string enter_pass;
        cout << "Enter username: ";
        getline(cin, enter_user);
        cout << "Enter password: ";
        getline(cin, enter_pass);

        ifstream ifile("auth.txt");
        string stored_user, stored_encrypted_pass;
        bool authenticated = false;

        while (ifile >> stored_user >> stored_encrypted_pass)
        {
            string decrypted_pass = encryptDecrypt(stored_encrypted_pass);

            if (stored_user == enter_user && decrypted_pass == enter_pass)
            {
                authenticated = true;
                break;
            }
        }

        ifile.close();

        if (authenticated)
        {
            cout << "Login successful. Welcome to CipherVault++!\n";
            // You can now call the password manager menu here
        }
        else
        {
            cout << "Invalid username or password.\n";
        }
    }
};

int main()
{
    passwordManager manager;
    manager.loadFromFile("Passwords.txt");

    int choice;
    string app, user, pass;

    cout << "\n--- Password Manager ---\n";
    while (choice != 0)
    {
        cout << "1. Add Password\n2. Show All\n3. Search\n0. Save & Exit\nYour choice: ";

        while (!(cin >> choice) || choice < 0 || choice > 3)
        {
            cout << "Invalid Choice! Enter a valid choice: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice)
        {
        case 1:
            cout << "App Name: ";
            getline(cin, app);
            transform(app.begin(), app.end(), app.begin(), ::tolower);

            cout << "Username: ";
            getline(cin, user);
            transform(user.begin(), user.end(), user.begin(), ::tolower);
            cout << "Password: ";
            getline(cin, pass);

            while (!manager.isStrongPassword(pass))
            {
                cout << "Weak password! Use uppercase, lowercase, digits, symbols, min 8 chars.\n";
                cout << "Password: ";
                getline(cin, pass);
            }

            manager.addEntry(passwordEntry(app, user, pass));
            break;

        case 2:
            manager.displayAll();
            break;

        case 3:
            cout << "Enter App Name to search: ";
            getline(cin, app);
            transform(app.begin(), app.end(), app.begin(), ::tolower);
            manager.searchByApp(app);
            break;

        case 0:
            manager.saveToFile("Passwords.txt");
            cout << "Data saved. Goodbye!\n";
            break;

        default:
            cout << "Invalid Choice!";
            break;
        }
        cout << endl;
    }

    return 0;
}