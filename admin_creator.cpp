
#include <iostream>
#include <string>
#include <fstream>
using namespace std;
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
int main(){
    ofstream file("auth_pass.txt");
    string pass = "papahoon";
    string en_pass = encryptDecrypt(pass);
    file << en_pass;

    return 0;
}
