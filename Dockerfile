FROM fedora:31


RUN dnf upgrade -y

RUN dnf install git vim gcc-c++ pkg-config python3-Cython.x86_64 readline-devel gnutls-devel msgpack-devel python3-devel redhat-rpm-config wget asio-devel.x86_64 python meson xz make dh-autoreconf cmake pip ninja-build -y

RUN pip install pytest
