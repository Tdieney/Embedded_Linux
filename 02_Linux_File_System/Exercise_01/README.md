# 📝 BT1:

## 📌 Đề bài:

Viết một chương trình mở một file bất kỳ với cờ `O_APPEND`, sau đó thực hiện `seek` về đầu file rồi ghi một vài dữ liệu vào file đó. Dữ liệu sẽ xuất hiện ở vị trí nào của file và tại sao lại như vậy?

## 💡 Trả lời:

- Khi mở file với `O_APPEND`, dù có gọi `seek()` để di chuyển con trỏ về đầu file, dữ liệu mới ghi vẫn sẽ được thêm vào cuối file.
- 📌 Lý do: `O_APPEND` đảm bảo rằng mọi thao tác ghi luôn được thực hiện ở cuối file, bỏ qua vị trí hiện tại của con trỏ file.

## 🖥️ Code C

Dưới đây là một đoạn mã minh họa cách `O_APPEND` hoạt động:

```c
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
  
    // assume foobar.txt is already created 
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
```

## 📊 Kết quả khi chạy chương trình

### 📂 Trước khi chạy chương trình:

```bash
$ cat test.txt
Initial content of the file.
```

### ▶️ Chạy chương trình:

```bash
$ gcc append_seek.c -o append_seek
$ ./append_seek
```

### 📄 Sau khi chạy chương trình:

```bash
$ cat test.txt
Initial content of the file.
Hello World
Tdieney
```

