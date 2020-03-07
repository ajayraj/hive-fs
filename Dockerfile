FROM fedora:31

RUN dnf upgrade -y

RUN dnf install git vim gcc-c++ pkg-config python3-Cython.x86_64 readline-devel gnutls-devel msgpack-devel python3-devel redhat-rpm-config wget asio-devel.x86_64 python meson xz make dh-autoreconf cmake pip ninja-build -y

RUN pip install pytest

RUN cd ~ && mkdir P2P && cd ~/P2P && mkdir lib

RUN cd ~/P2P/lib

RUN git clone https://github.com/savoirfairelinux/opendht.git

RUN wget https://github.com/aberaud/asio/archive/b2b7a1c166390459e1c169c8ae9ef3234b361e3f.tar.gz \
    && tar -xvf b2b7a1c166390459e1c169c8ae9ef3234b361e3f.tar.gz && cd asio-b2b7a1c166390459e1c169c8ae9ef3234b361e3f/asio \
    && ./autogen.sh && ./configure --prefix=/usr --without-boost --disable-examples --disable-tests \
    && sudo make install

RUN mv opendht ~/P2P/lib
