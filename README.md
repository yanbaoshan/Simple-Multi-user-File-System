# Simple-Multi-user-File-System - 简易多用户文件系统

> 一个基于C++实现的模拟多用户文件系统，参考UNIX/Linux设计思想，支持用户权限管理、虚拟磁盘映射及层级目录结构。

## 项目简介

该文件系统为用户提供一个解释执行相关命令的环境，模拟了操作系统重文件管理的核心流程。

---

## 1. 设计思想
本文件系统的设计参考了经典UNIX文件系统的架构：

- **虚拟磁盘映射**：系统在宿主机上创建一个二进制文件作为“虚拟磁盘”。磁盘被划分为**超级块区**、**索引节点区**和**数据区**。
- **基于INode的文件管理**：每个文件（或目录）对应一个索引节点，记录文件的元数据（权限、所有者、物理块指针等）。
- **目录结构**：
    * 采用两级目录，其中第一级对应用户账号，第二级对应用户账号下面的文件。
- **权限控制**：通过`di_mode`字段实现读、写、执行权限的位掩码控制，结合`di_uid`和`di_gid`确保用户隔离。
> **注意**：为了简单，本文件系统未考虑文件共享、文件系统安全以及管道文件与设备文件等特殊内容。


---


## 2. 主要数据结构

### 2.1 索引节点
用于描述文件元数据，内存中通过链表管理。
```cpp
struct inode
{
　　struct inode * i_flow;
　　struct inode * i_back;
　　char i_flag;
　　unsigned int i_ino;              //磁盘索引节点标号
　　unsigned int i_count;			//引用计数
　　unsigned short di_number;		//关联文件数，为0时删除该文件
　　unsigned short di_mode;		//存取权限
　　unsigned short di_uid;			//磁盘索引节点用户ID
　　unsigned short di_gid;			//磁盘索引节点组ID
　　unsigned int di_addr [NADDR];	//物理块号
};
```
### 2.2 超级块
用于管理整个文件系统的资源，记录空闲块和空闲索引节点。
```cpp
struct filsys
{
　　unsigned short s_isize;			//索引节点块块数
　　unsigned long s_fsize;			//数据块块数
　　
　　unsigned int s_nfree;			//空闲块块数
　　unsigned short s_pfree;			//空闲块指针
　　unsigned int s_free [NICFREE];	//空闲块堆栈
　　
　　unsigned int s_ninode;			//空闲索引节点数
　　unsigned short s_pinode;		//空闲索引节点指针
　　unsigned int s_inode [NICINOD];//空闲索引节点数组
　　unsigned int s_rinode;			//铭记索引节点
　　
　　char s_fmod;					//超级块修改标志
};
```
## 3. 主要函数
函数功能按模块划分，主要包含以下核心函数：

| 功能模块      | 函数名 | 功能描述 |
| :---         | :--- | :--- |
| **系统控制** | `format()` | 格式化虚拟磁盘，初始化超级块 |
| | `install()` | 加载文件系统，进入系统环境 |
| | `halt()` | 退出文件系统，同步数据到磁盘 |
| **存储管理** | `balloc()`, `bfree()` | 磁盘物理块的分配与释放 |
| | `ialloc()`, `ifree()` | 索引节点区的分配与释放 |
| **节点管理** | `iget()`, `iput()` | 内存索引节点的获取与释放 |
| **目录操作** | `mkdir()`, `chdir()` | 创建目录与切换当前目录 |
| | `namei()`, `iname()` | 路径解析与当前目录文件搜索 |
| | `_dir()` | 显示当前目录下的文件列表 |
| **文件操作** | `create()`, `delete()` | 创建文件与删除文件 |
| | `open()`, `close()` | 打开文件与关闭文件 |
| | `read()`, `write()` | 文件内容的读取与写入 |
| **用户管理** | `login()`, `logout()` | 用户登录与注销 |
| | `access()` | 检查用户对文件的操作权限 |


## 4.主程序说明
主程序执行流程如下：
### Begin
- **Step 1** ：对磁盘进行格式化
- **Step 2** ：调用install()，进入文件系统
- **Step 3** ：调用_dir()，显示当前目录
- **Step 4** ：调用login()，用户注册
- **Step 5** ：调用mkdir()和chdir()创建目录
- **Step 6** ：调用create()，创建文件0
- **Step 7** ：分配缓冲区
- **Step 8** ：写文件0
- **Step 9** ：关闭文件0并释放缓冲区
- **Step 10** ：调用mkdir()和chdir()创建子目录
- **Step 11** ：调用create()，创建文件1
- **Step 12** ：分配缓冲区
- **Step 13** ：写文件1
- **Step 14** ：关闭文件1并释放缓冲区
- **Step 15** ：调用chdir()将当前目录移到上一级
- **Step 16** ：调用create()，创建文件2
- **Step 17** ：分配缓冲区
- **Step 18** ：调用write()，写文件2
- **Step 19** ：关闭文件2并释放缓冲区
- **Step 20** ：调用delete()，删除文件0
- **Step 21** ：调用create()，创建文件3
- **Step 22** ：为文件3分配缓冲区
- **Step 23** ：调用write()，写文件3
- **Step 24** ：关闭文件3并释放缓冲区
- **Step 25** ：调用open()，打开文件2
- **Step 26** ：为文件2分配缓冲区
- **Step 27** ：写文件3后关闭文件3
- **Step 28** ：释放缓冲区
- **Step 29** ：用户退出
- **Step 30** ：关闭
### End
