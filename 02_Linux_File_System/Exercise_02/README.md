# 📝 BT2:&#x20;

## 📌 Đề bài:

Sau mỗi lệnh write dưới đây, Cho biết nội dung sẽ được ghi vào file nào, nội dung là gì và giải thích tại sao?

```c
fd1 = open(file, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
fd3 = open(file, O_RDWR);
write(fd1, "Hello,", 6);
write(fd2, "world", 6);
lseek(fd2, 0, SEEK_SET);
write(fd1, "HELLO,", 6);
write(fd3, "Gidday", 6);
```

---

## 🔍 Trả lời:

1. **`fd1 = open(file, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);`**

   - `O_RDWR` cho phép đọc và ghi file
   - `O_CREAT` tạo file nếu file chưa tồn tại
   - `O_TRUNC` sẽ xóa toàn bộ nội dung regular file nếu nó đã tồn tại và access mode cho phép ghi (tức là O_RDWR hoặc O_WRONLY) nó sẽ bị cắt thành độ dài 0. Nếu file là 1 FIFO hoặc terminal device file, `O_TRUNC` sẽ bị ignore. Nếu không, hiệu ứng của `O_TRUNC` sẽ không được chỉ định.
   - `S_IRUSR` 00400 user có quyền đọc
   - `S_IWUSR` 00200 user có quyền ghi

2. **`fd3 = open(file, O_RDWR);`**

   - Mở cùng file đó với quyền đọc/ghi.
   - `fd1` và `fd3` trỏ đến cùng một file nhưng có con trỏ tệp riêng biệt.

3. **`write(fd1, "Hello,", 6);`**

   - Ghi "Hello," (6 byte) vào file.
   - Con trỏ tệp `fd1` lúc này di chuyển đến vị trí 6.
   - Nội dung file sau bước này: `Hello,`

4. **`write(fd2, "world", 6);`**

   - **Error!** Lỗi xảy ra do `fd2` chưa được open() nên không thể write()
   - Nội dung file sau bước này không đổi: `Hello,`

5. **`lseek(fd2, 0, SEEK_SET);`**

   - **Error!** Lỗi xảy ra do `fd2` không tồn tại.

6. **`write(fd1, "HELLO,", 6);`**

   - `fd1` vẫn đang ở vị trí 6, ghi "HELLO," từ đó.
   - `O_TRUNC` chỉ có hiệu lực khi mở file. Sau khi file đã được mở, O_TRUNC không còn ảnh hưởng đến các thao tác ghi nữa.
   - Nội dung file sau bước này: `Hello,HELLO,`

7. **`write(fd3, "Gidday", 6);`**

   - `fd3` mở cùng file, nhưng con trỏ của nó chưa được dịch chuyển.
   - Vì vậy, "Gidday" sẽ ghi đè từ đầu file.
   - Nội dung cuối cùng của file: `GiddayHELLO,`

