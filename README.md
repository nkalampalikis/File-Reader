# Linux-File-Reader
A program in C that given any list of files can display statistics of all the files such readability, directories, total byte size, and total time of computation.

The input of the program is any file name or many. They can be pipelined throught the terminal with the unix commands.

The output of the program is to print out the following information:
• total number of “bad files.” These are file names causing stat() to return an error.
• total number of directories.
• total number of “regular” files. These are non-directory and non-special files. Most
  files are of this type.
• total number of “special” files. There are three types—block, character and fifo. If a
  file is not a directory or a regular file then it is a special file. For example files in the
  directory /dev/ are often special types of files as they refer to devices.
• total number of bytes used by regular files. 
• total number of regular files that contain all text.
• total number of bytes used by text files.

In order to make the program, once you are in the same directory with all the files type in the linux terminal:

linux> make

Then you are ready to run the program.

Example Input:

linux> ls -1d /dev/* | ./proj4

Example Output:

linux> Bad Files: 0

linux> Directories: 3

linux> Regular Files: 3

linux> Special Files: 140

linux> Regular File Bytes: 32881

linux> Text Files: 2

linux> Text File Bytes: 16541


In the serial architecture version the program (the default), the program will read one
file name from input, process it and then continue until all input file names are processed.
The program should continue to read file names until an end-of-file (EOF) is detected on
input.

Other than the default serial architecture, there is implemented a multi-threaded one that can handle many processes at once. With a limit of a maximum 15 threads.The making of the program is the same for both implementations,as well as the output. The only difference is the input which is as follows:

linux> ls -1d * | ./proj4 thread 10

The last number indicates the number of threads to be created.

In the output for both architectures time statistics are provided, also there is a written report provided commenting on the performance for both architectures.


