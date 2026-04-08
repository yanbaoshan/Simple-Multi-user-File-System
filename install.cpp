#include <cstdio>
#include <fstring>
#include "filesys.h"

/*
install.cpp - 实现文件系统的安装功能

install()函数负责创建文件系统的磁盘镜像，并初始化超级块、内存索引节点、目录项和用户密码表等数据结构。它首先计算需要的总磁盘空间，然后创建一个全为0的磁盘镜像文件。接着，它初始化超级块中的空闲块链表和空闲索引节点链表，并将这些信息写入磁盘。最后，它设置初始用户密码并关闭文件系统镜像。

install()函数的主要步骤如下：
1. 计算总磁盘空间：根据索引节点块数、数据块数   和其他元数据的大小，计算整个文件系统所需的总磁盘空间。
2. 创建磁盘镜像：使用std::fstream创建一个新的二进制文件，并将其初始化为全0。
3. 初始化超级块：设置超级块中的空闲块链表和空闲索引节点链表，并将这些信息写入磁盘。
4. 初始化用户密码表：为预定义的用户设置初始密码，并将密码表写入磁盘。
5. 关闭文件系统镜像：完成安装后，关闭文件系统镜像文件。
*/

void install(std::fstream & fs)
{
    int i,j;

    /*read the filsys from the superblock*/
    fs.seekp(BLOCKSIZE, std::ios::beg); //超级块位于第1块
    fs.read(reinterpret_cast<char*>(&filsys_instance), sizeof(filsys_instance));

    if (!fs)
    {
       std::cerr << "Error reading superblock during installation." << std::endl;
        return;
    }
    
    /* initialize the inode hash chain*/
    std:fill(std::begin(hinode_table), std::end(hinode_table), hinode{nullptr});

    /*initialize the sys_ofile*/
    for(auto & file : sys_ofile)
    {
        file.f_count = 0;
        file.f_inode = nullptr;
    }
    
    /*initialize the user*/
    for (auto & user : user_table)
    {
        user.u_uid = 0;
        user.u_gid = 0;
        std::fill(std::begin(user.u_ofile), std::end(user.u_ofile), 0);
    }
    

    /*read the main directory to initialize the dir*/
    cur_path_inode = iget(1); //根目录的索引节点号为1
    if(!cur_path_inode)
    {
        std::cerr << "Error reading root directory inode during installation." << std::endl;
        return;
    }
    dir_instance.size = cur_path_inode->di_size/(DIRSIZE + sizeof(uint32_t));

    for(auto & item : dir_instance.items)
    {
        std::fill(std::begin(item.d_name), std::end(item.d_name), ' ');
        item.d_ino = 0;
    }
    
    if (cur_path_inode->di_addr[0] != 0)
    {
        std::streampos block_offset = DATASTART + static_cast<std::streampos>(cur_path_inode->di_addr[0]) * BLOCKSIZE;

        for (i = 0; i < dir_instance.size && i < DIRNUM; i++)
        {
            std::streampos item_offset = static_cast<std::streampos>(i) * (DIRSIZE + sizeof(uint32_t));
            fs.seekg(block_offset + item_offset, std::ios::beg);
            fs.read(reinterpret_cast<char*>(&dir_instance.items[i]), sizeof(direct));

            if (!fs)
            {
                std::cerr << "Error reading directory item during installation." << i << std::endl;
                break;
            }
        }
    }

    // for (i = 0; i < dir_instance.size; i++)
    // {
    //     std::fseek(fd, DATASTART + i * (DIRSIZE + sizeof(uint32_t)), SEEK_SET);
    //     std::fread(&dir_instance.items[i], 1, sizeof(direct), fd);
    // }

    // fseek(fd, DATASTART + BLOCKSIZE * cur_path_inode->di_addr[i], SEEK_SET);
    // fread(&dir_instance.items[i], 1, sizeof(direct), fd);

}