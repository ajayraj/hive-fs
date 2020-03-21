#ifndef __ENTRY_H_
#define __ENTRY_H_


#include <iostream>
#include <ctime>
#include <vector>
#include <string>
#include <sstream>

//  Reference if implementing templated types: https://coliru.stacked-crooked.com/a/ebac7c124560e6e7

//Most written functions that are being passed an Entry will be passed an Entry* entry_struct
struct Entry {
    std::string uid; //Unique Identifier 64 bits
    std::string parent_uid;  //Can try to automatically assign on construction
    std::string path;   //Filepath
    std::string name;   //File name minus path
    std::string value; //Thinking of doing struct template typing to make this variable for Dir...
    //Originally planned on setting value to -1 if deleted, bool could be more functional? Remains to see in implementation
    bool exists;    //Set false if deleted
    bool is_file;   //Set true if File, false if directory
    std::string final_string;

    std::vector<std::string> entry_data;

    std::vector<std::string> split_str(std::string f) {
        //std::cout << "SPLIT STRING CALLED: " << f << std::endl;
        std::vector<std::string> attr;

        std::istringstream ss(f);
        std::string token;

        while(std::getline(ss, token, '&')) {
            //std::cout << "ITERATING THROUGH...: " << token << std::endl;
            attr.push_back(token);
        }

        std::string attributes = attr.at(0);
        std::istringstream s2(attributes);

        while(std::getline(s2, token, '|')) {
            //std::cout << "ITERATING THROUGH ATTRIBUTES: " << token << std::endl;
            attr.push_back(token);
        }

        attr.erase(attr.begin());
        attr.push_back(attr.at(0));
        attr.erase(attr.begin());

        return attr;
    }

    Entry(){} //Blank Entry constructor

    Entry(std::string uid, std::string value, std::string path) {    //Consider changing uid to string
        this->uid = uid;
        this->path = path;
        entry_data = split_str(value);
        //for (int i = 0; i < entry_data.size(); ++i ) { std::cout << "ENTRY VALUE AT VECTOR " << i << ": " << entry_data.at(i) << std::endl; }
        name = entry_data.at(0);
        parent_uid = entry_data.at(1);
        exists = (entry_data.at(3) == "1") ? true : false;

        if(uid.at(0) == 'f') {
            is_file = true;
        }
        else if (uid.at(0) == 'd') {
            is_file = false;
        }
        else{
            //Error, uid passed in through Entry must always start on either of these. Invalid key
            std::cout << "Major error, just stop what you're doing. \n" << uid << std::endl;
        }

        this->value = entry_data.back();
    }

    /* split_str(f): parses delimited attribute string into vector<string>, returns vector<string>
    * Expected input string format & contents (attributes + contents):
    * File String Input: name|parent_uid|is_file|exists|file_orig|file_last_author|time_orig|time_last_write|size&file_contents
    * Dir String Input: name|parent_uid|is_file|exists|is_root&dir_contents
     * File and Dir contents stored in last index of vector
    */


};


struct File : Entry {
    std::string file_orig; //Can store original node
    std::string file_last_author; //Stores node of last author
    std::string time_orig; //Time created
    std::string time_last_write; //Stores time written to
    std::size_t size;   //Could use size_t? Check FUSE docs

    //Can add more FUSE attributes here, especially from stat
    File() : Entry() {}

    File(std::string uid, std::string value, std::string path)
    : Entry(uid, value, path) { //Code called most likely for read only
        std::cout << "VALUE PASSED TO POPULATE FILE CONSTRUCTOR: " << value << std::endl;
        //Get all these values below from the value string
        file_orig = entry_data.at(4); //Can store original node
        file_last_author = entry_data.at(5); //Stores node of last author
        time_orig = entry_data.at(6); //Time created
        time_last_write = entry_data.at(7); //Stores time written to
        std::cout << "SIZE STORED IN FILE DURING CONSTRUCTOR FROM ENTRY_DATA: " << entry_data.at(8) << std::endl;
        std::istringstream iss(entry_data.at(8));
        iss >> size;

        std::cout << "SIZE : " << size << std::endl;

        final_string = name + "|" + parent_uid + "|" + std::to_string(is_file ? 1 : 0) + "|" + entry_data.at(3) + "|" + file_orig + "|" + file_last_author + "|" + time_orig + "|" + time_last_write + "|" + std::to_string(size) + "&" + this->value;
    }

    File(std::string uid, std::string value, std::string path, std::string node_id)
    : Entry(uid, value, path) { //This constructor can be called on a File write to update node_id to current calling author
        //Checks if File is new, populates info otherwise
        //Resource for time code: https://www.geeksforgeeks.org/c-program-print-current-day-date-time/
        //std::cout << "NODE_ID sanity check:: " << node_id << std::endl;
        std::time_t tt;
        struct tm * ti;
        std::time(&tt);
        ti = std::localtime(&tt);
        char* curr_time = std::asctime(ti);

        std::string file_source = entry_data.at(4);

        if(file_source == "NULL") { //NULL will be passed into the constructed string on first File write by fuse_write probably
            //std::cout << "CALLS FILE_SOURCE = NULL: " << node_id << std::endl;
            file_source = node_id;
            file_orig = file_source;
            time_orig =  curr_time; //Cast curr_time to string if need be
            file_last_author = node_id;
            time_last_write = curr_time;
        }
        else {
            //std::cout << "CALLS FILE_SOURCE = NOT NULL: " << file_source << std::endl;
            file_orig = file_source;
            time_orig = entry_data.at(6);
            file_last_author = node_id;
            time_last_write = curr_time;
        }
        std::cout << "SIZE STORED IN FILE DURING CONSTRUCTOR FROM ENTRY_DATA: " << entry_data.at(8) << std::endl;
        std::istringstream iss(entry_data.at(8));
        iss >> size;
        //size = std::to_string(entry_data.at(8));
        std::cout << "SIZE : " << size << std::endl;

        final_string = name + "|" + parent_uid + "|" + std::to_string(is_file ? 1 : 0) + "|" + entry_data.at(3) + "|" + file_orig + "|" + file_last_author + "|" + time_orig + "|" + time_last_write + "|" + std::to_string(size) + "&" + this->value;
    }

    std::string output_string() {

        final_string = (name + "|" + parent_uid + "|" + std::to_string(is_file ? 1 : 0) + "|" + std::to_string(exists ? 1 : 0) + "|" + file_orig + "|" + file_last_author + "|" + time_orig + "|" + time_last_write + "|" + std::to_string(size) + "&" + this->value);
        return final_string;
    }

};



struct Dir : Entry {
    std::vector<std::string> entries_in_path;   //Can store UID values of entries
    bool is_root;

    Dir() : Entry() {}

    Dir(std::string uid, std::string value, std::string path)
    : Entry(uid, value, path) {
        //std::cout << "ENTRY_DATA_SIZE: " << entry_data.size() << std::endl;
        //for (int i = 0; i < entry_data.size(); ++i){ std::cout << "ENTRYDATAVAL: " << entry_data.at(i) << std::endl; }
        is_root = (entry_data.at(4) == "0") ? false : true;

        std::stringstream ss(this->value);
        std::string s;

        while (ss >> s) { // reads a string from ss; stops at whitespace; returns false when we hit the end of ss
            entries_in_path.push_back(s);
        }

        final_string = name + "|" + parent_uid + "|" + std::to_string(is_file ? 1 : 0) + "|" + entry_data.at(3) + "|" + entry_data.at(4) + "&" + this->value;
    }

    std::string output_string(){
        final_string = (name + "|" + parent_uid + "|" + std::to_string(is_file ? 1 : 0) + "|" + std::to_string(exists ? 1 : 0) + "|" + entry_data.at(4) + "&" + this->value);
        return final_string;
    }

};


//Reference to pointer casting: https://old.reddit.com/r/unrealengine/comments/d40sko/how_do_i_cast_a_parent_struct_to_a_child_struct/f07tu6t/
//Code can check if created Entry is a File or Dir class and populate accordingly
//Upcasting and downcasting: https://www.bogotobogo.com/cplusplus/upcasting_downcasting.php
//Proper base and derived struct pointer and reference usage: https://www.geeksforgeeks.org/object-slicing-in-c/
//Constructors for derived: https://www.learncpp.com/cpp-tutorial/114-constructors-and-initialization-of-derived-classes/
//More derived class constructor info: https://en.cppreference.com/w/cpp/language/derived_class

//What I want to do: when a Entry is created, should be able to just construct struct of Entry.
//Entry creates File or Dir struct object and can populate it automatically.
//Entry should be able to access child functions? Cast appropriately.
#endif