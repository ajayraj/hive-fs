# 179F_proj
## Operating Systems Senior Design Project
- Making something cool for an expensive piece of paper

Project: A Peer-to-Peer network implementation using Filesystem in Userspace (FUSE).
Environment: A Docker container running Clear Linux with FUSE packages installed.

To generate Dockerimage:
```
# build ubuntu image
sudo docker build -t "hiveFS" .;
# create and run container for docker image
sudo docker run -it hiveFS -name node
```
