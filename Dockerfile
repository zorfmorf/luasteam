FROM alpine:3.15.4

RUN apk update
RUN apk upgrade
RUN apk add g++ musl-dev make luajit luajit-dev

COPY src /build/src
COPY sdk /build/sdk
COPY Makefile /build/Makefile

CMD cd build && make linux64