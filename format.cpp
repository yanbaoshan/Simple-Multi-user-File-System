#include <iostream>
#include "filesys.h"

//format.cpp - 实现文件系统的格式化功能

void format()
{
    inode * inode;
    direct dir_buf[BLOCKSIZE /(DIRSIZE + 2)];

    struct filsys;
    uint32_t block_buf[BLOCKSIZE / sizeof(uint32_t)];
    char * buf = nullptr;
    int i,j;

    //creatre the file system
    fd = std::fopen("filesystem", "r+w+b");
    buf = new char[(DINODEBLK + FILEBLK + 2) * BLOCKSIZE * sizeof(char)];
    if (buf == NULL)
    {
        std::cerr << "Failed to create file system." << std::endl;
        exit(0);
    }
    fseek(fd, 0, SEEK_SET);
    std::fwrite(buf, 1, (DINODEBLK + FILEBLK + 2) * BLOCKSIZE * sizeof(char), fd);

    //0.initialize the password
    pwd_table[0].p_uid = 2116;
    pwd_table[0].p_gid = 03;
    std::strcpy(pwd_table[0].password, "123456");
    pwd_table[1].p_uid = 2117;
    pwd_table[1].p_gid = 03;
    strcpy(pwd_table[1].password, "123456");
    pwd_table[2].p_uid = 2118;
    pwd_table[2].p_gid = 04;
    strcpy(pwd_table[2].password, "123456");
    pwd_table[3].p_uid = 2119;
    pwd_table[3].p_gid = 04;
    strcpy(pwd_table[3].password, "123456");
    pwd_table[4].p_uid = 2120;
    pwd_table[4].p_gid = 05;
    strcpy(pwd_table[4].password, "123456");

    //create the main directory and its sub dir etc and the file password
    inode = iget(0); //0 empty dinode id
    inode->di_number = DIEMPTY;
    iput(inode);

    inode = iget(1); //1 main directory id
    inode->di_number = 1;
    inode->di_mode = DEFAULTMODE|DIDIR;
    inode->di_size = 3 * (DIRSIZE + 2);
    inode->di_addr[0] = 0; //block 0# is used by the main directory
    strcpy(dir_buf[0].d_name, "..");
    dir_buf[0].d_ino = 1;
    strcpy(dir_buf[1].d_name, ".");
    dir_buf[1].d_ino = 1;
    strcpy(dir_buf[2].d_name, "etc");
    dir_buf[2].d_ino = 2;
    fseek(fd, DATASTART, SEEK_SET);
    std::fwrite(dir_buf, 1, 3 * (DIRSIZE + 2), fd);
    iput(inode);

    inode = iget(2); //2 etc directory id
    inode->di_number = 1;
    inode->di_mode = DEFAULTMODE|DIDIR;
    inode->di_size = 3 * (DIRSIZE + 2);
    inode->di_addr[0] = 1; //block 0# is used by the etc directory
    strcpy(dir_buf[0].d_name, "..");
    dir_buf[0].d_ino = 1;
    strcpy(dir_buf[1].d_name, ".");
    dir_buf[1].d_ino = 2;
    strcpy(dir_buf[2].d_name, "password");
    dir_buf[2].d_ino = 3;
    fseek(fd, DATASTART + BLOCKSIZE * 1, SEEK_SET);
    std::fwrite(dir_buf, 1, 3 * (DIRSIZE + 2), fd);
    iput(inode);

    inode = iget(3); //3 password id
    inode->di_number = 1;
    inode->di_mode = DEFAULTMODE|DIDIR;
    inode->di_size = BLOCKSIZE;
    inode->di_addr[0] = 2; //block 0# is used by the password file
    for ( i = 5; i < USERNUM; i++)
    {
        pwd_table[i].p_uid = 0;
        pwd_table[i].p_gid = 0;
        strcpy(pwd_table[i].password, "      ");
    }
    fseek(fd, DATASTART + BLOCKSIZE * 2, SEEK_SET);
    std::fwrite(pwd_table, 1, BLOCKSIZE, fd);
    iput(inode);

    //2.initialize the super block
    filsys_instance.s_isize = DINODEBLK;
    filsys_instance.s_fsize = FILEBLK;
    filsys_instance.s_ninode = DINODEBLK * BLOCKSIZE / DINODEBLK - 4;
    filsys_instance.s_nfree = FILEBLK - 3;

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

    fseek(fd, BLOCKSIZE, SEEK_SET);
    std::fwrite(&filsys_instance, 1, sizeof(filsys), fd);
    fseek(fd, BLOCKSIZE, SEEK_SET);
    std::fread(&filsys_instance.s_isize, 1, sizeof(filsys), fd);
}