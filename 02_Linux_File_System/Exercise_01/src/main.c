// C program to illustrate 
// I/O system Calls 
#include <stdio.h> 
#include <string.h> 
#include <unistd.h> 
#include <fcntl.h> 
  
int main (void) 
{ 
    int fd; 
    int numb_read, numb_write;
    char buf1[12] = "Hello World\n"; 
  
    fd = open("test.txt", O_RDWR | O_CREAT | O_APPEND, 0666);         
    if (-1 == fd)
    {
        printf("open() test.txt failed\n");
    }      

    numb_write = write(fd, buf1, strlen(buf1));
    printf("Write %d bytes to test.txt\n", numb_write);
  
    lseek(fd, 0, SEEK_SET);
    write(fd, "Tdieney\n", strlen("Tdieney\n"));
    
    close(fd); 
  
    return 0; 
} 
