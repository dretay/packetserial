FROM ubuntu:22.04

# update apt-get
RUN apt-get update -y && apt-get upgrade -y

#install deps
RUN apt-get install ruby valgrind cppcheck python3-pip protobuf-compiler python3-protobuf git vim -y
RUN pip install pyinstaller
COPY ./lib/nanopb-0.3.9.3 /opt/nanopb-0.3.9.3
WORKDIR /opt/nanopb-0.3.9.3/tools
RUN DEST=/opt/nanopb-0.3.9.3 ./make_linux_package.sh
WORKDIR /app
