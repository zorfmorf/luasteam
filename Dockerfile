FROM ubuntu:lunar

RUN apt update
RUN apt upgrade -y
RUN apt install -y make g++ luajit libluajit-5.1-dev

COPY src /build/src
COPY sdk /build/sdk
COPY Makefile /build/Makefile

CMD cd build && make linux64