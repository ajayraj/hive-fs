// See  FUSE:  example/hello.c

#include "hive-fuse.h"

int main(int argc, char *argv[])
{

  hivefs_fuse fs;
  hivefs_dht_init_node();

  int status = fs.run(argc, argv);

  return status;
}
