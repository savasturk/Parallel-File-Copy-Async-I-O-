# Parallel-File-Copy-Async-I-O-
Copy a text file to a defined destination path using Asynchronous I/O operations like download manager. You will copy the content of source file (source.txt) to a new destination file (destination.txt).

The POSIX AIO and pthread libraries will be used in C prog. lang..

You will take 3 input parameters.
o Source path (- (dash) means same path as executable file)
o Destination path (- (dash) means same path as executable file)
o Number of threads

Source and destination paths can be given as real paths OR as the character (– dash)
which means that related txt file will be created under the same path as executable
file.
Define a function to prepare a source file randomly. The content of the source file
must be readable. You will create the source file in main thread. (max size of file
100 MB)
For example; let’s say that the user enters 8 threads and the size of the source file
as 16 bytes. So each thread must copy 2 bytes for each. In order to be readable, your
source file must be created randomly as “aabbccddeeffgghh”.
o 1 st thread should copy from source.txt to destination.txt first 2 bytes > aa
o 2 nd thread should copy from source.txt to destination.txt second 2 bytes >
bb
o ...
o 8 th thread should copy from source.txt to destination.txt last 2 bytes > hh
Create threads (user will be able to define the number of threads from 1 to 10) for
asynchronous I/O copy process (read + write).
Finally, the source.txt file is the exact same content with the destination.txt file.

The code compiled in Ubuntu 16.04 LTS Operating System and with c99 compiler.

Please run the code like this.

// gcc -lrt -c copyFile.c

// gcc 2015510052.o -lrt -o copyFile.out -lpthread

// ./copyFile.out - - 5

// To check with md5sum
//**************************
// md5sum source.txt > md1.md5
// md5sum destination.txt > md2.md5
// md5sum -c md1.md5 md2.md5
///////OR////////
// md5sum source.txt destination.txt > myfiles.md5
// md5sum -c myfiles.md5
