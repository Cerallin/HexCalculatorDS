# Copied from https://github.com/devkitPro/docker/blob/master/toolchain-base/Dockerfile
# Maintainer: Cerallin <cerallin@cerallin.top>
# Based on devkitPro's toolchain-base image
# Version: 20241104 libnds=1.8.0
# Description: Docker image for developing homebrew applications for the Nintendo DS

FROM devkitpro/devkitarm:20241104

# update apt repositories and upgrade packages
RUN apt-get update && apt-get upgrade -y
RUN apt-get install -y --no-install-recommends apt-utils
RUN apt-get install -y --no-install-recommends sudo ssh ca-certificates curl wget bzip2 gnupg
RUN apt-get install -y --no-install-recommends zip unzip patch vim
RUN apt-get install -y --no-install-recommends git git-restore-mtime
RUN apt-get install -y --no-install-recommends gcc g++
RUN apt-get install -y --no-install-recommends gdb cppcheck make
RUN apt-get install -y --no-install-recommends cmake automake autoconf autoconf-archive autoconf-doc libtool
RUN apt-get install -y --no-install-recommends libfreeimage3 libfreeimage-dev
RUN apt-get install -y --no-install-recommends imagemagick

# locale
RUN apt-get install -y --no-install-recommends locales
RUN sed -i -e 's/# en_US.UTF-8 UTF-8/en_US.UTF-8 UTF-8/' /etc/locale.gen
RUN dpkg-reconfigure --frontend=noninteractive locales
RUN update-locale LANG=en_US.UTF-8
RUN apt-get clean

ENV LANG=en_US.UTF-8

# Setup env
ENV DEVKITPRO=/opt/devkitpro
ENV DEVKITARM=${DEVKITPRO}/devkitARM
ENV PATH=${DEVKITPRO}/tools/bin:$PATH

# Setup develop user
ARG USERNAME=user
ARG USER_UID=1000
ARG USER_GID=$USER_UID

# Create the user
RUN groupadd --gid $USER_GID $USERNAME \
    && useradd --uid $USER_UID --gid $USER_GID -s /bin/bash -m $USERNAME
