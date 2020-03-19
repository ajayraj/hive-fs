// See  FUSE:  example/hello.c

#include "hive-fuse.h"
#include "hive-dht.h"


int main(int argc, char *argv[])
{

  hivefs_fuse fs;
  handler dht_op;
  dht_op.hivefs_dht_init_node();

  int status = fs.run(argc, argv);

  return status;
}
