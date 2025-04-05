// C program to illustrate 
// I/O system Calls 
#include <stdio.h> 
#include <string.h> 
#include <unistd.h> 
#include <fcntl.h> 
  
int main (void) 
{ 
    int fd1, fd3; 
    char file[8] = "test.txt";

    fd1 = open(file, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    fd3 = open(file, O_RDWR);
    write(fd1, "Hello,", 6);
    // write(fd2, "world", 6);
    // lseek(fd2, 0, SEEK_SET);
    write(fd1, "HELLO,", 6);
    write(fd3, "Gidday", 6);

    
    close(fd1); 
    close(fd3); 
  
    return 0; 
} 
