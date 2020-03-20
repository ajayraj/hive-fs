#ifndef __HIVEFS_DHT_H_
#define __HIVEFS_DHT_H_


#include "entry.h"

#include <opendht.h>
#include <unistd.h>
#include <random>
#include <sys/types.h>
#include <sys/stat.h>

#include "Fuse.h"

#include "Fuse-impl.h"

 static dht::DhtRunner node;
 static std::string NODE_ID;


std::string dht_get_entry_contents(std::string key) {
    std::string dht_output;
    // get data from the dht
    node.get(key, [&dht_output](const std::vector<std::shared_ptr<dht::Value>>& values) {
        // Callback called when values are found
        for (const auto& value : values) {
            auto entry_contents = (*value).data;
            std::string s(entry_contents.begin(), entry_contents.end());
            dht_output = s;
        }

        return true; // return false to stop the search
    });

    // wait a little bit for DHT threads
    sleep(1);

    // remove weird "?" character
    //if(dht_output != "") { dht_output.erase(0,1); }

    if(dht_output != "") {
      std::string temp = dht_output.substr(dht_output.find("___") + 3);
      dht_output = temp;
    }

    return dht_output;
}


// Start node, connect node to bootstrap, assign name to node
//WORKING
void hivefs_dht_init_node() {
   // start node and connect to bootstrap node
   node.run(4855, dht::crypto::generateIdentity(), true);
   node.bootstrap("localhost", "4222");



   std::string n = "N";
   unsigned int i = 1;
   // assign first available name to node (N + int [e.g. "N1"])
   while(i != 0){
     std::string name_check = n + std::to_string(i);
     std::string cont = dht_get_entry_contents(name_check);

     node.put("/", "___d1|1");
     sleep(1);
     std::string root_val = "/";
     root_val = root_val + "|" + "d1" + "|" + "0" + "|" + "1" + "|" + "1" + "&" + " ";
     node.put("d1", "___" + root_val);

     // node name = key, value @ key indicates availability
     if (cont == "") {
        node.put(name_check, "___TAKEN!");
        NODE_ID = ""; //Clears NODE_ID again, seems to take on null value added at some point...
        NODE_ID = name_check;
        i = 0;
     } else { ++i; }
   }
}

// Generates random alphanumeric string of specified length. Used by gen_uid(is_dir).
//WORKING
std::string rand_str( size_t length )
{
    auto randchar = []() -> char
    {
        const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[ rand() % max_index ];
    };
    std::string str(length,0);
    std::generate_n( str.begin(), length, randchar );
    return str;
}

// Generates 64 character UUID, first char indicates if it is a file or directory
//WORKING
std::string gen_uid(bool is_dir) {
    std::string uid = rand_str(63);
    if(is_dir) {
      uid.insert(0, "d");
    } else {
      uid.insert(0, "f");
    }
    return uid;
}




/* -------------------------------------------------------------------------------------------
    hive_dht_X() FUNCTIONS:
   ------------------------------------------------------------------------------------------- */
/* 1. NODE_ID_KVS:      Key = node name, Value = TAKEN (or trusted node name for permissions)
 * 2. FILE_PATH_KVS:    Key = path, Value = uid + exists_bit
 * 3. FILE_KVS:         Key = fuid, Value = attributes + contents
 * 4. DIR_KVS:          Key = duid, Value = attributes + contents
 * Check entry.h structs for exact attribute information
 */

size_t find_attr_index(const std::string& attrs, int prev_delim) {
  size_t pos = 0;
  int count = 0;
  while( count != prev_delim) {
    pos += 1;
    pos = attrs.find("|", pos);
    if ( pos == std::string::npos ) { return -1; }
    ++count;
  }
  return pos;
}

//WORKING
int hivefs_dht_mkdir(std::string path) { //Is passed path from hive_fuse_mkdir(path)
    if(path[path.length()-1] != '/') { return -1; }
    //std::cout << std::endl << std::endl << std::endl << "BEGINNING MKDIR FOR: " << path << std::endl;
    std::string path_kvs = dht_get_entry_contents(path);
    //std::cout << "PATH_KVS CONTENTS: " << path_kvs << std::endl;
    sleep(1);
    std::string is_root = "0";    //Implement to activate to properly initialize root? May init root separately to avoid


    if(path_kvs == "" || (path_kvs.substr(path_kvs.find("|") + 1) == "0")) {   //Directory has never existed, was purged from DHT, or was deleted.
        //std::cout << "PASSES IF" << std::endl;
        std::string parent_path = path;
        //std::cout << "PARENT PATH: " << parent_path << std::endl;
        parent_path.pop_back(); //Gets rid of ending slash after name
        //std::cout << "PARENT PATH: " << parent_path << std::endl;
        //std::cout << "PARENT FIND LAST OF: " << parent_path.find_last_of("/") << std::endl;
        //std::cout << "PARENT FIND LAST OF SUB :" << parent_path.substr(0, parent_path.find_last_of("/") + 1) << std::endl;
        parent_path = parent_path.substr(0, parent_path.find_last_of("/") + 1);   //Parses path minus name
        //std::cout << "ASSIGNS PARENT PATH" << std::endl;
        std::string parent_kvs = dht_get_entry_contents(parent_path);
        //std::cout << "GETS PARENT KVS: " << parent_kvs << std::endl;
        sleep(1);

        if(parent_kvs == "" || parent_kvs.substr(parent_kvs.find("|") + 1) == "0") {    //If the parent of the path does not exist
            return -1;
        }

        //if (parent_kvs.find("f") != 0 && parent_kvs.find("d") != 0) { parent_kvs.erase(0, 1); }

        //std::cout << "PASSES IF CHECK" << std::endl;
        std::string parent_uid = parent_kvs.substr(0, parent_kvs.find("|"));
        //std::cout << "GETS PARENT UID: " << parent_uid << std::endl;
        std::string duid = gen_uid(true);
        std::string value = duid + "|1";
        //std::cout << "PREPARE TO PUT: " << value << std::endl;

        node.put(path, "___" + value);
        //std::cout << "GOOD PUT: " <<  dht_get_entry_contents(path) << std::endl;

        std::string name = path;
        name.pop_back();
        name = name.substr(name.find_last_of("/") + 1) + "/";
        //std::cout << "GETS NAME" << name << std::endl;

        std::string directory_value = name + "|" + parent_uid + "|" + "0" + "|" + "1" + "|" + is_root + "&" + " ";
        //std::cout << "DIR VAL STRING: " << directory_value << std::endl;

        Dir directory = Dir(duid, directory_value, path);



        //std::cout << "CREATES DIR: " << std::endl;
        node.put(duid, "___" + directory.output_string());

        //std::cout << "PUTS DIR: " << directory.output_string() << std::endl;
        sleep(1);
        //std::cout << "CREATES PARENT DIR: " << parent_uid << std::endl;
        Dir parent_dir = Dir(parent_uid, dht_get_entry_contents(parent_uid), parent_path);
        //std::cout << "PARENT DIR CONSTRUCTOR VALUE PASS IN: " << dht_get_entry_contents(parent_uid) << std::endl;
        //std::cout << "CREATES PARENT DIR SUCCESS: " << dht_get_entry_contents(parent_uid) << std::endl;
        parent_dir.value += duid + " ";
        //std::cout << "TRIES PUT PARENT: " << parent_uid << std::endl;
        //std::cout << "PARENT PATH VALUE OUTPUT STRING: " << parent_dir.output_string() << std::endl;
        node.put(parent_uid, "___" + parent_dir.output_string());
        //std::cout << "PARENT PUT SUCCESS: " << parent_uid << std::endl << std::endl << std::endl;
        std::string parent_content_check = dht_get_entry_contents(parent_uid);
        //std::cout << "PARENT GET FROM RECENT PUT: " << parent_content_check << std::endl;
        sleep(1);
    }

    else{   //Directory already exists at path, fails
        return -1;
    }

    return 0;

}

// Remove a file from the DHT
int hivefs_dht_unlink(std::string path) {
    //If path does not exist or if item is not a file, return -1
    //char end = path.back();
    if(path.substr(path.length() - 1) == "/") { return -1; }
    //std::cout << "PATH TO UNLINK " << path << std::endl;
    std::string value = dht_get_entry_contents(path);
    //std::cout << "UID OF PATH " << value << std::endl;
    if(value == "" || (value.substr(value.find("|") + 1) == "0")) { return -1; } // if doesn't exist, return -1 OR if it has already been deleted

    // change uid to does not exist
    value.back() = '0';
    node.put(path, "___" + value);

    // change attributes to does not exist
    std::string fuid = value.substr(0, value.find("|"));
    //std::cout << "FUID OF FILE TO REMOVE: " << fuid << std::endl;
    std::string contents = dht_get_entry_contents(fuid);
    //std::cout << "VALUE OF FUID TO REMOVE: " << contents << std::endl;

    File f = File(fuid, contents, path);
    f.exists = false;
    // find index of exists attribute in attribute string, change exists to 0
    // size_t pos = find_attr_index(value, 3) + 1;
    // value.replace(pos, pos + 1, '0');
    //std::cout << "NEW VALUE AT FUID: " << f.output_string() << std::endl;
    node.put(fuid, "___" + f.output_string()); // put new value at key = fuid
    //std::cout << "PARENT UID: " << f.parent_uid << std::endl;
    Dir d = Dir(f.parent_uid, dht_get_entry_contents(f.parent_uid), path.substr(0, path.find_last_of("/") + 1));
    //std::cout << "GET_CONT PARENT UID: " << dht_get_entry_contents(f.parent_uid) << std::endl;
    //std::cout << "PARENT ENTRIES: " << d.value << std::endl;


    //std::cout << "\n\nLOOK HERE\n\n" << "D.ENTRIES_IN_PATH THE VECTOR: \n" << std::endl;
    //for(int i = 0; i < d.entries_in_path.size(); ++i) { std::cout << d.entries_in_path.at(i) << " "; }


    d.entries_in_path.erase(std::remove(d.entries_in_path.begin(), d.entries_in_path.end(), f.uid));
    //std::cout << "\n\nLOOK HERE\n\n" << "NEW D.ENTRIES_IN_PATH THE VECTOR: \n" << std::endl;
    //for(int i = 0; i < d.entries_in_path.size(); ++i) { std::cout << d.entries_in_path.at(i) << " "; }

    std::string new_value = "";
    for (unsigned int i = 0; i < d.entries_in_path.size(); ++i) { new_value += d.entries_in_path.at(i); }
    d.value = new_value;
    //std::cout << "PARENT ENTRIES AFTER DELETING F.UID: " << d.value << std::endl;
    std::string val_update;
    for(const auto &piece : d.entries_in_path) val_update += piece + " ";
    d.value = val_update;
    node.put(d.uid, "___" + d.output_string());

    return 0;
}


//TO FIX
int hivefs_dht_rmdir(std::string path) {
  //If path does not exist, return -1. Ensure that it is a dir and not a file
  if(path[path.length()-1] != '/') { return -1; } // return -1 if not a dir
  if(path.length() == 1) { return -1; } // return -1 if trying to rm root
  std::string value = dht_get_entry_contents(path);
  if(value == "" || value[value.length()-1] == '0') { return -1; } // if doesn't exist, return -1

  // change uid to does not exist
  value.back() = '0';
  node.put(path, "___" + value);

  // change attributes to does not exist
  std::string duid = value.substr(0, value.find("|"));

  Dir old_d = Dir(duid, dht_get_entry_contents(duid), path);
  old_d.exists = 0;
  std::vector<std::string> dir_contents = old_d.entries_in_path;

  // loop to delete children of old_d
  for(unsigned int i = 0; i < old_d.entries_in_path.size(); ++i) {
    //std::cout << "First char: " << old_d.entries_in_path.at(i)[0] << std::endl;
    std::string sub_cont = dht_get_entry_contents(old_d.entries_in_path.at(i));
    std::string sub_name = sub_cont.substr(0, sub_cont.find("|"));
    std::string sub_path = path + sub_name;
    //std::cout << "Path of sub_dir: " << sub_path << "    Current Path: " << path <<std::endl;
    // Recursive rmdir for child dirs, unlink for child files
    if(old_d.entries_in_path.at(i)[0] == 'd') { hivefs_dht_rmdir(sub_path); }
    else if (old_d.entries_in_path.at(i)[0] == 'f') { hivefs_dht_unlink(sub_path); }
  }
  // remove deleted children from old_d
  old_d.entries_in_path.clear();
  old_d.value = "";

  // find index of exists attribute in attribute string, change exists to 0
  // size_t pos = find_attr_index(value, 3) + 1;
  // value.replace(pos, pos + 1, '0');
  node.put(duid, "___" + old_d.output_string()); // put val for deleted dir at duid

  // delete "/" at the end of the path
  path.pop_back();

  Dir d = Dir(old_d.parent_uid, dht_get_entry_contents(old_d.parent_uid), path.substr(0, path.find_last_of("/") + 1));
  d.entries_in_path.erase(std::remove(d.entries_in_path.begin(), d.entries_in_path.end(), old_d.uid));

  std::string val_update;
  for(const auto &piece : d.entries_in_path) val_update += piece + " ";
  d.value = val_update;
  node.put(d.uid, "___" + d.output_string());

  //Remove all subdirectories and files
  //Can make a recursive call to rmdir(subdir_path) to remove folders in the path, call to unlink to remove files.
  //Get the values in the directory, determine if fuid or duid, get filepath of file or dir
  //Then, pass filepath to rmdir or unlink

  return 0;
}

//WORKING
int hivefs_dht_create(std::string path) {
    if(path.substr(path.length() - 1) == "/") { return -1; }
    //std::cout << "CALLS DHT_CREATE: " << path << std::endl;

    std::string value = dht_get_entry_contents(path);
    //std::cout << "PATH_KVS: " << value << std::endl;

    if(value[value.length() - 1] == '1') { return -1; } // if exists, return -1

    //std::cout << "FILE AT PATH DOES NOT EXIST " << std::endl;
    //Check that file at path does not exist, else return -1
    std::string parent_path = path;
    parent_path = parent_path.substr(0, parent_path.find_last_of("/") + 1);   //Parses path minus name

    //std::cout << "PARENT PATH: " << parent_path << std::endl;

    //TESTED WORKING UNTIL HERE SO FAR:
    std::string parent_kvs = dht_get_entry_contents(parent_path);
    //std::cout << "PARENT_KVS: " << parent_kvs << std::endl;

    if(parent_kvs == "" || parent_kvs.substr(parent_kvs.find("|") + 1) == "0") {    //If the parent of the path does not exist
      //std::cout << "didnt make it" << std::endl;
        return -1;
    }


    //std::cout << "PARENT_KVS: " << parent_kvs << std::endl;

    std::string fuid = gen_uid(false);
    //std::cout << "NEW FILE UID: " << fuid << std::endl;
    std::string path_kvs = fuid + "|1";
    node.put(path, "___" + path_kvs);

    std::string parent_uid = parent_kvs.substr(0, parent_kvs.find("|"));
    std::string name = path.substr(path.find_last_of("/") + 1);


    std::string file_value = name + "|" + parent_uid + "|" + "1" + "|" + "1" + "|" + "NULL" + "|" + "NULL" + "|" + "NULL" + "|" + "NULL" + "|" + "NULL"+ "&" + " ";

    //std::cout << "NEW FILE CONSTRUCTOR VALUE: " << file_value << std::endl;
    //std::cout << "THIS IS THE WEIRD NODE_ID THING: " << NODE_ID << std::endl << std::endl << std::endl;
    File f = File(fuid, file_value, path, NODE_ID);
    //std::cout << "FILE STRUCT OUTPUT VALUE: " << f.output_string() << std::endl;
    //std::cout << "THIS IS THE WEIRD NODE_ID THING AFTER: " << f.file_orig << std::endl << std::endl << std::endl;

    node.put(fuid, "___" + f.output_string());

    Dir d = Dir(parent_uid, dht_get_entry_contents(parent_uid), parent_path);
    //std::cout << "DIR CONTENT STORE VALUE: " << d.value << std::endl;

    //PROBLEM IS HERE
    //std::cout << "VERY PROBLEMATIC VALUE SHOULD BE UID LIST: " << d.value << std::endl;



    d.value += fuid + " ";
    //std::cout << "NEW PROBLEMATIC VALUE SHOULD BE UID LIST: " << d.value << std::endl;

    //std::cout << "DIR CONTENT STORE VALUE AFTER ADD: " << d.output_string() << std::endl;


    node.put(parent_uid, "___" + d.output_string());

    std::string test_this_again = dht_get_entry_contents(parent_uid);
    //std::cout << "OUTPUT OF PARENTUID GET AFTER NEW PUT: " << test_this_again << std::endl;
    d = Dir(parent_uid, test_this_again, parent_path);
    //std::cout << "CONTENT AFTER PUT IN VALUE: " << d.value << std::endl;


    return 0;
}

//TO FIX
int hivefs_dht_rename(std::string path, std::string newpath){
  std::string value = dht_get_entry_contents(path);
  std::string new_value = dht_get_entry_contents(newpath);
  std::string newpath_check = dht_get_entry_contents(newpath.substr(0, newpath.find_last_of('/') + 1));

  if(value == "") { return -1; } // if doesn't exist, return -1
  if(new_value != "") { return -1; } // if newpath contents already exists, return -1
  if(newpath_check == "") { return -1; } // if parent dir of file doesn't exist, return -1
  if(value[0] == 'd') { return -1; } // if renaming dir, return -1
  if(newpath[newpath.length() - 1] == '/') { return -1; } // if rename file to dir, return -1

  std::string uid = value.substr(0, value.find("|"));
  std::string contents = dht_get_entry_contents(uid);

  File f = File(uid, contents, path, NODE_ID);
  hivefs_dht_unlink(f.path); // delete file from dht_output

  File new_f = File(uid, contents, newpath, NODE_ID);
  hivefs_dht_create(new_f.path); // create new file

  node.put(uid, "___" + new_f.output_string()); // put new value at key = fuid

  // std::cout << "This is the problem: " << newpath.substr(0, path.find_last_of("/") + 1) << std::endl;
  //
  // // add file to dir at path
  // Dir d = Dir(f.parent_uid, dht_get_entry_contents(f.parent_uid), newpath.substr(0, path.find_last_of("/") + 1));
  // std::cout << "7" << std::endl;
  // d.value += fuid + " ";
  //
  // node.put(d.uid, "___" + d.output_string());
  // std::cout << "8" << std::endl;
  return 0;
}

//TO FIX
std::string hivefs_dht_read(std::string path) {
    std::string path_kvs = dht_get_entry_contents(path);
    if(path[path.length() - 1] == '/') { return "-1"; }
    if(path_kvs == "" || (path_kvs[path_kvs.length() - 1] == '0')) { return "-1"; } // if doesn't exist, return -1
    //Make sure that file at path exists, else return "-1"
    //Return actual file value at path, without info
    std::string fuid = path_kvs.substr(0, path_kvs.find("|"));
    std::string value = dht_get_entry_contents(fuid);

    File f = File(fuid, value, path);

    return f.value;
}

//TO FIX
int hivefs_dht_write(std::string path, std::string data) {
  std::string value = dht_get_entry_contents(path);
  if(path[path.length() - 1] == '/') { return -1; }
  if(value == "" || (value[value.length() - 1] == '0')) { return -1; } // if doesn't exist, return -1
  //If file does not already exist, return -1; hivefs_dht_create must be called instead

  std::string fuid = value.substr(0, value.find("|"));
  std::string contents = dht_get_entry_contents(fuid);

  File f = File(fuid, contents, path, NODE_ID);
  f.value = data;

  node.put(fuid, "___" + f.output_string());

  return 0;
}

//TO FIX
int hivefs_dht_getattr(std::string path, struct stat& statbuf) {
    std::string path_kvs = dht_get_entry_contents(path);
    if(path_kvs == "" || (path_kvs.substr(path_kvs.find("|") + 1) == "0")) { return -1; } // if doesn't exist, return -1

    std::string uid = path_kvs.substr(0, path_kvs.find("|"));
    //std::vector<int> = return_vec;

    if (uid.at(0) == 'f') {
        //return_vec.push_back(1);

        File f = File(uid, dht_get_entry_contents(uid), path);
        statbuf.st_mode = statbuf.st_mode | S_IFREG; //(S_IRWXU | S_IRWXG | S_IRWXO);
        statbuf.st_nlink = 2;
        statbuf.st_size = f.size;
        statbuf.st_gid = std::stoi((f.file_orig).substr(1));
        statbuf.st_uid = std::stoi((f.file_last_author).substr(1));
        //Add time code update here later

    }

    else {
        //return_vec.push_back(0);
        statbuf.st_mode = statbuf.st_mode | S_IFDIR; //(S_IRWXU | S_IRWXG | S_IRWXO);
        statbuf.st_nlink = 1;
        //return.vec.push_back(1);
    }

    return 0;
}

//TO FIX
std::vector<std::string> hivefs_dht_readdir(std::string path) {
    //Determine if path is a directory, return "-1" in first item otherwise
    //Return vector of strings of filename
    //May need to check deletions here, if not already thoroughly checked
    std::vector<std::string> uids;

    std::string path_kvs = dht_get_entry_contents(path);
    std::string duid = path_kvs.substr(0, path_kvs.find("|"));
    if(path_kvs == "" || (path_kvs[path_kvs.length() - 1] == '0')) { uids.push_back("-1"); return uids; } // if doesn't exist, return -1
    if(duid[0] == 'f') { uids.push_back("-1"); return uids; }

    std::string dir_contents = dht_get_entry_contents(duid);

    Dir d = Dir(duid, dir_contents, path);
    uids = d.entries_in_path;

    std::vector<std::string> names;
    for(unsigned int i = 0; i < uids.size(); ++i) {
      std::string attr_string = dht_get_entry_contents(uids.at(i));
      names.push_back(attr_string.substr(0, attr_string.find("|")));
    }

    return names;
}



#endif

