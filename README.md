# 179F_proj
## Operating Systems Senior Design Project
- Making something cool for an expensive piece of paper

Project: A Peer-to-Peer network implementation using Filesystem in Userspace (FUSE).
Environment: A Docker container running Clear Linux with FUSE packages installed.

To generate Dockerimage:
    ```bash
    sudo docker build -t "hiveFS" .
    sudo docker run -it hiveFS
    ```
