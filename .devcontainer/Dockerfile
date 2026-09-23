# Copied from https://github.com/devkitPro/docker/blob/master/toolchain-base/Dockerfile
# Maintainer: Cerallin <cerallin@cerallin.top>
# Based on devkitPro's toolchain-base image
# Version: 20241104 libnds=1.8.0
# Description: Docker image for developing homebrew applications for the Nintendo DS

FROM ubuntu:latest

# Bullseye LTS ended 2026-08-31; live debian-security indexes 404.
# Use the snapshot mirrors already present in the base image.
#RUN sed -i \
#      -e 's|^deb http://deb.debian.org|# deb http://deb.debian.org|' \
#      -e 's|^deb http://security.debian.org|# deb http://security.debian.org|' \
#      -e 's|^# deb http://snapshot.debian.org|deb http://snapshot.debian.org|' \
#      /etc/apt/sources.list \
# && echo 'Acquire::Check-Valid-Until "false";' > /etc/apt/apt.conf.d/99no-check-valid-until \
RUN apt-get update && apt-get upgrade -y

RUN apt-get install -y --no-install-recommends sudo ssh ca-certificates curl wget bzip2 gnupg
RUN apt-get install -y --no-install-recommends zip unzip xz-utils patch vim
RUN apt-get install -y --no-install-recommends git git-restore-mtime
RUN apt-get install -y --no-install-recommends gcc g++
RUN apt-get install -y --no-install-recommends gdb cppcheck make
RUN apt-get install -y --no-install-recommends cmake automake autoconf autoconf-archive autoconf-doc libtool texinfo
RUN apt-get install -y --no-install-recommends libfreeimage3 libfreeimage-dev
RUN apt-get install -y --no-install-recommends imagemagick
RUN apt-get install -y --no-install-recommends clang-format clangd
RUN apt-get install -y --no-install-recommends python3-opencv python3-numpy

# Compile and install devkitpro
ENV DEVKITPRO=/opt/devkitpro
ENV DEVKITARM=${DEVKITPRO}/devkitARM

ENV DEVKIT_SOURCE=/build

## devkitARM
RUN mkdir -p ${DEVKIT_SOURCE}
RUN git clone --depth 1 https://github.com/devkitPro/buildscripts.git ${DEVKIT_SOURCE}/buildscripts -b 9b57a02
COPY ./dkpro-config.sh ${DEVKIT_SOURCE}/buildscripts/config.sh
COPY ./buildscripts-fetch-from-github.patch ${DEVKIT_SOURCE}/buildscripts-fetch-from-github.patch
RUN cd ${DEVKIT_SOURCE}/buildscripts && \
    patch -p1 < ${DEVKIT_SOURCE}/buildscripts-fetch-from-github.patch && \
    MAKE="make -j$(nproc)" ./build-devkit.sh

## Versions
ENV GRIT_VERSION=v0.10.0
ENV GENERAL_TOOLS_VERSION=v1.4.4
ENV NDSTOOL_VERSION=v2.3.1
ENV LIBNDS_VERSION=v1.8.3

## grit
RUN git clone -b ${GRIT_VERSION} --depth 1 https://github.com/devkitPro/grit.git ${DEVKIT_SOURCE}/grit
RUN cd ${DEVKIT_SOURCE}/grit && \
    ./autogen.sh && \
    CFLAGS="-flto" CXXFLAGS="-flto" ./configure --prefix=${DEVKITPRO}/tools && \
    make -j$(nproc) && \
    make install

## general tools
RUN git clone -b ${GENERAL_TOOLS_VERSION} --depth 1 https://github.com/devkitPro/general-tools.git ${DEVKIT_SOURCE}/general-tools
RUN cd ${DEVKIT_SOURCE}/general-tools && \
    ./autogen.sh && \
    CFLAGS="-flto" CXXFLAGS="-flto" ./configure --prefix=${DEVKITPRO}/tools && \
    make -j$(nproc) && \
    make install

## ndstool
RUN git clone -b ${NDSTOOL_VERSION} --depth 1 https://github.com/devkitPro/ndstool.git ${DEVKIT_SOURCE}/ndstool
RUN cd ${DEVKIT_SOURCE}/ndstool && \
    ./autogen.sh && \
    CFLAGS="-flto" CXXFLAGS="-flto" ./configure --prefix=${DEVKITPRO}/tools && \
    make -j$(nproc) && \
    make install

## libnds, requires general tools and grit
RUN git clone -b ${LIBNDS_VERSION} --depth 1 https://github.com/devkitPro/libnds.git ${DEVKIT_SOURCE}/libnds
RUN cd ${DEVKIT_SOURCE}/libnds && \
    sed -i 's|-g -Wall|-g -Wall -flto|' arm7/Makefile && \
    sed -i 's|-g -Wall|-g -Wall -flto|' arm9/Makefile && \
    make -j$(nproc) && \
    make install

# locale
RUN apt-get install -y --no-install-recommends locales
RUN sed -i -e 's/# en_US.UTF-8 UTF-8/en_US.UTF-8 UTF-8/' /etc/locale.gen
RUN dpkg-reconfigure --frontend=noninteractive locales
RUN update-locale LANG=en_US.UTF-8
RUN apt-get clean

ENV LANG=en_US.UTF-8

# Setup env
ENV PATH=${DEVKITPRO}/tools/bin:$PATH

# Passwordless sudo for the default ubuntu user (uid 1000/1001 depending on image)
RUN echo "ubuntu ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/ubuntu \
    && chmod 0440 /etc/sudoers.d/ubuntu \
    && visudo -cf /etc/sudoers.d/ubuntu
