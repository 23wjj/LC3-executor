//Lab 6b executor
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 struct Lc3 {
 	unsigned short int r[8],pc;//general used registers
 	char mem[0x10000][17];
    int neg,zero,pos;//flag registers
 };//simulate the memory of LC3 machine
struct Lc3 LC3;
char halt[17];//the const string of halt instruction
char temp[17];
int data[16];
void execute();
void addfunc(int inst[]);
void andfunc(int inst[]);
void brfunc(int inst[]);
void jmpfunc(int inst[]);
void jsrfunc(int inst[]);
void ldfunc(int inst[]);
void ldifunc(int inst[]);
void ldrfunc(int inst[]);
void leafunc(int inst[]);
void notfunc(int inst[]);
void stfunc(int inst[]);
void stifunc(int inst[]);
void strfunc(int inst[]);
void setcc(short num);
int sext(int num[],int cnt);
unsigned short int unsignsext(int *num,int cnt);
char* dec2char(int num);
int* char2dec(char *arr);
void numcpy(int *src,int *des,int num);
int pcchange;
int main(){
    int i=0,j=0;
    char array[17];
    int inst[16];
    unsigned short int addr;
    //initialize several parameters
    LC3.r[0]=LC3.r[1]=LC3.r[2]=LC3.r[3]=LC3.r[4]=LC3.r[5]=LC3.r[6]=LC3.r[7]=0x7777;
    pcchange=1;
    strcpy(halt,"1111000000100101");
    scanf("%s",array);
    getchar();
    for(i=0;i<16;i++)
        inst[i]=array[i]-48;
    LC3.pc=addr=unsignsext(inst,16);
    for(j=0;j<0x10000;j++)
        strcpy(LC3.mem[j],"0111011101110111");
    //store the instructions input
    while(scanf("%s",array)!=EOF){
    	getchar();
        strcpy(LC3.mem[addr++],array);
    }
    execute();
    //output registers
    for(i=0;i<8;i++){
        printf("R%d = x%04X",i,LC3.r[i]);
        if(i!=7)
            printf("\n");
    }
    return 0;
}
void execute(){
    while(1){
        if(!strcmp(halt,LC3.mem[LC3.pc]))
            break;
        else{
            int inst[16];
            int type,i; 
            for(i=0;i<16;i++)
                inst[i]=LC3.mem[LC3.pc][i]-48;
            type=inst[3]+2*inst[2]+4*inst[1]+8*inst[0];
            switch(type){
                case 1:addfunc(inst);break;//"0001"
                case 5:andfunc(inst);break;//"0101"
                case 0:brfunc(inst);break;// "0000"
                case 12:jmpfunc(inst);break;//"1100"
                case 4:jsrfunc(inst);break;//"0100"
                case 2:ldfunc(inst);break;//"0010"
                case 10:ldifunc(inst);break;//"1010"
                case 6:ldrfunc(inst);break;//"0110"
                case 14:leafunc(inst);break;//"1110"
                case 9:notfunc(inst);break;//"1001"
                case 3:stfunc(inst);break;//"0011"
                case 11:stifunc(inst);break;//"1011"
                case 7:strfunc(inst);break;//"0111"
                default:break;
            }
            if(pcchange)
            	LC3.pc++;
        }
    }
    return;
}
void addfunc(int inst[]){
    int dr,sr1,imm5,sr2;
    dr=inst[6]+2*inst[5]+4*inst[4];
    sr1=inst[9]+2*inst[8]+4*inst[7];
    if(inst[10]){
        imm5=sext(inst,5);
        LC3.r[dr]=LC3.r[sr1]+imm5;
    }
    else{
        sr2=inst[15]+2*inst[14]+4*inst[13];
        LC3.r[dr]=LC3.r[sr1]+LC3.r[sr2];   
    }
    setcc((short)LC3.r[dr]);
    pcchange=1;
    return;    
}
void andfunc(int inst[]){
    int dr,sr1,imm5,sr2;
    dr=inst[6]+2*inst[5]+4*inst[4];
    sr1=inst[9]+2*inst[8]+4*inst[7];
    if(inst[10]){
        imm5=sext(inst,5);
        LC3.r[dr]=LC3.r[sr1]&imm5;
    }
    else{
        sr2=inst[15]+2*inst[14]+4*inst[13];
        LC3.r[dr]=LC3.r[sr1]&LC3.r[sr2];   
    }
    setcc((short)LC3.r[dr]);
    pcchange=1;
    return;
}
void brfunc(int inst[]){
	if((inst[4]&LC3.neg)|(inst[5]&LC3.zero)|(inst[6]&LC3.pos)){
		LC3.pc=LC3.pc+sext(inst,9)+1;
        pcchange=0;
	}
	else
		pcchange=1;
    return;
}
void jmpfunc(int inst[]){
	LC3.pc=LC3.r[inst[9]+2*inst[8]+4*inst[7]];
    pcchange=0;
    return;
}
void jsrfunc(int inst[]){
    //if it is a jsr instruction
    LC3.r[7]=LC3.pc+1;
    if(inst[4])
        LC3.pc=LC3.pc+1+sext(inst,11);
    //else if it is a jsrr function
    else
        LC3.pc=LC3.r[inst[9]+2*inst[8]+4*inst[7]];
    pcchange=0;
    return;
}
void ldfunc(int inst[]){
    unsigned short int num;
    num=LC3.r[inst[6]+2*inst[5]+4*inst[4]]=sext(char2dec(LC3.mem[LC3.pc+1+sext(inst,9)]),16);
    setcc((short)num);
    pcchange=1;
    return;
}
void ldifunc(int inst[]){
    unsigned short num;
    num=LC3.r[inst[6]+2*inst[5]+4*inst[4]]=sext(char2dec(LC3.mem[unsignsext(char2dec(LC3.mem[LC3.pc+1+sext(inst,9)]),16)]),16);
    setcc((short)num);
    pcchange=1;
    return;
}
void ldrfunc(int inst[]){
    int basereg=inst[9]+2*inst[8]+4*inst[7];
    unsigned short num;
    num=LC3.r[inst[6]+2*inst[5]+4*inst[4]]=sext(char2dec(LC3.mem[LC3.r[basereg]+sext(inst,6)]),16);
    setcc((short)num);
    pcchange=1;
    return;
}
void leafunc(int inst[]){
    LC3.r[inst[6]+2*inst[5]+4*inst[4]]=LC3.pc+1+sext(inst,9);
    pcchange=1;
    return;
}
void notfunc(int inst[]){
    setcc((short)(LC3.r[inst[6]+2*inst[5]+4*inst[4]]=~(LC3.r[inst[9]+2*inst[8]+4*inst[7]])));
    pcchange=1;
    return;
}
void stfunc(int inst[]){
    strcpy(LC3.mem[LC3.pc+1+sext(inst,9)],dec2char(LC3.r[inst[6]+2*inst[5]+4*inst[4]]));
    pcchange=1;
    return ;
}
void stifunc(int inst[]){
    char tmp[17];
    strcpy(tmp,LC3.mem[LC3.pc+1+sext(inst,9)]);
    strcpy(LC3.mem[unsignsext(char2dec(tmp),16)],dec2char(LC3.r[inst[6]+2*inst[5]+4*inst[4]]));
    pcchange=1;
    return;
}
void strfunc(int inst[]){
    strcpy(LC3.mem[LC3.r[inst[9]+2*inst[8]+4*inst[7]]+sext(inst,6)],dec2char(LC3.r[inst[6]+2*inst[5]+4*inst[4]]));
    pcchange=1;
    return;
}
void setcc(short num){
    if(num>0){
        LC3.neg=LC3.zero=0;
        LC3.pos=1;
    }
    else if(num<0){
        LC3.pos=LC3.zero=0;
        LC3.neg=1;
    }
    else{
        LC3.neg=LC3.pos=0;
        LC3.zero=1;
    }
    return;
}
int sext(int num[],int cnt){
    int i=0,mul=1,result=0;
    for(i=0;i<cnt;i++){
        if(i<cnt-1)
            result+=mul*num[15-i];
        else
            result-=mul*num[15-i];
        mul*=2;
    }
    return result;
}
char* dec2char(int num){
    int i=0;
    for(i=0;i<16;i++){
        temp[15-i]=num%2+48;
        num=num/2;
    }
    temp[16]='\0';
    return temp;
}
int* char2dec(char* arr){
    int i;
    for(i=0;i<16;i++)
        data[i]=arr[i]-48;
    return data;
}
void numcpy(int *src,int *des,int num){
    int i;
    for(i=0;i<num;i++)
        des[i]=src[i];
    return;
}
unsigned short int unsignsext(int *num,int cnt){
	int i=0,mul=1;
	unsigned short int result=0;
    for(i=0;i<cnt;i++){
        result+=mul*num[15-i];
        mul*=2;
    }
    return result;
}
