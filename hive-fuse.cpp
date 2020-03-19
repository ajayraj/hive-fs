/* -- IMPORTANT COMMENTS, NOTES, AND ASSUMPTIONS --
   Choose: high level or low level API
   All nodes have access to all files in the OpenDHT hashtable. None have local access to all the files.
   If a file that is not locally on the node is needed, it is requested and downloaded from a node carrying that file.
   First, check if file is available on node, OR search for node where file resides
   Download file if necessary
   OpenDHT nodes will use a UUID-file key-value store, REST API calls to push and pull files from the hashtable.
   On file creation, FUSE will get the UUID of the file per the node, map directory internally if necessary, then add the kvp to DHT.
   Do not need to expect nodes to enter or exit frequently, if at all.
   Do files need to only be stored on the node, or written locally as per examples as well? Likely written locally based on node info.
   Is the node running on a computer only expected to have files in DHT? Not likely since mapped by UUID.
   FUSE may have to update each file based on DHT vals
   On edit/write of non-local file, may push new value to kvs, remove old value, or keep both as a feature for version control?
   Then, most recent value in hash will be valid.
   File will have to be updated at the host node. OpenDHT listening for change.
   If node is accessing non-local node for file, try to make it so that the node being accessed knows and listens.
   Use map for filepath string to UUID? Or hashed UUID? Hashing may be unnecessary if UUID is key.
   Devise method for each node to be aware of which it is.
   Maybe set up so that the singular bootstrap node will be only one running full updates on map?
   Look into the DHT node folder directory routing
   Worth setting up second network of DHT for filepath to UUID kvp? Must be a better solution...
   Otherwise, come up with solution for parsing all file-path UUID's not in node.
   Show sibling-level directories somehow?
   What do I need: way to "ls" another node's filesystem to chart UUID's
   Best solution may be for each node to have its own map...
   Nodes in DHT simply are just an access to the file... not a sorting.
   Maybe set so that first KVS in OpenDHT is a store of UUID's of all nodes? Sibling directories in root.
   IF second DHT for file locations:
   KEY: likely letter based for easy separation? maybe start with a specific character.
   VALUE: include all items in folder. Files will be expressed as UUID, folder with different identifier.
   Should we treat each node as its own directory?
   Consider the same folder across multiple nodes, each with different files.
   Can file management KVS just use folder names as the key???
   Probably, from the root down can likely assume that all folders are shared
   Problem: multiple folders with the same name (subfolders)
   Can work if very careful to compare against relative directory
   If cannot workaround, can store folder name as first value in kvs?
   After any changes are made to folder structure, add or remove UUID/folder key from value.
   Random name and storing folder directory name as first item in value may be best method.
   Folder names will be tagged according to id, names can be changed as value without affecting other filepath values.
   Need to translate from folder path directory name to key values accurately again... See if having a known root node will help.
   Likely new solution: use one DHT with both different UUID and folder-tag keys
   KVS will operate similarly to described above, but under one table.
   Implementation will have to address unique key parsing and identification, as well as value store format.
   UUID-KVS: UUID->(first_line_filename_or_filepath , base_64 encoded file information)
   TAG-KVS: TAG->(tag_real_folder_name , TAG_OF_SUBFOLDER_1 , TAG_OF_SUBFOLDER_2, ..., UUID_OF_FILE_1, UUID_OF_FILE_2, ...)

   May or may not have to reconstruct the DIRENT, if it can be copied over completely successfully.
   If a directory can successfully be locally opened, no need to pull everything.

   Would it be possible or useful to add a bit to mark whether or not a directory is local?
   When a folder is opened, could create empty files and populate on a file being opened?

   For updating a file locally and making sure copy is valid:
      We can have each fuse running node be aware of which node it is.
      NodeID will be an entry on the DHT, value will be UUID of local file being read on non-local node.
      The KVS for the UUID-file store may have a leading value with the key for the node.
      Each node will be constantly listening to its own NodeID to update a file upon change, will listen to UUIDs in value.
      On update, will remove UUID from value in NodeID entry

   Node Initialization:
      Connect to the bootstrap node.
      Might likely have unique key for root directory so that all can connect to it to populate base directory.
      Try to get NodeID ni, if successful then increment until an empty ID is found.
      Save as char* to remember its own identity.

   One function at a time :)
*/

//#include "hive-fuse.h"
//#include "fuse-linker-impl.h"

#include "hive-dht.cpp"

extern "C" {

#define FUSE_USE_VERSION 30

#include <fuse.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <assert.h>
#include <stdio.h>

int hivefs_fuse_mkdir(const char *path, mode_t mode){
   //Code for actually writing to memory
   /*char fpath[]; //Set to max path size
   fpath = ; //Full path name of folder at path, will get directory name from DHT file-tag and append
   mkdir(fpath, mode);*/

   if (hivefs_dht_mkdir(std::string(path)) == -1)
      return -EIO;

   return 0;
}

int hivefs_fuse_unlink(const char *path){  //Remove file function
   int status = hivefs_dht_unlink(std::string(path));
   if (status == -1)
      return -ENOENT;

   return 0;
}

int hivefs_fuse_rmdir(const char *path){
   int status = hivefs_dht_rmdir(std::string(path));

   if (status == -1)
      return -ENOENT;

   return 0;
}


//Both input parameters are filesystem relative
int hivefs_fuse_rename(const char *path, const char *newpath, unsigned int flags) {
   //Get file's UUID, update file name in value to newpath input
   int status = hivefs_dht_rename(std::string(path), std::string(newpath));

   if (status == -1)
      return -ENOENT;

   return 0;

}

static int hivefs_fuse_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
   //Returns bytes read
   std::string status = hivefs_dht_read(std::string(path));

   if (status == "-1")
      return -ENOENT;

   //Code to load file into buffer

   strcpy(buf, status.c_str());; //May have to copy more robustly
   return status.size(); //May have to cast size_t to int
   //pread(fi->fh, buf, size, offset) is the call to actually read the fuse_file_info into buffer

}


int hivefs_fuse_create(const char* path, mode_t mode, struct fuse_file_info *) {
   int status = hivefs_dht_create(std::string(path));
   if (status == -1)
      return -ENOENT;

   return 0;
}


/** Write data to an open file
 *
 * Write should return exactly the number of bytes requested
 * except on error.  An exception to this is when the 'direct_io'
 * mount option is specified (see read operation).
 *
 * Changed in version 2.2
 */
// As  with read(), the documentation above is inconsistent with the
// documentation for the write() system call.
int hivefs_fuse_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
   int status = hivefs_dht_write(std::string(path), std::string(buf));

   if (status == -1)
      return -ENOENT;

   return strlen(buf); //Cast to string if necessary, or find alternate function
   //Original call to write to memory: pwrite(fi->fh, buf, size, offset)
}



static int hivefs_fuse_getattr(const char* path, struct stat* statbuf, struct fuse_file_info * ffi) {
   //STATBUF STRUCT INFO: http://man7.org/linux/man-pages/man2/stat.2.html
   //Can ignore st_ino, st_dev, st_blksize
   int status = hivefs_dht_getattr(std::string(path), *statbuf);
   if (status == -1)
      return -ENOENT;

   //lstat(fpath, statbuf); //fpath is the full path string, used in writing to memory? check docs, may not be needed
   return 0;
}

static int hivefs_fuse_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi, fuse_readdir_flags flags) {
   //Every directory has two entries, .: current path; ..: previous path
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


static struct fuse_operations hivefs_oper;


int main(int argc, char *argv[])
{
	int ret;
   
	//struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
   hivefs_oper.getattr = hivefs_fuse_getattr;
	//hivefs_oper.readlink = hivefs_readlink;
	//hivefs_oper.mknod = hivefs_mknod;
	hivefs_oper.mkdir = hivefs_fuse_mkdir;
	//hivefs_oper.fgetattr = hivefs_fgetattr;
	//hivefs_oper.access = hivefs_access;

	//hivefs_oper.opendir = hivefs_opendir;
	hivefs_oper.readdir = hivefs_fuse_readdir;
   hivefs_oper.read = hivefs_fuse_read;
	//hivefs_oper.releasedir = hivefs_releasedir;


	//hivefs_oper.symlink = hivefs_symlink;
	hivefs_oper.unlink = hivefs_fuse_unlink;
	hivefs_oper.rmdir = hivefs_fuse_rmdir;
	hivefs_oper.rename = hivefs_fuse_rename;
	//hivefs_oper.link = hivefs_link;
	//hivefs_oper.chmod = hivefs_chmod;
	//hivefs_oper.chown = hivefs_chown;
	//hivefs_oper.truncate = hivefs_truncate;
	//hivefs_oper.ftruncate = hivefs_ftruncate;
   //Initializes node and connects to bootstrap, assigns unique Node ID in hive-dht.cpp
	
   hivefs_dht_init_node();

	ret = fuse_main(argc, argv, &hivefs_oper, NULL);
	//fuse_opt_free_args(&args);
	return ret;
}


}