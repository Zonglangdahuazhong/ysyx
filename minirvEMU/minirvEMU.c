#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

// 1. 【新增】引入 AM 核心头文件
#include <am.h>
#include <klib-macros.h>

#define MEM_SIZE 16777216

uint32_t pc;
uint32_t x[32];
uint8_t M[MEM_SIZE];

// 2. 【新增】定义标准屏幕大小
#define SCREEN_W 400
#define SCREEN_H 300

// 虚拟显存，初始化为全黑
uint32_t vram[SCREEN_W * SCREEN_H] = {0}; 

// 3. 【新增】负责将 vram 数组完整同步到屏幕的渲染函数
void flush_vram_to_screen() {
  for (int y = 0; y < SCREEN_H; y++) {
    // 传入 vram 对应行的起始指针，最后一行时 sync 设为 true 触发显示
    io_write(AM_GPU_FBDRAW, 0, y, &vram[y * SCREEN_W], SCREEN_W, 1, (y == SCREEN_H - 1));
  }
}

uint8_t r(uint32_t addr) {
	return M[addr];
}

void w(uint32_t addr, uint8_t val) { 
	M[addr] = val; 
}

uint32_t r4(uint32_t addr) {
	uint8_t h, j, k, l;
	h = M[addr];
	j = M[addr+1];
	k = M[addr+2];
	l = M[addr+3];
	return (uint32_t)h | (uint32_t)j << 8 | (uint32_t)k << 16 | (uint32_t)l << 24;
}

void w4(uint32_t addr, uint32_t val) {
	M[addr]     = val & 0xFF;
	M[addr + 1] = (val >> 8) & 0xFF;
	M[addr + 2] = (val >> 16) & 0xFF;
	M[addr + 3] = (val >> 24) & 0xFF;
}      

void load_bin(const char *filename) {
	FILE *f = fopen(filename, "rb");
	if (!f) {
		perror("open bin failed");
		return;
	}
	fread(M, 1, MEM_SIZE, f);
	fclose(f);
}

int main() {

	
	// 4. 【新增】在模拟器一启动时，初始化 AM 图形环境
	ioe_init(); 

	load_bin("vga.bin"); // 如果你要跑 vga 程序，记得把这里改成你的 vga.bin 路径

/*	M[0x228] = 0x73;
	M[0x228+1] = 0x00;
	M[0x228+2] = 0x10;
	M[0x228+3] = 0x00;  */
	pc = 0;

	while(1) {
		x[0] = 0;

		uint32_t next_pc = pc + 4;
		uint32_t inst = r4(pc);
		int op = inst & 0x7f;
		uint32_t rd = (inst >> 7) & 0x1f;
		uint32_t fun3 = (inst >> 12) & 0x7;
		uint32_t rs1 = (inst >> 15) & 0x1f;
		uint32_t rs2 = (inst >> 20) & 0x1f;
		uint32_t funct7 = (inst >> 25) & 0x7f;
		int32_t immi = (int32_t)inst >> 20;
		int32_t imms = ((inst >> 25) << 5) | ((inst >> 7) & 0x1f);
		
		if (inst & 0x80000000) { 
			imms |= 0xFFFFF000;  
		}
		uint32_t immu = inst & 0xfffff000;

		switch(op) {
			/* ADD */	
			case 0x33: {
				if (fun3 == 0x0 && funct7 == 0x00) {
					x[rd] = x[rs1] + x[rs2];
				}
				break;
			}
			/* ADDI */
			case 0x13: { 
				if (fun3 == 0x0) {
					x[rd] = x[rs1] + immi;
				}
				break;
			}
			/* lui */
			case 0x37: { 
				x[rd] = immu;
				break;
			}
			/* lw lbu */
			case 0x03: { 
				if (fun3 == 0x2) {
					uint32_t addr = x[rs1] + immi;
					x[rd] = r4(addr);
				} else if (fun3 == 0x4) {
					uint32_t addr = x[rs1] + immi;
					uint8_t m = r(addr);
					x[rd] = (uint32_t)m;
				}
				break;
			}
			/* sw sb */
			case 0x23: { 
				if (fun3 == 0x2) {
					uint32_t addr = x[rs1] + imms;
					w4(addr, x[rs2]);

					if (addr >= 0x20000000 && addr < 0x20040000) {
						uint32_t offset = (addr - 0x20000000) / 4;
						// 严格的安全检查：只有在 0 到 120000 范围内的合法偏移才允许写入
						if (offset >= 0 && offset < SCREEN_W * SCREEN_H) {
							vram[offset] = x[rs2];
						}				
						}
else {
        // 原本的普通内存写入逻辑
        w4(addr,x[rs2]);}
						else if (fun3 == 0x0) {
					uint32_t addr = x[rs1] + imms;
					uint8_t va1l = x[rs2] & 0xff;
					w(addr, va1l);
				}
				break;
			}} 
			/* jalr */ 
			case 0x67: {
				if (fun3 == 0x0) {
					uint32_t target = x[rs1] + immi;
					x[rd] = pc + 4;   
					next_pc = target & ~1;   
				}
				break;
			}  
			/* ebreak */
			case 0x73: {    
				if (inst == 0x00100073) {  
					printf("ebreak hit, stop program\n");
					printf("Result in x[10] (a0) = %u\n", x[10]);

					// 6. 【修正】当程序遇到 ebreak 停机时，开始向屏幕刷新画面并锁死窗口
				//	flush_vram_to_screen();
					while (1) {
						io_read(AM_INPUT_KEYBRD); // 轮询键盘防止卡死，按 ESC 可配合底层退出
					}
				}
				break;
			}
		}  
		
		pc = next_pc;

	}

}return 0;
}
