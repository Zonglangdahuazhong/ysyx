#include<stdint.h>
 #include<stdio.h>
  uint8_t pc=0;
  uint8_t R[4];
 uint8_t M[16]={139,145,160,177,41,23,209,126};
 void inst_cycle(){
   uint8_t inst=M[pc];
   pc=pc+1;
   uint8_t op=inst>>6;
   uint8_t rd=(inst>>4)&3;
   uint8_t rs1=(inst>>2)&3;
   uint8_t rs2=inst&3;
   uint8_t imm=inst&15;
   uint8_t addr=(inst>>2)&15;
   if(op==0){R[rd]=R[rs1]+R[rs2];}
 else if(op==2){ R[rd]=imm;}
   else if(op==3){if(R[rs2]!=R[0]) pc=addr; }}
   int main(){
 while(R[2]!=55){inst_cycle();
printf("%u %u %u %u\n",R[0],R[1],R[2],R[3]);

 } return 0;               }                 
