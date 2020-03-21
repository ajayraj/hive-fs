#include <opendht.h>
#include <string>
#include <unistd.h>
#include <vector>
#include <iostream>
#include <random>


#include "hive-dht.h"

void print_vec(std::vector<std::string> const &input) {
  for(int i = 0; i < input.size(); ++i) {
    std::cout << input.at(i) << ' ';
  }
}

int main() {
  // entry.h sanity check, also gen_uid and rand_str test
  /*std::string file_attr_string = "name|parent_uid|is_file|exists|file_orig|file_last_author|time_orig|time_last_write|size&file_contents";
  std::string dir_attr_string = "name|parent_uid|is_file|exists|is_root&dir_contents";
  Entry entry = Entry(gen_uid(false), file_attr_string, "/");
  Dir dir = Dir(gen_uid(true), dir_attr_string, "/test/");
  File file = File(gen_uid(false), file_attr_string, "/file.txt");



  std::cout << "--- entry.h ---" << std::endl;
  std::cout << "Entry Contents test: " << entry.uid + "|" + entry.path + "|" + entry.name + "|" + entry.value + "|" + std::to_string(entry.exists ? 1 : 0) + "|" + std::to_string(entry.is_file ? 1 : 0) + "|" + entry.final_string << std::endl;
  std::cout << "File Contents test: " << file.output_string() << std::endl;
  std::cout << "Dir Contents test: " << dir.output_string() << std::endl << std::endl;
  */

  // test hive-dht.cpp
  std::cout << "--- hive-dht.cpp ---" << std::endl;
  hivefs_dht_init_node();
  std::cout << "dht_init_node + get_entry_contents: " << dht_get_entry_contents(NODE_ID) << std::endl;

  std::cout << "\n- mkdir tests -\n" << std::endl;

  int firstmkdir = hivefs_dht_mkdir("/test/");
  //std::cout << "dht_mkdir test: func return: " << std::to_string(firstmkdir) << " Path kvs: " << dht_get_entry_contents("/test/") << std::endl;
  int secondmkdir = hivefs_dht_mkdir("/test/BOI/");
  //std::cout << "dht_mkdir test2: func return: " << std::to_string(secondmkdir) << " Path kvs: " << dht_get_entry_contents("/test/BOI/") << std::endl;
  int thirdmkdir = hivefs_dht_mkdir("/test/");
  //std::cout << "dht_mkdir test3: func return: " << std::to_string(thirdmkdir) << " Path kvs: " << dht_get_entry_contents("/test/") << std::endl;
  int fourthmkdir = hivefs_dht_mkdir("/does/not/exist/");
  //std::cout << "dht_mkdir test4: func return: " << std::to_string(fourthmkdir) << " Path kvs: " << dht_get_entry_contents("/test/does/not/exist/") << std::endl;
  int fifthmkdir = hivefs_dht_mkdir("/test/BOI/yezzir/");
  //std::cout << "dht_mkdir test5: func return: " << std::to_string(fifthmkdir) << " Path kvs: " << dht_get_entry_contents("/test/BOI/yezzir/") << std::endl;
  int sixthmkdir = hivefs_dht_mkdir("/test/BOI/yezzir/file.txt");
  //std::cout << "dht_mkdir test6: func return: " << std::to_string(sixthmkdir) << " Path kvs: " << dht_get_entry_contents("/test/BOI/yezzir/file.txt") << std::endl;

  std::cout << "\n- create tests -\n" << std::endl;

  int firstcreate = hivefs_dht_create("/file.txt");
  //std::cout << "dht_create test: func return: " << std::to_string(firstcreate) << " Path kvs: " << dht_get_entry_contents("/file.txt") << std::endl;
  int secondcreate = hivefs_dht_create("/file.txt");
  //std::cout << "dht_create test2: func return: " << std::to_string(secondcreate) << " Path kvs: " << dht_get_entry_contents("/file.txt") << std::endl;
  int thirdcreate = hivefs_dht_create("/test/file.txt");
  //std::cout << "dht_create test3: func return: " << std::to_string(thirdcreate) << " Path kvs: " << dht_get_entry_contents("/test/file.txt") << std::endl;
  int fourthcreate = hivefs_dht_create("/does/not/exist/file.txt");
  //std::cout << "dht_create test4: func return: " << std::to_string(fourthcreate) << " Path kvs: " << dht_get_entry_contents("/does/not/exist/file.txt") << std::endl;

  std::string test_value_uid = dht_get_entry_contents("/test/").substr(0, dht_get_entry_contents("/test/").find("|"));
  Dir test_value_dir = Dir(test_value_uid, dht_get_entry_contents(test_value_uid), "/test/");
  //std::cout << "/test/ contents test: " << test_value_dir.value.substr(0, test_value_dir.value.find("&")) << std::endl;
  //std::cout << "/test/file.txt uid: " << dht_get_entry_contents("/test/file.txt") << std::endl;
  //std::cout << "/test/ uid kvs: " << dht_get_entry_contents(test_value_dir.uid) << std::endl;

  std::cout << "\n- rmdir tests -\n" << std::endl;

  int firstrmdir = hivefs_dht_rmdir("/test/BOI/");
  //std::cout << "dht_rmdir test: func return: " << std::to_string(firstrmdir) << " Path kvs: " << dht_get_entry_contents("/test/BOI/") << std::endl;
  //std::cout << "dht_rmdir test2: func return: " << std::to_string(firstrmdir) << " Path kvs: " << dht_get_entry_contents("/test/BOI/yezzir/") << std::endl;
  int thirdrmdir = hivefs_dht_rmdir("/test/BOI/yezzir/");
  //std::cout << "dht_rmdir test3: func return: " <<  std::to_string(thirdrmdir)<< " Path kvs: " << dht_get_entry_contents("/test/BOI/yezzir/") << std::endl;
  int fourthrmdir = hivefs_dht_rmdir("/");
  //std::cout << "dht_rmdir test4: func return: " << std::to_string(fourthrmdir) << " Path kvs: " << dht_get_entry_contents("/") << std::endl;


  std::cout << "\n- readdir tests -\n" << std::endl;

  std::vector<std::string> firstreaddir = hivefs_dht_readdir("/file.txt");
  //std::cout << "dht_readdir test: names: "; print_vec(firstreaddir); std::cout << std::endl;
  std::vector<std::string> secondreaddir = hivefs_dht_readdir("/test/");
  //std::cout << "dht_readdir test2: names: "; print_vec(secondreaddir); std::cout << std::endl;
  std::vector<std::string> thirdreaddir = hivefs_dht_readdir("/does/not/exist/file.txt");
  //std::cout << "dht_readdir test3: names: "; print_vec(thirdreaddir); std::cout << std::endl;


  std::cout << "\n- unlink tests -\n" << std::endl;

  int firstunlink = hivefs_dht_unlink("/test/file.txt");
  //std::cout << "dht_unlink test: func return: " << std::to_string(firstunlink) << " Path kvs: " << dht_get_entry_contents("/test/file.txt") << std::endl;
  int recreate = hivefs_dht_create("/test/file.txt");
  //std::cout << "Recreate Test: func return: " << std::to_string(recreate) << " Path kvs: " << dht_get_entry_contents("/test/file.txt") << std::endl;
  int reunlink = hivefs_dht_unlink("/test/file.txt");
  //std::cout << "reunlink test: func return: " << std::to_string(reunlink) << " Path kvs: " << dht_get_entry_contents("/test/file.txt") << std::endl;
  int secondunlink = hivefs_dht_unlink("/does_not_exist.txt");
  //std::cout << "dht_unlink test2: func return: " << std::to_string(secondunlink) << " Path kvs: " << dht_get_entry_contents("/does_not_exist.txt") << std::endl;
  int thirdunlink = hivefs_dht_unlink("/test/file.txt");
  //std::cout << "dht_unlink test3: func return: " << std::to_string(thirdunlink) << " Path kvs: " << dht_get_entry_contents("/test/file.txt") << std::endl;
  int fourthunlink = hivefs_dht_unlink("/test/");
  //std::cout << "dht_unlink test4: func return: " << std::to_string(fourthunlink) << " Path kvs: " << dht_get_entry_contents("/test/") << std::endl;

  std::cout << "\n- read/write tests -\n" << std::endl;

  int firstwrite = hivefs_dht_write("/file.txt", "this is file content");
  //std::cout << "dht_write test: func return: " << std::to_string(firstwrite) << " Read: " << hivefs_dht_read("/file.txt") << std::endl;
  int secondwrite = hivefs_dht_write("/test/", "this dir should not have file content");
  //std::cout << "dht_write test2: func return: " << std::to_string(secondwrite) << " Read: " << hivefs_dht_read("/test/") << std::endl;
  int thirdwrite = hivefs_dht_write("/does/not/exist/file.txt", "this file doesn't exist...");
  //std::cout << "dht_write test3: func return: " << std::to_string(thirdwrite) << " Read: " << hivefs_dht_read("/does/not/exist/file.txt") << std::endl;



  std::cout << "\n- rename tests -\n" << std::endl;

  int firstrename = hivefs_dht_rename("/file.txt", "/test/newfile.txt");
  //std::cout << "dht_rename test: func return: " << std::to_string(firstrename) << " Path kvs: " << dht_get_entry_contents("/test/newfile.txt") << std::endl;
  int secondrename = hivefs_dht_rename("/file.txt", "/test/file.txt");
  //std::cout << "dht_rename test2: func return: " << std::to_string(secondrename) << " Path kvs: " << dht_get_entry_contents("/test/file.txt") << std::endl;
  int thirdrename = hivefs_dht_rename("/does/not/exist/file.txt", "/test/newfile.txt");
  //std::cout << "dht_rename test3: func return: " << std::to_string(thirdrename) << " Path kvs: " << dht_get_entry_contents("/test/newfile.txt") << std::endl;
  int fourthrename = hivefs_dht_rename("/file.txt", "/does/not/exist/file.txt");
  //std::cout << "dht_rename test4: func return: " << std::to_string(fourthrename) << " Path kvs: " << dht_get_entry_contents("/does/not/exist/file.txt") << std::endl;
  int fifthrename = hivefs_dht_rename("/test/newfile.txt", "/test/");
  //std::cout << "dht_rename test5: func return: " << std::to_string(fifthrename) << " Path kvs: " << dht_get_entry_contents("/test/") << std::endl;
  //std::cout << "dht_rename test6: func return: " << std::to_string(fifthrename) << " Path kvs: " << dht_get_entry_contents("/test/newfile.txt") << std::endl;

  // std::cout << "- getattr tests -" << std::endl;


  /*
  // gen_uid() tests
  std::cout << "UID directory: " << gen_uid(true) << std::endl;
  std::cout << "UID file: " << gen_uid(false) << std::endl;

  // split_str() tests
  std::string attr = "name|parent_uid|is_file|exists|file_orig|file_last_author|time_orig|time_last_write|size&file_contents";
  std::vector<std::string> fss = split_str(attr);
  for(int i = 0; i < fss.size(); ++i)
    std::cout << fss.at(i) << std::endl;

  // node tests
  // init_node() check
  init_node();

  // empty node test
  std::cout << "Empty node output: " << dht_get_file_contents("asdfasdfasdfasdf") << std::endl;

  // put, get and init_node() tests
  node.put("unique_key", "CONTENT");
  std::cout << "unique_key value: " << dht_get_file_contents("unique_key") << std::endl;
  std::cout << "Is " + NODE_ID + " available?: " << dht_get_file_contents(NODE_ID) << std::endl;
  node.join();
  */
  return 0;
}
