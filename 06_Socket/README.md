# SOCKET

## 1. Introduction to Sockets

### 1.1 Definition
- A **Socket** is a communication mechanism that allows processes to communicate with each other, whether they are running on the same device or on different devices.
- A socket is represented by a **file descriptor**.
- Sockets are used to transmit information within a file. This information includes:
  - **Domain**: The communication scope (Internet or UNIX).
  - **Type**: The socket type (Stream or Datagram).
  - **Protocol**: The protocol used (TCP, UDP, etc.).
<p align="center">
    <img src="https://github.com/user-attachments/assets/9150dc0d-bff6-42f1-9f80-0d0140c2f8de" width="600">
</p>
### 1.2 Domain
The domain defines the communication scope that the socket operates within:
- **Internet Domain**: Used for communication over the Internet, supporting:
  - IPv4 (`AF_INET`)
  - IPv6 (`AF_INET6`)
- **UNIX Domain**: Used for communication between processes on the same system (`AF_UNIX`).

**Illustration: Domain Hierarchy**

```
Domain
├── Unix Domain
└── Internet Domain
    ├── IPv4
    └── IPv6
```

### 1.3 Type
Sockets are categorized into two main types:
- **Stream Socket** (TCP):
  - Data is transmitted as a **byte stream**.
  - Ensures data is delivered in order, without loss, and includes error reporting.
- **Datagram Socket** (UDP):
  - Data is transmitted as **datagrams**.
  - Does not guarantee order, data may be lost, and there is no error reporting.

**Comparison Table: Stream Socket vs. Datagram Socket**

| **Criteria**              | **Stream Socket**                          | **Datagram Socket**                      |
|---------------------------|--------------------------------------------|------------------------------------------|
| **Characteristics**       | Ensures order, no data loss, error reporting | No order guarantee, possible data loss  |
| **Connection Requirement**| Requires connection setup                 | No connection setup required            |
| **Data Transmission**     | Byte stream                               | Datagram packets                        |

### 1.4 Protocol
- The protocol specifies the communication protocol used by the socket. Examples include:
  - `AF_INET` (IPv4 Internet protocols)
  - `AF_INET6` (IPv6 Internet protocols)
  - `AF_APPLETALK` (AppleTalk protocols)
  - `AF_PACKET` (Low-level packet interface)
- Each Domain and Type has a corresponding list of protocols. If no protocol is specified, the system selects a default protocol (typically 0).

**List of Common Protocols**

| **Name**      | **Purpose**                          | **Man Page**  |
|---------------|--------------------------------------|---------------|
| `AF_UNIX`     | Local communication                 | `unix(7)`     |
| `AF_INET`     | IPv4 Internet protocols             | `ip(7)`       |
| `AF_INET6`    | IPv6 Internet protocols             | `ipv6(7)`     |
| `AF_PACKET`   | Low-level packet interface          | `packet(7)`   |
| `AF_RD`       | Reliable Datagram Sockets (RDS)     | `rds(7)`      |

### 1.5 File Descriptor, Open File Table, I-node Table
- **File Descriptor Table**: A table containing pointers to entries in the **Open File Table**.
- **Open File Table**: Stores information about open files, including their status and pointers to the **I-node Table**.
- **I-node Table**: Contains detailed file information (e.g., data location, read/write permissions, open/closed status, etc.).

**Illustration: Relationship Between File Descriptor, Open File Table, and I-node Table**

```
File Descriptor Table → Open File Table → I-node Table
```

- **Questions in the I-node Table**:
  - Which device is it connected to?
  - What are the read/write permissions?
  - Is the data closed or open?

---

## 2. Socket Workflow

### 2.1 Workflow of Stream Socket (TCP)
- **Stream Sockets** require a connection to be established before data transmission.
- The process initiating the connection is the **client**, while the process receiving the connection is the **server**.

**Workflow**

<p align="center">
    <img src="https://github.com/user-attachments/assets/2546ffcd-cbcc-40a9-b30a-5b7bd719f20c" width="500">
</p>

### 2.2 Workflow of Datagram Socket (UDP)
- **Datagram Sockets** do not require a connection to be established; both the server and client can send/receive data immediately.

**Workflow**

<p align="center">
    <img src="https://github.com/user-attachments/assets/5a790cf6-5473-4dde-9ac6-38c067490536
" width="500">
</p>
---

## 3. Sockets: Internet Domain Socket

### 3.1 Internet Socket Address
- The **Internet Socket Address** is defined by the `struct sockaddr` structure:
  - **Domain**: `AF_INET` (IPv4) or `AF_INET6` (IPv6).
  - Sockets use a single data type: `sockaddr`.
- To use the correct domain, the `struct sockaddr` must be cast to:
  - **IPv4**: `struct sockaddr_in`
  - **IPv6**: `struct sockaddr_in6`

#### 3.1.1 IPv4 Socket Address
Structure of `struct sockaddr_in`:
```c
struct sockaddr_in {
    sa_family_t sin_family;     // Address family (AF_INET)
    in_port_t sin_port;         // Port number
    struct in_addr sin_addr;    // IPv4 address
    unsigned char __pad[X];     // Padding to match the size of 16 bytes
};

struct in_addr {
    in_addr_t s_addr;           // IPv4 4-byte address (32-bit)
};
```

#### 3.1.2 IPv6 Socket Address
Structure of `struct sockaddr_in6`:
```c
struct sockaddr_in6 {
    sa_family_t sin6_family;    // Address family (AF_INET6)
    in_port_t sin6_port;        // Port number
    uint32_t sin6_flowinfo;     // IPv6 flow information
    struct in6_addr sin6_addr;  // IPv6 address
    uint32_t sin6_scope_id;     // Scope ID (new in kernel 2.4)
};

struct in6_addr {
    uint8_t s6_addr[16];        // IPv6 16-byte address (128-bit)
};
```

### 3.2 Address Conversion for Sockets
- Internet Socket addresses are defined by an **IP address** and **port**, with data stored as **integers**.
- Different devices store data in different orders:
  - **Big-endian**: Most significant byte (MSB) is stored first.
  - **Little-endian**: Least significant byte (LSB) is stored first.
- Sockets use a common convention (typically **Big-endian**) to avoid conflicts.

**Illustration: Address Conversion**

| **Big-endian** (MSB → LSB) | **Little-endian** (LSB → MSB) |
|----------------------------|-------------------------------|
| N, N+1 (MSB, LSB)         | N, N+1 (LSB, MSB)            |

### 3.3 Functions Used for Address Conversion
Functions in `<arpa/inet.h>` are used to convert between **host byte order** and **network byte order**:

- **Convert from host to network byte order**:
  ```c
  #include <arpa/inet.h>
  uint16_t htons(uint16_t host_uint16);
  // Returns host_uint16 converted to network byte order

  uint32_t htonl(uint32_t host_uint32);
  // Returns host_uint32 converted to network byte order
  ```

- **Convert from network to host byte order**:
  ```c
  #include <arpa/inet.h>
  uint16_t ntohs(uint16_t net_uint16);
  // Returns net_uint16 converted to host byte order

  uint32_t ntohl(uint32_t net_uint32);
  // Returns net_uint32 converted to host byte order
  ```

---

## 4. Sockets: Unix Domain Socket

### 4.1 Unix Socket Address
- **Unix Sockets** facilitate communication between processes on the same device but cannot transmit data over a network like Internet Sockets.
- **Domain**: `AF_UNIX`.
- **Type**: Supports two socket types:
  - `SOCK_STREAM` (stream)
  - `SOCK_DGRAM` (datagram)
- **Protocol**: Always 0, e.g., `socket(AF_UNIX, SOCK_STREAM, 0)`.

#### Unix Socket Address Structure
- After calling `bind()` to assign an address to the socket, a socket file is created based on the `path_name`:
  ```c
  struct sockaddr_un {
      sa_family_t sun_family;     // Always AF_UNIX
      char sun_path[108];         // Null-terminated socket pathname
  };
  ```

#### Key Notes
- A socket cannot be bound to a `path_name` that already exists.
- The `path_name` can be:
  - An absolute path (`/home/user/socket`)
  - A relative path (`./socket`)
- Although a socket is represented by a socket file, it cannot be opened using `open()` to establish a connection.
- After the socket is closed or the program terminates, the `path_name` file persists. To remove it, use `unlink()` or `remove()`.

#### Access Permissions
- To connect or send data to a socket, the process must have **write** permissions for the `path_name` file.
- The `bind()` command creates the socket file with permissions determined by `umask()`, or permissions can be modified by changing the directory containing the socket file.

---

## Summary
- **Sockets** are a network communication tool, supporting both **Internet Domain** (TCP/UDP) and **Unix Domain**.
- **Internet Sockets**:
  - Support IPv4 (`AF_INET`) and IPv6 (`AF_INET6`).
  - Require address conversion between host and network byte order (using `htons`, `ntohl`, etc.).
- **Unix Sockets**:
  - Used for communication on the same device (`AF_UNIX`).
  - Use `struct sockaddr_un` to assign addresses via `path_name`.
- **Workflow**:
  - Stream Socket (TCP): Requires a connection setup.
  - Datagram Socket (UDP): Does not require a connection setup.
