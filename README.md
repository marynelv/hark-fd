HARK-FD
=======

This repository contains HARK-FD v. 2.2.0 (a library with the main modules for HARK). The code was originally downloaded from http://www.hark.jp/wiki.cgi?page=Softwares and has been slightly modified to add more debug information.

The license agreement for this software can be downloaded from http://www.hark.jp/HARK_License_Agreement.pdf.


## Installation in Ubuntu

1) Install dependencies:

```bash
$ sudo apt-get install libtool libgnomeui-dev libxml++2.6-dev libxml2-dev libasound2-dev libboost-dev libboost-regex-dev libboost-thread-dev libboost-program-options-dev libboost-filesystem-dev libsndfile1-dev ispell
```

2) Compile libharknetapi

```bash
$ cd libharknetapi
$ make static
```

3) Compile hark-df & install

```bash
$ cd hark-fd
$ ./configure --prefix=/usr/ --enable-harknetapi --with-harknetapi-inc=`pwd`/../libharknetapi --with-harknetapi-lib=`pwd`/../libharknetapi 
$ make
$ sudo make install
```

**NOTE:** You can install hark-fd in a different directory (e.g., --prefix=/usr/local is the default option) but this may prevent hark_designer from finding the HARK modules. The official HARK documentation suggests to set the FLOWDESIGNER_PATH environment variable in these cases. For example,

```bash
export FLOWDESIGNER_PATH=$FLOWDESIGNER_PATH:/usr/local/lib
```

See the official [installation instructions](http://www.hark.jp/wiki.cgi?page=HARK+Installation+Instructions) for more details.


## Debugging
Use the batchflow command line tool to debug the execution of a HARK program, as described in http://www.hark.jp/document/2.2.0/hark-cookbook-en/subsec-SomethingIsWrong-007.html.