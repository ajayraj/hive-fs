// Hello filesystem class implementation

#include "hive-fuse.h"

#include <iostream>
#include <string>

// include in one .cpp file
#include "Fuse-impl.h"

using namespace std;

static const string root_path = "/";
static const string hello_str = "Hello World!\n";
static const string hello_path = "/hello";

int hivefs_fuse::getattr(const char *path, struct stat *statbuf, struct fuse_file_info *)
{
   int status = hivefs_dht_getattr(std::string(path), *statbuf);
   if (status == -1)
      return -ENOENT;

   //lstat(fpath, statbuf); //fpath is the full path string, used in writing to memory? check docs, may not be needed
   return 0;
}

int hivefs_fuse::readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			               off_t, struct fuse_file_info *, enum fuse_readdir_flags)
{
   	filler(buf, ".", NULL, 0, FUSE_FILL_DIR_PLUS);
	filler(buf, "..", NULL, 0, FUSE_FILL_DIR_PLUS);

   	std::vector<std::string> entry_list;
   	entry_list = hivefs_dht_readdir(std::string(path));

   	if (entry_list.at(0) == "-1")
      return -ENOENT;

   	int i = 0;
   	while (i < entry_list.size()){
      filler(buf, entry_list.at(i).c_str(), NULL, 0, FUSE_FILL_DIR_PLUS);
      ++i;
   }

   return 0;
}


int hivefs_fuse::open(const char *path, struct fuse_file_info *fi)
{
	/*if (path != hello_path)
		return -ENOENT;

	if ((fi->flags & 3) != O_RDONLY)
		return -EACCES;*/

	return 0;
}


int hivefs_fuse::read(const char *path, char *buf, size_t size, off_t offset,
		              struct fuse_file_info *)
{
	std::string status = hivefs_dht_read(std::string(path));

   	if (status == "-1")
    	return -ENOENT;

   //Code to load file into buffer

   strcpy(buf, status.c_str());; //May have to copy more robustly
   return status.size(); //May have to cast size_t to int
}

int hivefs_fuse::create(const char* path, mode_t mode, struct fuse_file_info * ffi) {
   int status = hivefs_dht_create(std::string(path));
   if (status == -1)
      return -ENOENT;

   return 0;
}

int hivefs_fuse::rename(const char *path, const char *newpath, unsigned int flags) {
   //Get file's UUID, update file name in value to newpath input
   int status = hivefs_dht_rename(std::string(path), std::string(newpath));

   if (status == -1)
      return -ENOENT;

   return 0;

}

int hivefs_fuse::rmdir(const char *path){
   int status = hivefs_dht_rmdir(std::string(path));

   if (status == -1)
      return -ENOENT;

   return 0;
}

int hivefs_fuse::unlink(const char *path){  //Remove file function
   int status = hivefs_dht_unlink(std::string(path));
   if (status == -1)
      return -ENOENT;

   return 0;
}

int hivefs_fuse::mkdir(const char *path, mode_t mode){
   //Code for actually writing to memory
   /*char fpath[]; //Set to max path size
   fpath = ; //Full path name of folder at path, will get directory name from DHT file-tag and append
   mkdir(fpath, mode);*/

   if (hivefs_dht_mkdir(std::string(path)) == -1)
      return -EIO;

   return 0;
}

int hivefs_fuse::write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
   int status = hivefs_dht_write(std::string(path), std::string(buf));

   if (status == -1)
      return -ENOENT;

   return strlen(buf); //Cast to string if necessary, or find alternate function
   //Original call to write to memory: pwrite(fi->fh, buf, size, offset)
}

