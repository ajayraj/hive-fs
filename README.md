# 179F_proj
## Operating Systems Senior Design Project
- Making something cool for an expensive piece of paper

Project: A Peer-to-Peer network implementation using Filesystem in Userspace (FUSE).
Environment: A Docker container running Clear Linux with FUSE packages installed.

To generate Dockerimage:
    'sudo docker build -t "fedora:Dockerfile" .'
    'sudo docker run -it fedora:Dockerfile'



Command that seems to work to install opendht when facing asio errors:
	'wget https://github.com/aberaud/asio/archive/b2b7a1c166390459e1c169c8ae9ef3234b361e3f.tar.gz \'
    '&& tar -xvf b2b7a1c166390459e1c169c8ae9ef3234b361e3f.tar.gz && cd asio-b2b7a1c166390459e1c169c8ae9ef3234b361e3f/asio \'
    '&& ./autogen.sh && ./configure --prefix=/usr --without-boost --disable-examples --disable-tests  \'
    '&& sudo make install'

