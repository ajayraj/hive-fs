# hive-fs
## Distributed Peer-to-Peer Filesystem using OpenDHT and LibFUSE

Project: A Peer-to-Peer network implementation using Filesystem in Userspace (FUSE).
Environment: A Docker container running Clear Linux with FUSE packages installed.

To run code: 
- Build OpenDHT and install fuse3 on your system
- cd into the src folder
- make to build HiveFS
- Create a directory to be mounted
- Force FUSE unmount: `fusermount -u [directory_name]`
- Add group fuse if not already existing
- Grant yourself permissions using the command `sudo usermod -aG fuse [username]`
- Start a bootstrap node in a separate terminal using `dhtnode -b localhost -p 4222`
- Then run `./hive-fuse -f [directory_name]` to mount your filesystem and connect to OpenDHT
- In a separate terminal, you can enter the mounted directory and test basic commands (ls, mkdir, etc.)

To generate Dockerimage:
```
# build ubuntu image
sudo docker build -t "hive_fs" .
# create and run container for docker image
sudo docker run -it hive_fs
```
