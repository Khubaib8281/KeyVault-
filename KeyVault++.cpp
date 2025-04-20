#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <limits>
using namespace std;

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

    string encryptDecrypt(const string &data)
    {
        string result = data;
        for (char &c : result)
        {
            c ^= encryptionKey[0];
        }
        return result;
    }

public:
    bool admin_access()
    {
        string entered_pass;
        bool auth = false;
        cout << "You got 5 attempts\n\n";
        

        ifstream ifile("auth_pass.txt");
        string stored_pass;
        if (ifile.is_open())
        {
            getline(ifile, stored_pass);
            for(int i = 1 ; i <= 5 ; i++){
            if(i == 5){
                cout << "\nLast attempt!\nEnter wisely: ";
            }else{
                cout << "\nAttempt number: " << i << "\n";
            }
            cout << "Enter password: ";
            getline(cin, entered_pass);
            if (encryptDecrypt(stored_pass) == entered_pass)
            {
                auth = true;
                break;
            }
            else
            {
                auth = false;
            }
            }
            if(!auth){
                cout << "Access denied! Exiting...\n";
                return false;
            }else{
                cout << "\nAccess granted!" << endl;
                return true;
            }
            ifile.close();
        }
        else
        {
            cout << "Error opening file!";
            return false;
        }
        return false;
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

int main()
{
    passwordManager manager;
    int choice;
    string app, user, pass;
    manager.loadFromFile("Passwords.txt");

    cout << "\n\t\t\t\t\t---- CipherVault++ ----\n";
    cout << "---- Admin Authentication ----\n";

    if (manager.admin_access())

    {
        cout << "\n\t\t\t\t\t--- Welcome to CipherVault++ ---\n";
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
    }
    return 0;
}