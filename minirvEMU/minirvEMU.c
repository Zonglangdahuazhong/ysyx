#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <am.h>
#include <stdbool.h>

#define MEM_SIZE 16777216

uint32_t pc;
uint32_t x[32];
uint8_t M[MEM_SIZE];
#define VGA_START 0x20000000
#define VGA_END   0x20040000


#define SCREEN_W 256
#define SCREEN_H 256
uint32_t vram[SCREEN_W * SCREEN_H]; 




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
load_bin("vga.bin"); 

/*	M[0x228] = 0x73;
	M[0x228+1] = 0x00;
	M[0x228+2] = 0x10;
	M[0x228+3] = 0x00;  */
	pc = 0;

	while(1) {
		x[0] = 0;
		if (pc >= MEM_SIZE - 4) {
            printf("[minirvEMU] PC out of bounds, entering host dead loop to keep display.\n");
            while(1) { /* 留驻画面 */ }
        }

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
			 if (fun3 == 0x2) { // sw 指令
                    uint32_t addr = x[rs1] + imms;
                    
                    // ================= 2. 关键：拦截显存写入并实时刷新 =================
                    if (addr >= VGA_START && addr < VGA_END) {
                        uint32_t offset = (addr - VGA_START) / 4;
                        if (offset < SCREEN_W * SCREEN_H) {
                            vram[offset] = x[rs2]; // 写入自定义显存
                            
                            // 计算当前像素的 X, Y 坐标
                            int pixel_x = offset % SCREEN_W;
                            int pixel_y = offset / SCREEN_W;
                            
                            // 实时同步这 1 个像素到 AM 屏幕上
                            AM_GPU_FBDRAW_T draw = {
                                .x = pixel_x,   .y = pixel_y, 
                                .pixels = &vram[offset], 
                                .w = 1,         .h = 1, 
                                .sync = true
                            };
                            ioe_write(AM_GPU_FBDRAW, &draw);
                        }
                    } else {
                        // 普通物理内存写入
                        w4(addr, x[rs2]);
                    }
                }
						else if (fun3 == 0x0) {
					uint32_t addr = x[rs1] + imms;
					uint8_t va1l = x[rs2] & 0xff;
					w(addr, va1l);
				}
				break;
			}
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
			/*case 0x73: {    
				if (inst == 0x00100073) {  
					printf("ebreak hit, stop program\n");
					printf("Result in x[10] (a0) = %u\n", x[10]);
}} */
				break;
			}pc = next_pc;
		}  
		
	while(1) { }
    return 0;

	}


