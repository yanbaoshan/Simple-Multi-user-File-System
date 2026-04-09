#include <iostream>
#include <cstring>
#include <fstream>
#include "filesys.h"

//format.cpp - 实现文件系统的格式化功能

void format()
{
    inode * inop;
    //计算一块能放多少目录项
    direct dir_buf[BLOCKSIZE /(DIRSIZE + sizeof(uint32_t))];

    uint32_t block_buf[BLOCKSIZE / sizeof(uint32_t)];
    char * buf = nullptr;
    int i,j;

    //内存分配与初始化
    size_t total_size = (DINODEBLK + FILEBLK + 2) * BLOCKSIZE;
    buf = new char[total_size];
    if (buf == NULL)
    {
        std::cerr << "Failed to create file system." << std::endl;
        exit(EXIT_FAILURE);
    }
    std::memset(buf, 0, total_size);
    //打开文件
    std::fstream fs("filesystem", std::ios::out | std::ios::binary | std::ios::trunc);
    
    if (!fs.is_open())
    {
        std::cerr << "Failed to create file system image." << std::endl;
        delete[] buf;
        exit(EXIT_FAILURE);
    }
    //初始化磁盘镜像全为0
    fs.write(buf, total_size);
    //辅助lambda函数，安全地复制字符串，确保不会发生缓冲区溢出
    auto safe_strcpy = [](char * dest, const char * src, size_t max_len) {
        std::strncpy(dest, src, max_len - 1);
        dest[max_len - 1] = '\0'; 
    };

    //初始化密码表
    pwd_table[0].p_uid = 2116;
    pwd_table[0].p_gid = 03;
    safe_strcpy(pwd_table[0].password, "123456", sizeof(pwd_table[0].password));
    pwd_table[1].p_uid = 2117;
    pwd_table[1].p_gid = 03;
    safe_strcpy(pwd_table[1].password, "123456", sizeof(pwd_table[1].password));
    pwd_table[2].p_uid = 2118;
    pwd_table[2].p_gid = 04;
    safe_strcpy(pwd_table[2].password, "123456", sizeof(pwd_table[2].password));
    pwd_table[3].p_uid = 2119;
    pwd_table[3].p_gid = 04;
    safe_strcpy(pwd_table[3].password, "123456", sizeof(pwd_table[3].password));
    pwd_table[4].p_uid = 2120;
    pwd_table[4].p_gid = 05;
    safe_strcpy(pwd_table[4].password, "123456", sizeof(pwd_table[4].password));

    //创建目录结构 create the main directory and its sub dir etc and the file password
    //0号节点
    inop = iget(0); //0 empty dinode id
    if(inop)
    {
        inop->di_number = DIEMPTY;
        iput(inop);
    }
    //1号节点
    inop = iget(1); //1 main directory id
    if(inop)
    {
        inop->di_number = 1;
        inop->di_mode = DEFAULTMODE | DIDIR;
        inop->di_size = 3 * (DIRSIZE + sizeof(uint32_t));
        inop->di_addr[0] = 0; //block 0# is used by the main directory
        safe_strcpy(dir_buf[0].d_name, "..", DIRSIZE);
        dir_buf[0].d_ino = 1;
        safe_strcpy(dir_buf[1].d_name, ".", DIRSIZE);
        dir_buf[1].d_ino = 1;
        safe_strcpy(dir_buf[2].d_name, "etc", DIRSIZE);
        dir_buf[2].d_ino = 2;
        //定位到数据区起始（第0块）
        fs.seekp(DATASTART, std::ios::beg);
        fs.write(reinterpret_cast<char*>(dir_buf), inop->di_size);
        iput(inop);
    }
    //2号节点(/etc) -> 物理块1
    inop = iget(2); //2 etc directory id
    if(inop)
    {
        inop->di_number = 1;
        inop->di_mode = DEFAULTMODE | DIDIR;
        inop->di_size = 3 * (DIRSIZE + sizeof(uint32_t));
        inop->di_addr[0] = 1; //block 0# is used by the etc directory
        safe_strcpy(dir_buf[0].d_name, "..", sizeof(dir_buf[0].d_name));
        dir_buf[0].d_ino = 1;
        safe_strcpy(dir_buf[1].d_name, ".", sizeof(dir_buf[1].d_name));
        dir_buf[1].d_ino = 2;
        safe_strcpy(dir_buf[2].d_name, "password", sizeof(dir_buf[2].d_name));
        dir_buf[2].d_ino = 3;
        //定位到数据区起始 + 1 块（第1块）
        fs.seekp(DATASTART + BLOCKSIZE, std::ios::beg);
        fs.write(reinterpret_cast<char*>(dir_buf), inop->di_size);
        // fseek(fd, DATASTART + BLOCKSIZE * 1, SEEK_SET);
        // std::fwrite(dir_buf, 1, 3 * (DIRSIZE + sizeof(uint32_t)), fd);
        iput(inop);
    }

    inop = iget(3); //3 password id
    if(inop)
    {   
        inop->di_number = 1;
        inop->di_mode = DEFAULTMODE | DIFILE;
        inop->di_size = BLOCKSIZE;
        inop->di_addr[0] = 2; //block 0# is used by the password file
        
        for ( i = 5; i < USERNUM; i++)
        {
            pwd_table[i].p_uid = 0;
            pwd_table[i].p_gid = 0;
            safe_strcpy(pwd_table[i].password, "      ", PWDSIZE);
        }
        //定位到数据区起始 + 2 块（第2块）
        fs.seekp(DATASTART + BLOCKSIZE * 2, std::ios::beg);
        fs.write(reinterpret_cast<char*>(pwd_table), BLOCKSIZE);
        // fseek(fd, DATASTART + BLOCKSIZE * 2, SEEK_SET);
        // std::fwrite(pwd_table, 1, BLOCKSIZE, fd);
        iput(inop);}

    //2.初始化超级块
    filsys_instance.s_isize = DINODEBLK;
    filsys_instance.s_fsize = FILEBLK;
    filsys_instance.s_ninode = DINODEBLK * BLOCKSIZE / DINODESIZE - 4;
    filsys_instance.s_nfree = FILEBLK - 3;
    //初始化空闲索引节点栈
    for (i = 0; i < NICINOD; i++)
    {
        //begin with 4,0,1,2,3 is used by main ,etc password
        filsys_instance.s_inode[i] = i + 4;
    }
    filsys_instance.s_pinode = 0;
    filsys_instance.s_rinode = NICINOD + 4;

    block_buf[NICFREE - 1] = FILEBLK + 1;//FILEBLK + 1 is a flag of end
    for (i = 0; i < NICFREE - 1; i++)
    {
        block_buf[NICFREE - 2 - i] = FILEBLK + 1;
    }
    fseek(fd, DATASTART + BLOCKSIZE * (FILEBLK - NICFREE - 1), SEEK_SET);
    std::fwrite(block_buf, 1, BLOCKSIZE, fd);

    for (i = FILEBLK - NICFREE - 1; i > 2; i -= NICFREE)
    {
        for (j = 0; j < NICFREE; j++)
        {
            block_buf[j] = i - j;
        }
        block_buf[j] = 50;
        fseek(fd, DATASTART + BLOCKSIZE * (i - 1), SEEK_SET);
        std::fwrite(block_buf, 1, BLOCKSIZE, fd);
    }

    j = i + NICFREE;
    for (i = j; i > 2; i--)
    {
        filsys_instance.s_free[NICFREE - 1 + i - j] = i;
    }
    filsys_instance.s_pfree = NICFREE - 1 - j + 3;
    filsys_instance.s_pinode = 0;

    fs.seekp(BLOCKSIZE, std::ios::beg); //超级块位于第1块
    fs.write(reinterpret_cast<char*>(&filsys_instance), sizeof(filsys_instance));

    delete[] buf;
    fs.close();
}