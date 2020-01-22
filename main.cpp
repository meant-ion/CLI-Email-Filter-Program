#include <iostream>
#include <fstream>
#include <string>

#include "ourvector.h"

using namespace std;

//listen, I know this is kinda ugly, but this was the only way I could get one function to handle 4 possible cases at once without some complicated/hard-for-me-to-read bullshit caca-dookie bumblefcuk nonsense code
//parses the text spat into it to get the needed information to do the correct checks in binary search, without having to expand my code more than it already is
string ParseText(string parseThis, int domainCheck) {
    string newString;
    size_t foundChar;
    if (domainCheck == 0) {//for getting the domain of the user's input 
       foundChar = parseThis.find('@');
       newString = parseThis.substr(foundChar + 1);
    }
    if (domainCheck == 1) {//for the domain of the currently selected string of the Binary Search
       foundChar = parseThis.find(':');
       newString = parseThis.substr(0, foundChar); 
    }
    if (domainCheck == 2) {//for the user name of the user's input, if it exists
        foundChar = parseThis.find('@');
        newString = parseThis.substr(0, foundChar);
    }
    if (domainCheck == 3) {//for getting the username from the spamlist; for help checking if its a spam username
        foundChar = parseThis.find(':');
        newString = parseThis.substr(foundChar + 1);
    }
    return newString;
}

//Function Name makes what this does obvious
bool BinarySearch (string usrDomain, string usrName, ourvector<string>& emails) {
    //mid, low, and high values for the search
    int low = 0;
    int high = emails.size() - 1;
    int midpoint = low + (high - low) / 2;
    
     while (high >= low ) {
         //strings made from being passed into the function during the search
         string compMid = ParseText(emails.at(midpoint), 1);
         string bannedUsrName = ParseText(emails.at(midpoint), 3);
         
        //checks to see if it has been found
        if (compMid > usrDomain) {
            high = midpoint - 1;
        } else if (compMid < usrDomain) {
            low = midpoint + 1;
        } else if (compMid == usrDomain) {//the domain has been found in the spamlist, so we check the usernames first
            if (bannedUsrName == "*") {//determines the usernames being the same; fucking thing has to be exactly here or it breaks hardcore and I hate my life for not seeing this sooner goddamnit
                return true;
            } else if (usrName > bannedUsrName) {
                low = midpoint + 1;
            } else if (usrName < bannedUsrName) {//the usernames are the same
                high = midpoint - 1;
            } else if (usrName == bannedUsrName) {//the username in the spamlist is a star, meaning all emails with that domain are spam
                return true;
            }
        }

        midpoint = low + (high - low) / 2;
    }

	return false;//wasnt found by the algorithm, so returns false for not found
}

//opens the spam file specified by user and stuffs it into a vector to be used later
void LoadSpam(string file, ourvector<string>& emailsVector) {

    ifstream chosenFile;
    chosenFile.open(file);
    
    string singleLine;
    
    if (emailsVector.size() != 0) {
        emailsVector.clear();
    }
    
    if (!chosenFile.is_open()) {
        cout << "**Error, unable to open " << "'" << file << "'" << endl;
    } else {
        
        chosenFile >> singleLine;
           
        while (!chosenFile.eof()) {
            emailsVector.push_back(singleLine);
            chosenFile >> singleLine;
        }
        cout << "Loading '" << file << "'" << endl;
        cout << "# of spam entries: " << emailsVector.size() << endl;
    }
    
    chosenFile.close();
}

//Checks for the wildcard character so we dont run into issues down the road
bool CheckForWildCard (string userDomain, string userName, ourvector<string>& emails) {
    bool wordFound = BinarySearch(userDomain, "*", emails);
        if (wordFound == false) {
            wordFound = BinarySearch(userDomain, userName, emails);
        }
    return wordFound;
}

//opens the emails file, compares each address in the source to the spamlist, and spits it to an output file if it isnt spam
void FilterFile(string sourceFile, ourvector<string>& spamList, string outputFileName) {
    ifstream chosenFile;
    ofstream outputFile;
    chosenFile.open(sourceFile);
    outputFile.open(outputFileName);
    
    //All three vars are initialized to garbage data, so I can tell they actually get assigned their separate values
    int singleMSGID = -1;
    string singleEmailAddress = "";
    string singleEmailSubject = "";
    int outputLinesCounter = 0;
    int totalLinesProcessed = 0;
    //if the email file or the output file cant be opened
    if (!chosenFile.is_open()) {
        cout << "**Error, unable to open " << "'" << sourceFile << "'" << endl;
    } else if (!outputFile.good()){
        cout << "**Error, unable to open " << "'" << outputFileName << "'" << endl;
    } else {
        
        chosenFile >> singleMSGID;
        chosenFile >> singleEmailAddress;
        getline(chosenFile, singleEmailSubject);
        
        //getting the strings typed by the user to be used during the search
        string userDomain = ParseText(singleEmailAddress, 0);
        string userName = ParseText(singleEmailAddress, 2);
        
        bool wordFound = CheckForWildCard(userDomain, userName, spamList);

        while (!chosenFile.eof()) {
            if (wordFound == false) {//the email adress passed to the binary search wasn't a spam address, so it goes into the output file
                outputFile << singleMSGID << " " << singleEmailAddress << " " << singleEmailSubject << endl;
                outputLinesCounter++;
            }
            
            chosenFile >> singleMSGID;
            chosenFile >> singleEmailAddress;
            getline(chosenFile, singleEmailSubject);
            
            userDomain = ParseText(singleEmailAddress, 0);
            userName = ParseText(singleEmailAddress, 2);
        
            wordFound = CheckForWildCard(userDomain, userName, spamList);
            
            totalLinesProcessed++;
            
        }
        cout << "# emails processed: " << totalLinesProcessed << endl;
        cout << "# non-spam emails:  " << outputLinesCounter << endl;
    }
    
    chosenFile.close();
    outputFile.close();
}

//meat and potatoes of the program
int main() {
    string command;
    ourvector<string> emails;
    
    cout << "** Welcome to spam filtering app **" << endl;
    cout << endl;
    cout << "Enter command or # to exit> ";
    
    cin >> command;
    string file;//for simplicitiy's sake, this var is called file b/c I'm too lazy to create multiple vars for every command, even if they arent a file name being input
    
    while (command != "#") {
      if (command == "load") {//loads a file, preferrably a spamlist
          
            cin >> file;
            LoadSpam(file, emails);
            cout << endl;
     
        } else if (command ==  "display") {//to display the contents of the loaded file
          
            for (int i = 0; i < emails.size(); i++) {
                cout << emails.at(i) << endl;
            }
            cout << endl;
          
        } else if (command == "check") {//checks a domain to see if its in the spam list
          
            cin >> file;
            //getting the strings typed by the user to be used during the search
            string userDomain = ParseText(file, 0);
            string userName = ParseText(file, 2);
            bool wordFound = CheckForWildCard(userDomain, userName, emails);
            if (wordFound == true) {//email has been found in the spamlist
                cout << file << " is spam" << endl;
            } else {//email wasnt found in the spamlist
                cout << file << " is not spam" << endl;
            }
            cout << endl;
          
        } else if (command == "filter") {//takes an email file, filters its content through binary search, and slaps it into an output file
          
          string fileToBeFiltered;
          string outputFileName;
          cin >> fileToBeFiltered;
          cin >> outputFileName;
          FilterFile(fileToBeFiltered, emails, outputFileName);
          
        } else {
          cout << "**invalid command" << endl;
          cout << endl;
        }
            
        cout << "Enter command or # to exit> ";
        cin >> command;
    }  
    
    return 0;
}