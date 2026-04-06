#include <string>
#include <cstdio>
#include <iostream>
#include "filesys.h"

//全部变量
hinode hinode_table[NHINO]; //内存索引节点hash表 
dir dir_instance; //当前目录
file sys_ofile[SYSOPENFIILE]; //系统打开文件表
filsys filsys_instance; //文件系统超级块
pwd pwd_table[USERNUM]; //用户密码表
user user_table[USERNUM]; //用户表
std::FILE * fd = nullptr; //文件系统文件指针
inode * cur_path_inode = nullptr; //当前路径的内存索引节点
int user_id = 0;
uint32_t file_block = 0;

int main()
{
    unsigned short ab_fd1, ab_fd2, ab_fd3, ab_fd4;
    unsigned short bhy_fd1;
    char *buf;

    std::cout << "\nDo you want to format the disk? (y/n): \n";
    
    char choice;
    std::cin >> choice;

    if (choice == 'y')
    {
        std::cout << "Format will erase all content on the disk. Are you sure? (y/n): \n";
        std::cin.ignore(); //清除输入缓冲区
        std::cin >> choice;
        
        if (choice != 'y')
        {
            return 0;
        }

        format();
        install();
        
    }
    else
        return 0;
    
    _dir();

    login(2118, "123456");
    user_id = 0; //默认登录为用户0

    mkdir("a2118");
    chdir("a2118");
    ab_fd1 = create("file0.c", 01777);
    file_block = BLOCKSIZE * 6 + 5; 
    buf = new char[file_block];
    write(ab_fd1, buf, file_block);
    close(ab_fd1); 
    delete[] buf;

    mkdir("subdir");
    chdir("subdir");
    ab_fd2 = create("file1.c", 01777);
    file_block = BLOCKSIZE * 4 + 20;
    buf = new char[file_block];
    write(ab_fd2, buf, file_block);
    close(ab_fd2);
    delete[] buf;

    chdir("..");
    ab_fd3 = create("file2.c", 01777);
    file_block = BLOCKSIZE * 3 + 255;
    buf = new char[file_block];
    write(ab_fd3, buf, file_block);
    close(ab_fd3);
    delete[] buf;

    _dir();
    remove_file("ab_file0.c");

    ab_fd4 = create("file3.c", 01777);
    file_block = BLOCKSIZE * 8 + 300;
    buf = new char[file_block];
    write(ab_fd4, buf, file_block);
    close(ab_fd4);
    delete[] buf;

    _dir();
    ab_fd3 = open("file2.c", FAPPEND);
    file_block = BLOCKSIZE * 3 + 100;
    buf = new char[file_block];
    write(ab_fd3, buf, file_block);
    close(ab_fd3);
    delete[] buf;

    _dir();
    chdir("..");
    logout();
    halt();

}

