#include "common.h"
#include "burst.h"
#include "stdlib.h"
#include "memory/cache.h"
uint32_t dram_read(hwaddr_t addr, size_t len);
void call_ddr3_read(hwaddr_t, void *);
void call_ddr3_write(hwaddr_t, void *, uint8_t *);
void init_cache(){
//初始化cache设置初始值
    int i;
    for (i = 0; i < CACHE_SIZE / CACHE_BLOCK; i++){
        cache[i].valid = false;
        cache[i].tag = 0;
        memset(cache[i].byte, 0, CACHE_BLOCK);
        //将block中的值更新
    }
    cnt = 0;
}

//模拟cache访问
uint32_t cache_read(hwaddr_t addr) {
    uint32_t set = (addr >> 6) & 0x7f; //取出地址中set的部分（右移block位并 & 1111111）
    bool hit = false;//定义是否命中
    int i;
    for (i = set * E; i < (set + 1) * E; i++) { //找到相应set所在
        if (cache[i].tag == (addr >> 13) && cache[i].valid) { 
            //valid+tag 命中
            hit = true; 
            cnt += 2;
            break;
        }
    }
    //如果无法命中
    //分两种情况：有空白区域，找到空白区域，从内存中读取
    //没有空白，执行随机替换算法，再读取数据
    if (!hit) { 
        for (i = set * E; i < (set + 1) * E; i++) { 
            if (!cache[i].valid) 
            break; 
        }
        if (i == (set + 1) * E) { 
            srand(0);
            //随机替换
            i = set * E + rand() % E;
        }
        cache[i].valid = true;
        cache[i].tag = addr >> 13;
       int j;
       uint32_t block = (addr >> 6) << 6; // 取出block
         for (j = 0; j < BURST_LEN; j++) {
            call_ddr3_read(block + j * BURST_LEN, cache[i].byte + j * BURST_LEN); 
         }
        cnt += 200;
    }
    return i;
}

void cache_write(hwaddr_t addr, size_t len, uint32_t data) {
    uint32_t set = (addr >> 6) & 0x7f;
    uint32_t offset = addr & (CACHE_BLOCK - 1); 
    int i;
    bool hit = false;
    for (i = set * E; i < (set + 1) * E; i++) {
        if (cache[i].tag == (addr >> 13) && cache[i].valid) {
            hit = true;
            break;
        }
    }
    if (hit) { 
        memcpy(cache[i].byte + offset, &data, len);
    }     
    else {
        i = cache_read(addr);
        //先从内存中读出同步更新cache
         memcpy(cache[i].byte + offset, &data, len);
    }
}