// Hello filesystem class definition

#ifndef __HIVEFS_FUSE_H_
#define __HIVEFS_FUSE_H_

#include "Fuse.h"

#include "Fuse-impl.h"
#include "hive-dht.h"

class hivefs_fuse : public Fusepp::Fuse<hivefs_fuse>
{
public:
  hivefs_fuse() {}

  ~hivefs_fuse() {}
  static handler dht_op;

  //static int getattr (const char *, struct stat *, struct fuse_file_info *);

  /*static int readdir(const char *path, void *buf,
                     fuse_fill_dir_t filler,
                     off_t offset, struct fuse_file_info *fi,
                     enum fuse_readdir_flags);*/
  
  static int open(const char *path, struct fuse_file_info *fi);

  /*static int read(const char *path, char *buf, size_t size, off_t offset,
                  struct fuse_file_info *fi);*/

        static int getattr(const char*, struct stat*, struct fuse_file_info *);
        static int mkdir(const char *, mode_t );
        static int rmdir(const char *);
        static int unlink(const char *);
        static int rename(const char *, const char *, unsigned int);
        static int read(const char *, char *, size_t , off_t , struct fuse_file_info *);
        static int create(const char* path, mode_t mode, struct fuse_file_info * ffi);
        static int write(const char *, const char *, size_t , off_t , struct fuse_file_info *);
        static int readdir(const char *, void *, fuse_fill_dir_t, off_t , struct fuse_file_info *, fuse_readdir_flags flags);

};

#endif
