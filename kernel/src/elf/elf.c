#include "common.h"
#include "memory.h"
#include <string.h>
#include <elf.h>

#define ELF_OFFSET_IN_DISK 0

#ifdef HAS_DEVICE
void ide_read(uint8_t *, uint32_t, uint32_t);
#else
void ramdisk_read(uint8_t *, uint32_t, uint32_t);
#endif

#define STACK_SIZE (1 << 20)

void create_video_mapping();
uint32_t get_ucr3();

uint32_t loader() {
	Elf32_Ehdr *elf;
	Elf32_Phdr *ph = NULL;

	uint8_t buf[4096];

#ifdef HAS_DEVICE
	ide_read(buf, ELF_OFFSET_IN_DISK, 4096);
#else
	ramdisk_read(buf, ELF_OFFSET_IN_DISK, 4096);
#endif

	elf = (void*)buf;

	/* TODO: fix the magic number with the correct one */
    //修改为elf文件的魔数
	const uint32_t elf_magic = 0x464c457f;
	uint32_t *p_magic = (void *)buf;
	nemu_assert(*p_magic == elf_magic);
 
	/* Load each program segment */
    //初始化ph指向program header开头，buf指向elf文件的开头，e_phoff为program header偏移量
    ph = (void *)buf + elf->e_phoff;
    //eph指向program header的末尾，e_phnum为program header中segment的数量
    //遍历program header表，加载需要加载的segment
	Elf32_Phdr *eph = 0;
	for(eph = ph + elf->e_phnum;ph < eph;ph++) {
		/* Scan the program header table, load each segment into memory */
		if(ph->p_type == PT_LOAD) {
            uint32_t addr = ph->p_vaddr; //存储segment加载到的目标地址
			/* TODO: read the content of the segment from the ELF file 
			 * to the memory region [VirtAddr, VirtAddr + FileSiz)
			 */
            //利用函数从当前segment中读取filesiz大小的数据到目标地址
         ramdisk_read((void *)addr, ELF_OFFSET_IN_DISK + ph->p_offset,ph->p_filesz);
			
			/* TODO: zero the memory region 
			 * [VirtAddr + FileSiz, VirtAddr + MemSiz)
			 */
         //通过函数将未初始化的数据置0
         memset((void *)addr + ph->p_filesz,0,ph->p_memsz - ph->p_filesz);

#ifdef IA32_PAGE
			/* Record the program break for future use. */
			extern uint32_t cur_brk, max_brk;
			uint32_t new_brk = ph->p_vaddr + ph->p_memsz - 1;
			if(cur_brk < new_brk) { max_brk = cur_brk = new_brk; }
#endif
		}
	}

	volatile uint32_t entry = elf->e_entry;

#ifdef IA32_PAGE
	mm_malloc(KOFFSET - STACK_SIZE, STACK_SIZE);

#ifdef HAS_DEVICE
	create_video_mapping();
#endif

	write_cr3(get_ucr3());
#endif

	return entry;
}
