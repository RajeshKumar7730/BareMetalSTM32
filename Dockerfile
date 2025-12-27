

FROM ubuntu:latest

WORKDIR /workspace

RUN apt-get update && \
     apt-get install -y \
        build-essential \
        make \
        cmake \
        python3 \
         python3-pip \
        gcc-arm-none-eabi \
        gdb-multiarch \
        binutils-arm-none-eabi \
        openocd \
        git-all \
        protobuf-compiler

RUN pip3 install --no-cache-dir protobuf grpcio-tools --break-system-packages
ENV PATH="${PATH}:/workspace/protobuf/nanopb/generator"

ENTRYPOINT ["/bin/bash"]
