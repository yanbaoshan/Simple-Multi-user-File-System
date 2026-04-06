//filesys.h  - 定义文件系统中的数据结构和常数
#ifndef FILESYS_H
#define FILESYS_H

#include <cstdio>
#include <cstdint>


//常量定义
#define BLOCKSIZE 512   //块大小
#define SYSOPENFIILE 40//最多同时打开40个文件
#define DIRNUM 128      //单个目录最多128个目录项
#define DIRSIZE 14      //目录项中目录名的最大长度
#define PWDSIZE 12      //用户密码最大长度
//#define PWDNUM 32       //用户密码表的大小
#define NOFILE 20       //每个用户最多同时打开20个文件
#define NADDR 10        //索引节点中的直接地址项数量
#define NHINO 128       //内存索引节点hash表大小
#define USERNUM 10      //用户数量
#define DINODESIZE 32   //磁盘索引节点大小

//文件系统布局常量
#define DINODEBLK 32
#define FILEBLK 512
#define NICFREE 50
#define NICINOD 50
#define DINODESTART (2 * BLOCKSIZE)
#define DATASTART ((2 + DINODEBLK) * BLOCKSIZE)

//文件类型 di_mode 
#define DIEMPTY 0000    //空文件
#define DIFILE 01000    //普通文件
#define DIDIR 02000     //目录文件
//权限位
#define UDIREAD 00001
#define UDIWRITE 00002
#define UDIEXICUTE 00004
#define GDIREAD 00010
#define GDIWRITE 00020
#define GDIEXICUTE 00040
#define ODIREAD 00100
#define ODIWRITE 00200
#define ODIEXICUTE 00400
//操作模式
#define READ 1
#define WRITE 2
#define EXICUTE 4

#define DEFAULTMODE 00777   //默认权限

//标志位
#define IUPDATE 00002   //索引节点需要更新
#define SUPDATE 00001   //超级块需要更新
#define FREAD 00001     //文件读打开
#define FWRITE 00002    //文件写打开
#define FAPPEND 00004   //文件追加打开

//错误码
#define DISKFULL 65535  //磁盘已满

//寻址模式
#define SEEK_SET 0  //文件头偏移


//数据结构定义
struct inode; //前向声明
//内存索引节点
struct inode
{
    inode *i_forw;  //hash表前向指针
    inode *i_back;  //hash表后向指针
    char i_flag;    //内存索引节点标志
    uint32_t i_ino;             //磁盘索引节点标志
    uint32_t i_count;           //引用计数
    uint16_t di_number;       //关联文件数，为0时删除该文件
    uint16_t di_mode;         //存取权限
    uint16_t di_uid;          //用户ID
    uint16_t di_gid;          //组ID
    uint32_t di_size;           //文件大小
    uint32_t di_addr[NADDR];    //物理块号
};

//磁盘索引节点
struct dinode
{
    uint16_t di_number;		//关联文件数
    uint16_t di_mode;		//存取权限

    uint16_t di_uid;			
    uint16_t di_gid;
    uint32_t di_size;			//文件大小
    uint32_t di_addr[NADDR];	//物理块号
};

//目录项结构
struct direct
{
    char d_name[DIRSIZE];		//目录名
    uint32_t d_ino;				//目录号
};

//超级块
struct filsys
{
    uint16_t s_isize;			//索引节点区域占用的块数
    uint32_t s_fsize;			//数据区域占用的块数

    uint32_t s_nfree;			//空闲块数量
    uint16_t s_pfree;			//空闲块指针
    uint32_t s_free [NICFREE];	//空闲块栈

    uint32_t s_ninode;			//空闲索引节点数
    uint16_t s_pinode;		//空闲索引节点指针
    uint32_t s_inode [NICINOD];//空闲索引节点数组
    uint32_t s_rinode;			//铭记索引节点

    char s_fmod;					//超级块修改标志
};

//用户密码
struct pwd
{
    uint16_t p_uid; //用户ID
    uint16_t p_gid; //组ID
    char password [PWDSIZE];    //密码
};

//目录
struct dir
{
    direct items[DIRNUM];     
    int32_t size;                       //当前目录大小
};

//查找内存索引节点的hash表
struct hinode
{
    inode * i_forw;          //hash表指针
};

//系统打开表
struct file
{
    char f_flag;					//文件操作标志
    uint32_t f_count;			//引用计数
    inode * f_inode;			//指向内存索引节点
    uint32_t f_off;			//读写指针
};

//用户打开表
struct user
{
    uint16_t u_default_mode;//默认创建掩码
    uint16_t u_uid;			//用户ID
    uint16_t u_gid;			//用户组ID
    uint16_t u_ofile [NOFILE];//用户打开文件表
};

//全局变量声明
extern hinode hinode_table[NHINO];		//内存索引节点hash表
extern dir dir_instance;        //目录实例
extern file sys_ofile[SYSOPENFIILE];		//系统打开文件表
extern filsys filsys_instance;				//超级块实例
extern pwd pwd_table[USERNUM];				//用户密码表
extern user user_table[USERNUM];			//当前用户表
extern std::FILE *fd;    /* the file system column of all the system */
extern inode * cur_path_inode;		//当前路径的内存索引节点
extern int user_id;
extern uint32_t file_block; 

//函数原型声明
extern inode * iget();
extern void iput();
extern unsigned int balloc();
extern void bfree();
extern inode * ialloc();
extern void ifree();
extern unsigned int namei();
extern unsigned int iname();
extern unsigned int access();
extern void _dir();
extern void mkdir(const char *pathname);
extern void chdir(const char *pathname);
extern void dirlt();
extern unsigned short open(const char *pathname, uint16_t mode);
extern unsigned short create(const char *pathname, uint16_t mode);
extern void remove_file(const char *pathname);
extern uint32_t read(unsigned short fd, char * buf, uint32_t count);
extern uint32_t write(unsigned short fd, const char * buf, uint32_t count);
extern int login(uint16_t uid, const char * pwd);
extern void logout();
extern void install();
extern void format();
extern void close(unsigned short fd);
extern void halt();

#endif
