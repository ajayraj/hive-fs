// See  FUSE:  example/hello.c

#include "hive-fuse.h"

//static dht::DhtRunner node;
//static std::string NODE_ID;

int main(int argc, char *argv[])
{
  //handler dht_op =  handler();
  //dht_op.
  hivefs_fuse fs;
  //handler dht_op;
  hivefs_dht_init_node();

  int status = fs.run(argc, argv);

  return status;
}
