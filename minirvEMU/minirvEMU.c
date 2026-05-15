#include<stdio.h>
#include<stdint.h>
#define MEM_SIZE 16777216
uint32_t pc=0;
uint32_t  x[32];
uint8_t M[MEM_SIZE];
uint8_t r(uint32_t addr){
	return M[addr];
}
void w(uint32_t addr,uint8_t val){M[addr]=val;}

uint32_t r4(uint32_t addr){
uint8_t h,j,k,l;
h=M[addr];
j=M[addr+1];
k=M[addr+2];
l=M[addr+3];
return (uint32_t)h|(uint32_t)j << 8|(uint32_t)k << 16|(uint32_t)l << 24;}
void w4(uint32_t addr,uint32_t val)
{
 M[addr]     = val & 0xFF;
    M[addr + 1] = (val >> 8) & 0xFF;
    M[addr + 2] = (val >> 16) & 0xFF;
    M[addr + 3] = (val >> 24) & 0xFF;}                                                                                                                                           
int main(){
  
while(1){uint32_t inst=M[pc];
  int op=inst&0x7f;
  uint32_t rd=(inst>>7)&0x1f;
  uint32_t fun3=(inst>>12)&0x7;
  uint32_t rs1=(inst>>15)&0x1f;
  uint32_t rs2=(inst>>20)&0x1f;
 uint32_t funct7=(inst>>25)&0x7f;
  int32_t immi= (int32_t)inst >> 20;
	int32_t imms=((inst>>25)<<5)|((inst>>7)&0x1f);
if (imms & 0x800) imms |= 0xfffff000;    
uint32_t immu=inst & 0xfffff000;

	pc=pc+4;
	switch(op){
	/*   ADD  */	
case 0x33:
    {if (fun3 == 0x0 && funct7 == 0x00) {
        x[rd]=x[rs1] + x[rs2];
    }
    break;}
/*  ADDI */
case 0x13:
   { if (fun3 == 0x0) {
        x[rd]=x[rs1] + immi;
    }
    break;}


/* lui */
case 0x37:
   { x[rd]=immu;
    break;}
/* lw  lbu*/
		case 0x03:
   { if (fun3 == 0x2) {
        uint32_t addr=x[rs1] + immi;
        x[rd] = r4(addr);
    }else if (fun3 == 0x4){
             uint32_t addr=x[rs1] + immi;
            uint8_t x=r(addr);
						x[rd]=(uint32_t)x;}
    break;}


/*  sw  sb*/
		case 0x23:
   { if (fun3 == 0x2) {
        uint32_t addr=x[rs1] + imms;
        w4(addr, x[rs2]);
    }else if (fun3 == 0x0){
            uint32_t addr=x[rs1] + imms;
						uint8_t va1l=x[rs2]&0xff;
						w(addr,va1l)}
    break;
	 } 
	 /* jalr*/ 
	 case 0x67:
	 {
if(fun3==0x0)
{
 uint32_t target = x[rs1] + immi;

    x[rd] = pc + 4;   
    pc = target & ~1;   

  
}break;}














