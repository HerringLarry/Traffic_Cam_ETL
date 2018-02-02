//
//  main.cpp
//  CSVFileConverter
//
//  Created by William Newman on 6/20/17.
//  Copyright © 2017 William Newman. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>      // std::istringstream
#include<stdlib.h>               //for exit()
//#include "apstring.cpp"
#include <sqlite3.h>
#include "HTTPDownloader.h"
#include <ctime>

using namespace std;

class csvConvert2{
private:
    string * oneDataPoint = new string[14]; //For storing all 14 members of a single data point
    string * cN = new string[14]; //For storing the column names at top of csv
    ifstream csvFile;
    ofstream createCSV;
    ofstream sqlCommands;
    string check;
    string commands;
    string downloaded;
    HTTPDownloader fileDownload = *new HTTPDownloader(); // To download file
    time_t now = time(0);
    tm *ltm = localtime(&now);
    
public:
    csvConvert2(){
        downloaded = fileDownload.download("http://207.251.86.229/nyc-links-cams/LinkSpeedQuery.txt");
        std::ofstream out("/Users/williamnewman/Desktop/Research/Project 1/transition.txt");
        out << downloaded;
        csvFile.open("/Users/williamnewman/Desktop/Research/Project 1/transition.txt");
        sqlCommands.open("/Users/williamnewman/Desktop/Research/Project 1/sqlCommands4.txt");
        out.close();
        if (!csvFile) {
            cerr << "Unable to open file datafile.txt";
            exit(1);   // call system to stop
        }
        convert();
        
    }
    
    char * accessCommands(){
        //commands = storeInString();
        //cout << commands << endl;
        char sqlConversion[100000];
        strcpy(sqlConversion, commands.c_str());
        return sqlConversion;
    }
    
    static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
        int i;
        for(i = 0; i<argc; i++) {
            printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        }
        printf("\n");
        return 0;
    }
    
    //Converts csv file into sql commands
    void convert(){
        storecN();
        storeData();
        storeInString();
        sqlCommands.close();
        
        
    }
    
    void storecN(){
        
        
        
        
        for(int i = 0; i < 13; i++){
            char a[1000];
            char throwaway[1000];
            csvFile.getline(throwaway, 1000, '"');
            csvFile.getline(a,1000,'"');
            string temp(a);
            cN[i] = temp;
            
            
        }
        commands = commands + "CREATE TABLE foo5(";
        commands = commands + cN[0] + " VARCHAR(5) NOT NULL ";
        for(int i = 1; i < 13; i++){
            commands = commands + "," + cN[i] + " VARCHAR(255) NOT NULL";
        }
        commands = commands + ");";
    }
    
    void storeData(){
        do {
            string * temp = new string[13];
            bool date = true;
            
            for(int i = 0; i < 13; i++){
                char a[1000];
                char throwaway[1000];
                csvFile.getline(throwaway, 1000, '"');
                csvFile.getline(a,1000,'"');
                temp[i] = a;
            }
            
            int a = 0;
            int slash = 0;
            string str;
            
            //Puts date into a separate string
            while(slash < 2 && a < 6){
                if(temp[4][a] == '/'){
                    ++slash;
                }
                str = str + temp[4][a];
                
                a++;
            }
            for(int i = 0; i < 4; i++){
                str = str + temp[4][a+i];
                
            }
            
            date = dateCheck(str);
            
            //dateCheck(str);
            if(date  == true){
                
                commands = commands + "INSERT INTO foo5(" + cN[0] + "," + cN[1] + "," + cN[2] + "," + cN[3] + "," + cN[4] + "," + cN[5] + "," + cN[6] + "," + cN[7] + "," + cN[8] + "," + cN[9] + "," + cN[10] + "," + cN[11] + "," + cN[12] + ") VALUES(";
                for(int i = 0; i < 13; i++){
                    if(i != 12){
                        commands = commands + "'" + temp[i] + "',";
                    }
                    else{
                        commands = commands +  "'" + temp[i] + "'";
                    }
                }
                commands = commands + ");";
                
            }
           
        } while(!csvFile.eof());
        //std::cout << commands << endl;
    }
    
    
    string storeInString(){
        std::stringstream buffer;
        
        buffer << sqlCommands.rdbuf();
        std::string str = buffer.str();
        return str;
    }
    
    
    void c0ut(){
        for(int i = 0; i < 14; i++){
            cout << cN[i] << ' ' << endl;
        }
    }
    
    bool dateCheck(string date){
        int year = 1900 + ltm->tm_year;
        int day = ltm->tm_mday;
        int month = 1 + ltm->tm_mon;
        std::string y = std::to_string(year);
        std::string d = std::to_string(day);
        std::string m = std::to_string(month);
        string currDate = m + "/" + d + "/" + y;
        std::cout << currDate << "CCDD" << endl;
        if(currDate == date){
            return true;
        }
        return false;
        
    }
};

int main(){
    csvConvert2 a;
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char * sql;
    
    /* Open database */
    rc = sqlite3_open("/Users/williamnewman/new10.db",&db); //Change path for personal use
    
    if( rc ) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return(0);
    } else {
        fprintf(stderr, "Opened database successfully\n");
    }
    
    /* Create SQL statement */
    sql = a.accessCommands();
    std::cout << sql << std::endl;
    
    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql, a.callback, 0, &zErrMsg);
    
    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        fprintf(stdout, "Records created successfully\n");
    }
    sqlite3_close(db);
    return 0;
    
    
    
    
    
}
