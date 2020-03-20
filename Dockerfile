FROM ubuntu:rolling

RUN apt update && apt upgrade -y;

RUN apt install git vim lsb-release udev g++ pkg-config libncurses5-dev fuse3 libreadline-dev nettle-dev libgnutls28-dev libargon2-0-dev libmsgpack-dev librestbed-dev libjsoncpp-dev curl wget cython3 python3-dev python3-setuptools python-pip python3-pip meson make dh-autoreconf cmake ninja-build -y

RUN pip install pytest

RUN cd ~ && mkdir P2P && cd ~/P2P && mkdir lib

RUN cd ~/P2P/lib

RUN git clone https://github.com/savoirfairelinux/opendht.git

RUN wget https://github.com/aberaud/asio/archive/b2b7a1c166390459e1c169c8ae9ef3234b361e3f.tar.gz \
    && tar -xvf b2b7a1c166390459e1c169c8ae9ef3234b361e3f.tar.gz && cd asio-b2b7a1c166390459e1c169c8ae9ef3234b361e3f/asio \
    && ./autogen.sh && ./configure --prefix=/usr --without-boost --disable-examples --disable-tests \
    && make install

RUN mv opendht ~/P2P/lib
