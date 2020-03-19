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

class handler {
public:

void hivefs_dht_init_node();

static dht::DhtRunner node;

  handler() {}

  ~handler() {}


static std::string NODE_ID;
std::string dht_get_entry_contents(std::string);
std::string rand_str( size_t  );
std::string gen_uid(bool );
size_t find_attr_index(const std::string& , int );
int hivefs_dht_mkdir(std::string );
int hivefs_dht_unlink(std::string );
int hivefs_dht_rmdir(std::string );
int hivefs_dht_create(std::string );
int hivefs_dht_rename(std::string , std::string );
std::string hivefs_dht_read(std::string );
int hivefs_dht_write(std::string , std::string ) ;
int hivefs_dht_getattr(std::string , struct stat& );
std::vector<std::string> hivefs_dht_readdir(std::string );
};




#endif

