FROM fedora:31


RUN dnf upgrade -y

RUN dnf install git vim gcc-c++ pkg-config python cmake pip ninja-build -y

RUN pip install Cython
