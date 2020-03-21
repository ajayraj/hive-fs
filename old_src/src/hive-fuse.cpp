// Hello filesystem class implementation

#include "hive-fuse.h"
//#include "hive-dht.h"

#include <iostream>
#include <string>

// include in one .cpp file
#include "Fuse-impl.h"

/*
static const string root_path = "/";
static const string hello_str = "Hello World!\n";
static const string hello_path = "/hello";*/

int hivefs_fuse::getattr(const char *path, struct stat *statbuf, struct fuse_file_info *)
{
   std::cout << "ENTERS GETATTR " << std::endl;
   int status = hivefs_dht_getattr(std::string(path), *statbuf);
   if (status == -1)
      return -ENOENT;

   //lstat(fpath, statbuf); //fpath is the full path string, used in writing to memory? check docs, may not be needed
   return 0;
}

int hivefs_fuse::readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			               off_t, struct fuse_file_info *, enum fuse_readdir_flags)
{
   std::cout << "ENTERS READDIR " << std::endl;
   
   /*if (strcmp(path, "/") != 0)
		return -ENOENT;*/


   	filler(buf, ".", NULL, 0, FUSE_FILL_DIR_PLUS);
	filler(buf, "..", NULL, 0, FUSE_FILL_DIR_PLUS);

   	std::vector<std::string> entry_list;
      std::cout << "READING DIR " << path << std::endl;
   	entry_list = hivefs_dht_readdir(std::string(path));
      std::cout << "ENTRY_LIST SIZE " << entry_list.size() << std::endl;

   	if (entry_list.size() == 0 || entry_list.at(0) == "-1")
         return -ENOENT;

   	unsigned int i = 0;
   	while (i < entry_list.size()){
      filler(buf, entry_list.at(i).c_str(), NULL, 0, FUSE_FILL_DIR_PLUS);
      ++i;
   }

   return 0;
}


int hivefs_fuse::open(const char *path, struct fuse_file_info *fi)
{
   std::cout << "ENTERS OPEN " << std::endl;
	/*if (path != hello_path)
		return -ENOENT;

	if ((fi->flags & 3) != O_RDONLY)
		return -EACCES;*/

	return 0;
}


int hivefs_fuse::read(const char *path, char *buf, size_t size, off_t offset,
		              struct fuse_file_info *)
{
   std::cout << "ENTERS READ " << std::endl;
	std::string status = hivefs_dht_read(std::string(path));

   	if (status == "-1")
    	return -ENOENT;

   //Code to load file into buffer

   strcpy(buf, status.c_str());; //May have to copy more robustly
   return status.size(); //May have to cast size_t to int
}

int hivefs_fuse::create(const char* path, mode_t mode, struct fuse_file_info * ffi) {
   std::cout << "ENTERS CREATE " << std::endl;
   int status = hivefs_dht_create(std::string(path));
   if (status == -1)
      return -ENOENT;

   return 0;
}

int hivefs_fuse::rename(const char *path, const char *newpath, unsigned int flags) {
   //Get file's UUID, update file name in value to newpath input
   std::cout << "ENTERS RENAME " << std::endl;
   int status = hivefs_dht_rename(std::string(path), std::string(newpath));

   if (status == -1)
      return -ENOENT;

   return 0;

}

int hivefs_fuse::rmdir(const char *path){
   std::cout << "ENTERS RMDIR " << std::endl;
   int status = hivefs_dht_rmdir(std::string(path));

   if (status == -1)
      return -ENOENT;

   return 0;
}

int hivefs_fuse::unlink(const char *path){  //Remove file function
   std::cout << "ENTERS UNLINK " << std::endl;
   int status = hivefs_dht_unlink(std::string(path));
   if (status == -1)
      return -ENOENT;

   return 0;
}

int hivefs_fuse::mkdir(const char *path, mode_t mode){
   std::cout << "ENTERS MKDIR " << std::endl;
   //Code for actually writing to memory
   /*char fpath[]; //Set to max path size
   fpath = ; //Full path name of folder at path, will get directory name from DHT file-tag and append
   mkdir(fpath, mode);*/
   std::string dir_path = path;
   dir_path += "/";

   if (hivefs_dht_mkdir(std::string(dir_path)) == -1)
      return -EIO;

   return 0;
}

int hivefs_fuse::write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
   std::cout << "ENTERS WRITE " << std::endl;
   int status = hivefs_dht_write(std::string(path), std::string(buf));

   if (status == -1)
      return -ENOENT;

   return strlen(buf); //Cast to string if necessary, or find alternate function
   //Original call to write to memory: pwrite(fi->fh, buf, size, offset)
}

int main(int argc, char *argv[])
{
  //handler dht_op =  handler();
  //dht_op.
  std::cout << "ENTERS MAIN " << std::endl;
  hivefs_fuse fs;
  std::cout << "CONSTRUCTS HIVEFS_FUSE " << std::endl;
  //handler dht_op;
  hivefs_dht_init_node();
  std::cout << "PASSES INIT_NODE " << std::endl;

  int status = fs.run(argc, argv);

  return status;
}
