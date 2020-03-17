// 班级：F1703603
// 姓名：李嘉琦
// 学号：517021910814
// 循环首次适应法 main.c

#include <stdio.h>
#include <stdlib.h>

struct map // to store free memory and size
{
    char *m_addr;
    unsigned m_size;
    struct map *prior, *next;
};

struct used // to store used memory location and size 
{
    char *m_addr;
    unsigned m_size;
    struct used *next;
};

char *mp; // refer to memory pointer
struct map *coremap; // refer to list pointer
struct used *usedmap; // refer to used memory list pointer
const int MAX_MEMORY_SIZE = 200; // total memory size(bytes)

void initialize();
void end();
void display();
char *lmalloc(unsigned size);
void lfree(unsigned size, char *addr);

int main()
{
    initialize();

    char str[100];
    while (1)
    {
        printf("please input your opcode(1 for malloc, 2 for free, 3 for display, 0 for exit): ");
        gets(str);
        if (str[0] == '1') // lmalloc
        {
            FILE *f = fopen("log.txt", "a");
            printf("please input the size(bytes): ");
            gets(str);
            unsigned size = 0;
            for (int i = 0; i < 100; ++i)
            {
                if (str[i] < '0' || str[i] > '9')
                    break;
                size *= 10;
                size += str[i] - '0';
            }
            char *loc = lmalloc(size);
            fprintf(f, "operation: lmalloc(%lu).\n", size);
            if (loc == -1) // request denied
            {
                printf("Warning: No Enough Memory!\n");
                fprintf(f, "Warning: No Enough Memory!\n");
                fclose(f);
                continue;
            }
            else
            {
                printf("the address is %lu\n", loc);
                fprintf(f, "the address is %lu\n", loc);
                fclose(f);
            }
            struct used *tmp = usedmap; // add a new element to used memory list
            while(tmp->next != NULL)
                tmp = tmp->next;
            tmp->next = (struct used *)malloc(sizeof(struct used));
            tmp = tmp->next;
            tmp->m_addr = loc;
            tmp->m_size = size;
            tmp->next = NULL;
        }
        else if (str[0] == '2') // lfree
        {
            FILE *f = fopen("./log.txt", "a");
            printf("please input the address: ");
            gets(str);
            unsigned addr = 0;
            for (int i = 0; i < 100; ++i)
            {
                if (str[i] < '0' || str[i] > '9')
                    break;
                addr *= 10;
                addr += str[i] - '0';
            }
            fprintf(f, "operation: lfree(%lu).\n", addr);
            struct used *p1 = usedmap, *p2 = usedmap->next; // look up in used memory list to get the size
            while (p2 != NULL)
            {
                if ((unsigned)p2->m_addr == addr)
                    break;
                p1 = p2;
                p2 = p2->next;
            }
            if (p2 == NULL) // No such memory location in used memory list
            {
                printf("Error: Wrong Address!\n");
                fprintf(f, "Error: Wrong Address!\n");
                fclose(f);
                continue;
            }
            unsigned size = p2->m_size;
            p1->next = p2->next;
            free(p2);
            lfree(size, addr);
            printf("free success!\n");
            fprintf(f, "free success!\n");
            fclose(f);
        }
        else if (str[0] == '3') // display
        {
            FILE *f = fopen("./log.txt", "a");
            fprintf(f, "operation: display.\n");
            fclose(f);
            display();
        }
        else // exit
            break;
    }
    end();
    return 0;
}

// 初始化，向系统申请一片内存空间
void initialize()
{
    mp = malloc(MAX_MEMORY_SIZE);
    coremap = (struct map *)malloc(sizeof(struct map));
    coremap->m_addr = mp;
    coremap->m_size = MAX_MEMORY_SIZE;
    coremap->prior = coremap;
    coremap->next = coremap;
    usedmap = (struct used *)malloc(sizeof(struct used));
    usedmap->m_addr = 0;
    usedmap->m_size = 0;
    usedmap->next = NULL;
}

// 释放申请的地址
void end()
{
    struct used *p1 = usedmap, *p2 = usedmap->next;
    while (p2 != NULL)
    {
        p1 = p2;
        p2 = p2->next;
        lfree(p1->m_size, p1->m_addr);
        free(p1);
    }
    free(mp);
    free(coremap);
    free(usedmap);
    FILE *f = fopen("./log.txt", "a");
    fprintf(f, "close successfully.\n");
    fclose(f);
}

// 显示内存状况
void display()
{
    FILE *f = fopen("./log.txt", "a");
    struct map *tmp = coremap;
    printf("-------Free Memory-------\n");
    fprintf(f, "-------Free Memory-------\n");
    printf("pointer 1 address: %lu, pointer 1 size: %lu\n", tmp->m_addr, tmp->m_size);
    fprintf(f, "pointer 1 address: %lu, pointer 1 size: %lu\n", tmp->m_addr, tmp->m_size);
    tmp = tmp->next;
    int count = 2;
    while(tmp != coremap)
    {
        printf("pointer %d address: %lu, pointer %d size: %lu\n", count, tmp->m_addr, count, tmp->m_size);
        fprintf(f, "pointer %d address: %lu, pointer %d size: %lu\n", count, tmp->m_addr, count, tmp->m_size);
        tmp = tmp->next;
        ++count;
    }
    printf("-------------------------\n");
    fprintf(f, "-------------------------\n");
    struct used *tmp2 = usedmap->next;
    if (tmp2 != NULL)
    {
        printf("-------Used Memory-------\n");
        fprintf(f, "-------Used Memory-------\n");
        while (tmp2 != NULL)
        {
            printf("used memory location: %lu, size: %lu\n", tmp2->m_addr, tmp2->m_size);
            fprintf(f, "used memory location: %lu, size: %lu\n", tmp2->m_addr, tmp2->m_size);
            tmp2 = tmp2->next;
        }
        printf("-------------------------\n");
        fprintf(f, "-------------------------\n");
    }
    fclose(f);
}

// 内存分配
char *lmalloc(unsigned size)
{
    if(coremap->m_size < size) // 申请大小大于当前指针所指的空闲区大小
    {
        struct map *tmp = coremap->next;
        while(tmp != coremap)
        {
            if(tmp->m_size >= size)
                break;
            tmp = tmp->next;
        }
        if(tmp == coremap) // 若遍历循环链表都没有找到 m_size >= size 的空闲区
            return -1;
        coremap = tmp; // 当前指针即为 m_size >= size 的空闲区
    }
    if(coremap->m_size > size) // 申请大小小于当前指针指向空闲区大小
    {
        char *res = coremap->m_addr;
        coremap->m_size -= size;
        coremap->m_addr += size;
        return res;
    }
    else // 申请大小等于当前指针所指的空闲区大小
    {
        struct map *tmp = coremap;
        char *res = coremap->m_addr;
        coremap = coremap->next;
        if(tmp == coremap) // 若空闲区链表只有一个元素
        {
            coremap->m_addr += size;
            coremap->m_size = 0;
            return res;
        }
        tmp->prior->next = coremap;
        coremap->prior = tmp->prior;
        free(tmp);
        return res;
    }
}

// 地址释放
void lfree(unsigned size, char *addr)
{
    if(coremap->m_addr < addr) // 当前指针指向地址在请求释放的地址的前面
    {
        if(coremap->next == coremap) // 空闲区链表只有一个元素
        {
            if(coremap->m_addr + coremap->m_size == addr) // 要释放的内存区和上一个空闲区相连
            {
                coremap->m_size += size;
                return;
            }
            else // 要释放的内存区和上一个空闲区不相连
            {
                struct map *tmp = (struct map *)malloc(sizeof(struct map));
                tmp->m_addr = addr;
                tmp->m_size = size;
                if (coremap->m_size != 0)
                {
                    coremap->next = tmp;
                    coremap->prior = tmp;
                    tmp->prior = coremap;
                    tmp->next = coremap;
                }
                else
                {
                    coremap = tmp;
                    coremap->next = coremap;
                    coremap->prior = coremap;
                }
            }
            return;
        }
        else // 空闲区表有多个元素
        {
            struct map *p1 = coremap;
            struct map *p2 = coremap->next;
            while(p2->m_addr > p1->m_addr) // 当p2->m_addr指向的地址在p1->m_addr后面时
            {
                if(p1->m_addr < addr && p2->m_addr > addr) // addr 在 p->m_addr 和 p2->m_addr 之间
                {
                    if(p1->m_addr + p1->m_size + size == p2->m_addr) // 释放后三个区域相连
                    {
                        p1->m_size += size + p2->m_size;
                        p1->next = p2->next;
                        p2->next->prior = p1;
                        free(p2);
                        return;
                    }
                    else if(p1->m_addr + p1->m_size == addr) // 释放后p1与释放区相连
                    {
                        p1->m_size += size;
                        return;
                    }
                    else if(addr + size == p2->m_addr) // 释放后p2与释放区相连
                    {
                        p2->m_addr = addr;
                        p2->m_size += size;
                        return;
                    }
                    else // 释放区与二者都不相连
                    {
                        struct map *tmp = (struct map *)malloc(sizeof(struct map));
                        tmp->m_addr = addr;
                        tmp->m_size = size;
                        tmp->next = p2;
                        tmp->prior = p1;
                        p1->next = tmp;
                        p2->prior = tmp;
                        return;
                    }
                }
                p1 = p2;
                p2 = p2->next;
            }
            if(p1->m_addr + p1->m_size == addr) // p2回到链表头部，此时p1指向链表尾部，p1与释放区相连
            {
                p1->m_size += size;
                return;
            }
            else // p1与释放区不相连
            {
                struct map *tmp = (struct map *)malloc(sizeof(struct map));
                tmp->m_addr = addr;
                tmp->m_size = size;
                tmp->prior = p1;
                tmp->next = p2;
                p1->next = tmp;
                p2->prior = p1;
            }
        }
    }
    else // 当前指针指向地址在请求释放地址之后
    {
        if(coremap->next == coremap) // 空闲区链表只有一个元素
        {
            if(addr + size == coremap->m_addr) // 释放区与空闲区相连
            {
                coremap->m_addr -= size;
                coremap->m_size += size;
                return;
            }
            else // 释放区与空闲区不相连
            {
                struct map *tmp = (struct map *)malloc(sizeof(struct map));
                tmp->m_addr = addr;
                tmp->m_size = size;
                if (coremap->m_size != 0)
                {
                    tmp->next = coremap;
                    tmp->prior = coremap;
                    coremap->next = tmp;
                    coremap->prior = tmp;
                }
                else
                {
                    coremap = tmp;
                    coremap->next = coremap;
                    coremap->prior = coremap;
                }
            }
        }
        else // 空闲区链表有多个元素
        {
            struct map *p1 = coremap;
            struct map *p2 = coremap->prior;
            while(p2->m_addr < p1->m_addr) // p2->m_addr 在 p1->m_addr 之前
            {
                if(p2->m_addr < addr && p1->m_addr > addr) // addr 在 p2->m_addr 和 p1->m_addr 之间
                {
                    if(p2->m_addr + p2->m_size + size == p1->m_addr) // 释放后三个区域相连
                    {
                        p2->m_size += size + p1->m_size;
                        p2->next = p1->next;
                        p1->next->prior = p2;
                        if(p1 == coremap)
                            coremap = p2;
                        free(p1);
                        return;
                    }
                    else if(p2->m_addr + p2->m_size == addr) // 释放区与p2相连
                    {
                        p2->m_size += size;
                        return;
                    }
                    else if(addr + size == p1->m_addr) // 释放区与p1相连
                    {
                        p1->m_addr = addr;
                        p1->m_size += size;
                        return;
                    }
                    else // 三个区域不相连
                    {
                        struct map *tmp = (struct map *)malloc(sizeof(struct map));
                        tmp->m_addr = addr;
                        tmp->m_size = size;
                        tmp->prior = p2;
                        tmp->next = p1;
                        p2->next = tmp;
                        p1->prior = tmp;
                        return;
                    }
                }
                p1 = p2;
                p2 = p2->prior;
            }
            if(addr + size == p1->m_addr) // 释放区与p1相连，此时p2指向链表尾部
            {
                p1->m_addr = addr;
                p1->m_size += size;
                return;
            }
            else // 释放区与p1不相连，此时释放区为空闲区链表头
            {
                struct map *tmp = (struct map *)malloc(sizeof(struct map));
                tmp->m_addr = addr;
                tmp->m_size = size;
                tmp->next = p1;
                tmp->prior = p2;
                p1->prior = tmp;
                p2->next = tmp;
                return;
            }
        }
    }
}