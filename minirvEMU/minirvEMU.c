#include<stdio.h>
#include<stdint.h>
#define MEM_SIZE 1024
uint32_t pc=0;
uint32_t  x[32];
uint8_t M[MEM_SIZE];
uint8_t r(uint32_t addr){
	return M[addr];
}
void w(uint32_t addr,uint8_t val){
	M[addr]=val;}
uint32_t r4(uint32_t addr){
uint8_t h,j,k,l;
h=M[addr];
j=M[addr+1];
k=M[addr+2];
l=M[addr+3];
return (uint32_t)h|(uint32_t)j << 8|(uint32_t)k << 16|(uint32_t)l << 24;
void w4(uint32_t addr,uint32_t val)
{
 M[addr]     = val & 0xFF;
    M[addr + 1] = (val >> 8) & 0xFF;
    M[addr + 2] = (val >> 16) & 0xFF;
    M[addr + 3] = (val >> 24) & 0xFF;}





