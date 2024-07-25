// 韦东良，522031910516

#include "cachelab.h"
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

typedef struct line
{
    int valid;
    int tag;
    int timeStamp;
} Line;

typedef struct cache
{
    int S;
    int E;
    int B;
    Line **lines;
} Cache;

Cache* cache;
int hits = 0;
int misses = 0;
int evictions = 0;
int TIMESTAMP = 0;

void update(int tag, int set);
int findLRU(int set);

int main(int argc, char *argv[])
{

    int s, e, b;
    char opt;
    char *file;

    // 读参数
    while ((opt = getopt(argc, argv, "hvs:E:b:t:")) != -1){
        switch (opt)
        {
            case 's':
                s = atoi(optarg);
                break;
            case 'E':
                e = atoi(optarg);
                break;
            case 'b':
                b = atoi(optarg);
                break;
            case 't':
                file = optarg;
                break;
        }
    }

    // cache初始化
    cache = (Cache*)malloc(sizeof(Cache));
    cache->S = 1 << s;
    cache->E = e;
    cache->B = 1 << b;
    cache->lines = (Line**)malloc(sizeof(Line*) * cache->S);

    for (int i = 0; i < cache->S; i++)
    {
        cache->lines[i] = (Line*)malloc(sizeof(Line) * cache->E);
        for (int j = 0; j < cache->E; j++)
        {
            cache->lines[i][j].valid = 0;
            cache->lines[i][j].tag = -1;
            cache->lines[i][j].timeStamp = 0;
        }
    }

    FILE* fp = fopen(file, "r");
    char op;
    unsigned long addr;
    int size;

    // 读文件
    while (fscanf(fp, " %c %lx,%d", &op, &addr, &size) > 0)
    {
        if (op == 'I')
            continue;

        int tag = addr >> (s + b);
        int set = (addr >> b) & ((1 << s) - 1);
        
        update(tag, set);

        // M有两次
        if (op == 'M')
        {
            update(tag, set);
        }
       
    }

    fclose(fp);
    free(cache);

    printSummary(hits, misses, evictions);
    return 0;
}

// 模拟访问cache
void update(int tag, int set){

    int index = -1;
    int isFull = 1;

    for (int i = 0; i < cache->E; i++)
    {
        if (cache->lines[set][i].valid && cache->lines[set][i].tag == tag)
        {
            index = i;
            break;
        }
    }

    if (index == -1)
    {
        misses++;

        // 判断是否已满
        for (int i = 0; i < cache->E; i++)
        {
            if (cache->lines[set][i].valid == 0)
            {
                isFull = 0;
                index = i;
                break;
            }
        }

        if (isFull)
        {
            evictions++;
            index = findLRU(set);
        }

    } else {
        hits++;
    }

    // 越后访问，TIMESTAMP越大
    TIMESTAMP++;
    cache->lines[set][index].valid = 1;
    cache->lines[set][index].tag = tag;
    cache->lines[set][index].timeStamp = TIMESTAMP;
    
}

// 寻找LRU,TIMESTAMP最小的
int findLRU(int set){
    int min = 0;
    for (int i = 0; i < cache->E; i++)
    {
        if (cache->lines[set][i].timeStamp < cache->lines[set][min].timeStamp)
        {
            min = i;
        }
    }
    return min;
}