#include <bits/stdc++.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
using namespace std;

//Register file
static int reg[32];

//memory
static unsigned char memory[4000];

//CLOCK CYCLES & TOTAL INSTRUCTIONS
static int clock_cycles = 0;
int inst_index=0;

//Stack pointer
static unsigned int sp = 4000;

//Instruction register & program counter & interstage registers;
static int IR,PC,Ra,Rb,imm,Rd,Rz, temp_PC, Ry, Rm, Rm_temp,flag=0,stall=0,stall1=0,knob2=1,b_terminate=0,branch_taken_flag,exit_IR;
vector<int>Control_line;
vector<int> IRq;
vector<int> Rd_buffer;
vector<int> Rd_index;

void print_Control_line()
{
	for(int i=0;i<Control_line.size();i++)
	{
		//cout<< "Control_line[" << i << "]: " << Control_line[i] << endl;
	}
}


void print_IRq()
{
	for(int i=0;i<IRq.size();i++)
	{
		//printf("IRq[%d]: %x\n", i ,IRq[i]);
	}
}

void print_Rd()
{
	//cout << "Rd_buffer " ;
	for(int i=0;i<Rd_buffer.size();i++)
	{
		//cout << Rd_buffer[i] << " " ;
	}
}
// it is used to set the reset values reset all registers and memory content to 0
void reset()

{
	for(int i=0;i<32;i++)
		reg[i]=0x00000000;
	reg[2] = sp;		//Stack pointer
	for(int i=0;i<4000;i++)
		memory[i]=0x00;
}

void write_word(unsigned char *mem, unsigned int address, unsigned int data)
{
  	mem+=address;
  	int *p = (int*)mem;
  	*p = data;
}

int read_word(unsigned char *mem, unsigned int address)
{
	mem+=address;
	int *p = (int*)mem;
	return *p;
}

//load_program_memory reads the input memory, and pupulates the instruction memory
void load_program_memory()
{
	FILE *fp;
  	unsigned int address,word;
  	fp = fopen("machine_code.mc", "r");
  	if(fp == NULL)
  	{
    	printf("Error opening input mem file\n");
    	exit(1);
  	}
  	while(fscanf(fp, "%x %x", &address, &word) != EOF)
  	{
  		if(word== 0xffffffff)
  			continue;
  		write_word(memory, address, word);	
  	} 	
  	fclose(fp);
  	for(int i=0;i<32;i++)
  		reg[i] = 0x0;
  	reg[2]=sp;			// initialising registers with zero
}

//writes the data memory in "data_out.mem" file
void write_data_memory() 
{
  	FILE * fp;
  	unsigned int i;
  	fp = fopen("data_out.mem", "w+");
  	if(fp == NULL) 
  	{
    	printf("Error opening dataout.mem file for writing\n");
    	return;
  	}
  	for(int i=0;i<32;i++)
		fprintf(fp,"reg%d:%d\n",i,reg[i]);
  	for(i=0; i < 4000; i = i+4)
  		fprintf(fp, "%x %x\n", i, read_word(memory, i));	
  	fclose(fp);
}

void fetch()
{
	//cout<<"INSIDE FETCH"<<endl;
	//cout<<"fetch------pc:"<<PC<<endl;
	unsigned char *mem = memory;
	int *p = (int*)(mem+PC);
	IR = 0x00000000;
	IR = *p;
	PC+=4;
	if(IR==0xEF000011)
	{
		//cout<<"fall_through detected\n";
		b_terminate=1;
		flag=1;
		exit_IR=0xEF000011;
		return;
	}
	b_terminate=0;		
	if(IRq.size()==5)
		IRq.erase(IRq.begin());
	
	IRq.push_back(IR);
	//printf("IR:%x\n", IR);

	if(Control_line.size()==5)
		Control_line.erase(Control_line.begin());
	Control_line.push_back(0);
}

int dependency(int r1,int r2,int index,int inst_type)
{
	int flag1=0,flag2=0;
	int k = Rd_buffer.size();
	if( (inst_type>=49 && inst_type<=57) )		// this denotes inst. which do not have Rd
	{
		if(k >=1)
		{
			if((index-Rd_index[k-1]) != 1)
			{
				return 0;
			}
			if(r1==Rd_buffer[k-1])
			{
				flag1=1;
				//cout << "____________________________" << endl;
				return 1;
			}
			else if(r2==Rd_buffer[k-1])
			{
				flag2=1;
				//cout << "____________________________" << endl;
				return 2;
			}
		}
		else
		{
			return 0;
		}
	}
	else if (inst_type>=1 && inst_type<=27)
	{
		if(k >=2)
		{	
			if((index-Rd_index[k-2]) != 1)
			{
				return 0;
			}
			if(r1==Rd_buffer[k-2])
			{
				flag1=1;
				//cout << "____________________________" << endl;
				return 1;
			}
			else if(r2==Rd_buffer[k-2])
			{
				flag2=1;
				//cout << "____________________________" << endl;
				return 2;
			}
		}
		else
		{
			return 0;
		}	
	}	
}

int dependency(int r1,int index,int inst_type)
{
	int flag1=0;
	int k = Rd_buffer.size();
	//cout << "k:" << k << endl;
	if(inst_type>=28 && inst_type<=48)
	{
		if(k >=2)
		{
			if((index-Rd_index[k-2]) != 1)  // checking for consecutive instructions , index = inst_index
			{
				return 0;
			}
			if(r1==Rd_buffer[k-2])
			{
				flag1=1;
				//cout << "____________________________" << endl;
				return 1;
			}
		}
		else
		{
			return 0;
		}
	}
	else 		// this denotes inst. which do not have Rd
	{
		if(k >=1)      
		{
			if((index-Rd_index[k-1]) != 1)
			{
				return 0;
			}
			if(r1==Rd_buffer[k-1])
			{
				flag1=1;
				//cout << "____________________________" << endl;
				return 1;
			}
		}
		else
		{
			return 0;
		}
	}
}

int branch_dependency(int r1,int r2,int index,int inst_type)
{
	int flag1=0,flag2=0;
	int k = Rd_buffer.size();
	if(k==0)
		return 0;
	if( (inst_type>=52 && inst_type<=57) )		// this denotes inst. which do not have Rd
	{
		if((index-Rd_index[k-1])>2)
		{
			return 0;
		}
		else
		{
			if((index-Rd_index[k-1])==1)
			{
				if(k==1)
				{
					if(r1==Rd_buffer[k-1] && r1==r2)
						return 1;
					else if(r1==Rd_buffer[k-1] && r1!=r2)
						return 2;
					else if(r2==Rd_buffer[k-1] && r1!=r2)
						return 3;
				}
				else
				{
					if((index-Rd_index[k-2])!=2)
					{
						if(r1==Rd_buffer[k-1] && r1==r2)
							return 1;
						else if(r1==Rd_buffer[k-1] && r1!=r2)
							return 2;
						else if(r2==Rd_buffer[k-1] && r1!=r2)
							return 3;
					}
					else
					{
						if(r1==r2)
						{
							if(r1==Rd_buffer[k-1])
								return 1;
							else if(r1==Rd_buffer[k-2])
								return 4;	
						}
						else
						{
							if(r1==Rd_buffer[k-1] && r2==Rd_buffer[k-2])
								return 5;
							else if(r1==Rd_buffer[k-1] && r2!=Rd_buffer[k-2])
							{
								return 2;
							}
							else if(r1!=Rd_buffer[k-1] && r2==Rd_buffer[k-2])
								return 6;

							else if(r2==Rd_buffer[k-1] && r1==Rd_buffer[k-2])
								return 7;
							else if(r2==Rd_buffer[k-1] && r1!=Rd_buffer[k-2])
								return 3;
							else if(r2!=Rd_buffer[k-1] && r1==Rd_buffer[k-2])
								return 8;
							else if(r1!=Rd_buffer[k-1] && r2!=Rd_buffer[k-2]) //
								return 0;
							else if(r2!=Rd_buffer[k-1] && r1!=Rd_buffer[k-2]) //
								return 0;
						}
					}
				}
			}
			else if((index-Rd_index[k-1])==2)
			{
				if(r1==Rd_buffer[k-1] && r1==r2)
					return 9;
				else if(r1==Rd_buffer[k-1] && r1!=r2)
					return 10;
				else if(r2==Rd_buffer[k-1] && r1!=r2)
					return 11;
			}
		}
	}
	return 0;
}


/************ DECODE **************************/
unsigned int opcode(unsigned int word)
{
	unsigned int temp = 0x0000007F;
	temp = temp & word;
	return temp;
}

unsigned int rd(unsigned int word)
{
	unsigned int temp = 0x00000F80;
	temp = temp &  word;
	temp = temp/128;
	return temp;
}

unsigned int func3(unsigned int word)
{
	unsigned int temp = 0x00007000;
	temp = temp & word;
	temp = temp/4096;
	return temp;
}

unsigned int rs1(unsigned int word)
{
	unsigned int temp = 0x000F8000;
	temp = temp & word;
	temp = temp/32768;
	return temp;
}

unsigned int rs2(unsigned int word)
{
	unsigned int temp = 0x01F00000;
	temp = temp & word;
	temp = temp/1048576;
	return temp;
}

unsigned int func7(unsigned int word)
{
	unsigned int temp = 0xFE000000;
	temp = temp & word;
	temp = temp/33554432;
	return temp;
}

int imm_11_0 (unsigned int word)
{
	int temp = 0xFFF00000;
	temp = temp & word;
	temp = temp/1048576;
	return temp;
}

int imm_4_0 (unsigned int word)
{
	return rd(word);
}

int imm_11_5 (unsigned int word)
{
	return func7(word);
}

int imm_4_1_11 (unsigned int word)
{
	return rd(word);
}

int imm_12_10_5 (unsigned int word)
{
	return func7(word);
}

int imm_31_12 (unsigned int word)
{
	int temp = 0xFFFFF000;
	temp = temp&word;
	temp = temp/4096;
	return temp;
}

int imm_20_10_1_11_19_12 (unsigned int word)
{
	return imm_31_12(word);
}

void memory_access(int IR,int index);
void writeback(int IR,int index);

/**********************************************************/
void extract_r(int IR,int index,int inst_index)
{
	Ra = reg[rs1(IR)];
	Rb = reg[rs2(IR)];
	Rd = rd(IR);
	Rd_buffer.push_back(Rd);
	Rd_index.push_back(inst_index);

	int opc = opcode(IR);
	int f3 = func3(IR);
	int f7 = func7(IR);		
	if(opc==51)
	{
		if(f3== 0 && f7==0)
			Control_line[index] = 1;			//add inst

		else if(f3== 0 && f7==32)
			Control_line[index] = 2;			//sub inst

		else if(f3==1 && f7==0)	
			Control_line[index] = 3;			//sll inst

		else if(f3==2 && f7==0)	
			Control_line[index] = 4;	//slt inst

		else if(f3==3 && f7==0)	
			Control_line[index] = 5;		//sltu inst

		else if(f3==4 && f7==0)			
			Control_line[index] = 6;			//xor inst

		else if(f3==5 && f7==0)
			Control_line[index] = 7;			//srl inst

		else if(f3==5 && f7==32)
			Control_line[index] = 8;			//sra inst

		else if(f3==6 && f7==0)
			Control_line[index] = 9;			//or inst

		else if(f3==7 && f7==0)
			Control_line[index] = 10;			//and inst

		else if(f3==0 && f7==1)
			Control_line[index] = 11;			//mul inst

		else if(f3==1 && f7==1)
			Control_line[index] = 12;			//mulh inst

		else if(f3==2 && f7==1)
			Control_line[index] = 13;			//mulhsu inst
		
		else if(f3==3 && f7==1)
			Control_line[index] = 14;			//mulhu inst
		
		else if(f3==4 && f7==1)
			Control_line[index] = 15;			//div inst
		
		else if(f3==5 && f7==1)
			Control_line[index] = 16;			//divu inst
		
		else if(f3==6 && f7==1)
			Control_line[index] = 17;			//rem inst
		
		else if(f3==7 && f7==1)
			Control_line[index] = 18;			//remu inst
	}

	else if(opc==59)
	{
		if(f3==0 && f7==0)			//addw inst
			Control_line[index] = 21;
		
		else if(f3==0 && f7==32)			//subw inst
			Control_line[index] = 22;

		else if(f3==1 && f7==0)			//sllw inst
			Control_line[index] = 23;
		
		else if(f3==5 && f7==0)			//srlw inst
			Control_line[index] = 24;

		else if(f3==5 && f7==48)
			Control_line[index] = 25;			//sraw inst
		
		else if(f3==0 && f7==1 )
			Control_line[index] = 26;				//mulw inst
		
		else if(f3==4 && f7==1 )
			Control_line[index] = 27;					//divw inst
		
		else if(f3==7 && f7==1)
			Control_line[index] = 19;			//remw inst
		
		else if(f3==7 && f7==1)
			Control_line[index] = 20;			//remuw inst	
	}		
}

void extract_i(int IR,int index,int inst_index)
{
	Ra = reg[rs1(IR)];
	imm = imm_11_0(IR);
	int temp = 0x00000800;
	temp = temp & imm;
	if(temp==0x800)
	{
		temp = 0xfffff000;
		imm = imm | temp;
	}
	Rd = rd(IR);
	Rd_buffer.push_back(Rd);
	Rd_index.push_back(inst_index);
	int opc = opcode(IR);
	int f3 = func3(IR);

	//cout<<"extract i-------Rd:"<<Rd<<" Ra:"<<Ra<<" rs1:"<<rs1(IR)<<" imm:"<<imm<<endl;

	if(opc==3)
	{	
		if(f3==0)
			Control_line[index] = 28;			//lb inst
		
		else if(f3==1)
			Control_line[index] = 29;			//lh inst		
		
		else if(f3==2)
			Control_line[index] = 30;			//lw inst		
		
		else if(f3==3)
			Control_line[index] = 31;			//ld inst		

		else if(f3==4)
			Control_line[index] = 32;			//lbu inst		

		else if(f3==5)
			Control_line[index] = 33;			//lhu inst		

		else if(f3==6)
			Control_line[index] = 34;			//lwu inst	
	}

	else if(opc==19)
	{	
		if(f3==0)
			Control_line[index] = 35;			//addi inst
		
		else if(f3==1)
			Control_line[index] = 36;			//slli inst DOUBTFUL		

		else if(f3==2)
			Control_line[index] = 37;		//slti inst		
		
		else if(f3==3)
			Control_line[index] = 38;		//sltiu inst					
		
		else if(f3==4)
			Control_line[index] = 39;			//xori inst		
		
		else if(f3==5)
		{		
			int f7 = func7(IR);
			if(f7==0)
				Control_line[index] = 40;			//srli inst		DOUBTFULL			
		
			else if(f7==32)
				Control_line[index] = 41;			//srai inst DOUBTFULL
		}
		
		else if(f3==6)
			Control_line[index] = 42;			//ori	inst			
		
		else if(f3==7)
			Control_line[index] = 43;		//andi inst			
	}

	else if(opc==27)
	{
		if(f3==0)
			Control_line[index] = 44;		//addiw inst				
		
		else if(f3==1)
		{
			int f7 = func7(IR);
			if(f7==0)
				Control_line[index] = 45;		//slliw inst
		}
		
		else if(f3==5)
		{
			int f7 = func7(IR);
			if(f7==0)
				Control_line[index] = 46;			//srliw inst
	
			else if(f7==32)
				Control_line[index] = 47;		//sraiw inst
		}
	}

	else if(opc==103)
	{
		if(f3==0)
		{
			Control_line[index] = 48;		//jalr inst
			if(b_terminate==0)
			{
				IRq.erase(IRq.end()-1);        //flush			print_Control_line();
				Control_line.erase(Control_line.end()-1);
			}
			b_terminate=0,flag=0;
			PC-=4;
			temp_PC = PC;
			PC -= 4;
			PC = Ra+imm;
			fetch();
		}
	}
}

void extract_s(int IR,int index)
{
	Ra = reg[rs1(IR)];
	Rb = reg[rs2(IR)];
	imm = (imm_11_5(IR) * 32) + imm_4_0(IR) ;
	int temp = 0x00000800;
	temp = temp & imm;
	if(temp==0x800)
	{
		temp = 0xfffff000;
		imm = imm | temp;
	}
	int f3 = func3(IR);

	if(f3==0)
		Control_line[index] = 49;		//sb inst

	else if(f3==1)
		Control_line[index] = 50;  		//sh inst
	
	else if(f3==2)
		Control_line[index] = 51;  		//sw inst	
}

void extract_sb(int IR,int index)
{
	//cout << "extract_sb____________________ : " << IR << endl;
	Ra = reg[rs1(IR)];
	Rb = reg[rs2(IR)];
	unsigned int temp = imm_12_10_5(IR);
	imm = temp& 0b1000000;
	imm = imm * 32;
	temp = temp & 0b0111111;
	temp = temp * 16;
	imm = imm | temp;
	temp = imm_4_1_11(IR);
	temp = temp & 0b11110;
	temp = temp/2;
	imm = imm | temp;
	temp = imm_4_1_11(IR);
	temp = temp& 0b00001;
	temp = temp * 1024;
	imm = imm | temp;
	temp = 0x00000800;		// for negative immediate
	temp = temp & imm;
	if(temp==0x800)
	{
		temp = 0xfffff000;
		imm = imm | temp;
	}
	imm = imm << 1;

	//printf(" IR: %x, imm: %d", IR,imm);
	
	
	int f3 = func3(IR);
	if(f3==0)
	{
		Control_line[index] = 52;		//beq		
		//cout << "___________________________________" << endl;
	}
		
	
	else if(f3==1)
	{
		Control_line[index] = 53;		//bne

	}
	
	
	else if(f3==4)
	{
		Control_line[index] = 54;		//blt
	
	}
	
	else if(f3==5)
	{
		Control_line[index] = 55;		//bge
	
	}
	
	
	else if(f3==6)
	{
		Control_line[index] = 56;		//bltu

	
	}
	
	else if(f3==7)
	{
		Control_line[index] = 57;		//bgeu
	
	}
		
		//cout << "extract_sb-----DATA FORWARDING" << endl;
		if((Control_line[index]>=52 && Control_line[index]<=57))
		{
			int inst_type = Control_line[index];
			int lw = 1;
			if(Control_line[index-1]>=28 && Control_line[index-1]<=34)
				lw =1;
			//cout << "inst_type" << inst_type << endl;
			int dtemp = branch_dependency(rs1(IR),rs2(IR),inst_index,inst_type);
			if(dtemp==1)
			{
				if(lw==1)
				{
					if(index>=2)
					{
						writeback(IRq[index-2],index-2);
					}
					memory_access(IRq[index-1],index-1);
					Ra=Ry;
					Rb = Ry;
					clock_cycles++;
				}
				else
				{
					Ra = Rz;				//Ra, Rb is overwritten.
					Rb = Rz;
				}
			}
			else if(dtemp==2)
			{
				if(lw==1)
				{
					if(index>=2)
					{
						writeback(IRq[index-2],index-2);
					}
					memory_access(IRq[index-1],index-1);
					Ra=Ry;
					Rb = reg[rs2(IR)];
					clock_cycles++;
				}
				else
				{
					Ra = Rz;
					Rb = reg[rs2(IR)];
				}
			}
			else if(dtemp==3)
			{
				if(lw==1)
				{
					if(index>=2)
					{
						writeback(IRq[index-2],index-2);
					}
					memory_access(IRq[index-1],index-1);
					Ra=reg[rs1(IR)];
					Rb = Ry;
					clock_cycles++;
				}
				else
				{
					Ra=reg[rs1(IR)];
					Rb=Rz;
				}
			}
			else if(dtemp==4)
			{
				Ra=Ry;
				Rb=Ry;
			}
			else if(dtemp==5)
			{
				if(lw==1)
				{
					if(index>=2)
					{
						writeback(IRq[index-2],index-2);
					}
					memory_access(IRq[index-1],index-1);
					Ra = Ry;
					Rb = reg[rs2(IR)];
					clock_cycles++;
					stall1=1;
				}
				else
				{
					Ra=Rz;
					Rb=Ry;
				}
			}
			else if(dtemp==6)
			{
				Ra=reg[rs1(IR)];
				Rb=Ry;
			}
			else if(dtemp==7)
			{
				if(lw==1)
				{
					if(index>=2)
					{
						writeback(IRq[index-2],index-2);
					}
					memory_access(IRq[index-1],index-1);
					Ra=reg[rs1(IR)];
					Rb = Ry;
					clock_cycles++;
					stall1=1;
				}
				else
				{
					Ra=Ry;
					Rb=Rz;
				}
			}
			else if(dtemp==8)
			{
				Ra=Ry;
				Rb=reg[rs2(IR)];
			}
			else if(dtemp==9)
			{
				Ra=Ry;
				Rb=Ry;
			}
			else if(dtemp==10)
			{
				Ra=Ry;
				Rb=reg[rs2(IR)];
			}
			else if(dtemp==11)
			{
				Ra=reg[rs1(IR)];
				Rb=Ry;
			}

			else 
			{
				Ra = reg[rs1(IR)];
				Rb = reg[rs2(IR)];
			}
			//cout<<"Ra:"<<Ra<<" Rb:"<<Rb<< " dtemp:"<<dtemp<<endl;
		}
	int temp_Rz=0;
	if(Control_line[index]==52)
	{
		cout << "beq " << "Ra:"<<Ra<<" Rb:"<<Rb<< " dtemp:"<<endl;
		if(Ra==Rb)
		{
			temp_Rz=1;	
		}					//beq
		else
		{
			temp_Rz=0;
		}
	}
	else if(Control_line[index]==53)
	{
		if(Ra!=Rb)
		{
			temp_Rz=1;
		}					//bne
		else
		{
			temp_Rz=0;
		}
	}
	else if(Control_line[index]==54)
	{
		cout << "blt" << "Ra:"<<Ra<<" Rb:"<<Rb<< " dtemp:" << endl;
		if(Ra<Rb)
		{
			temp_Rz=1;
			cout << "done" << endl;
		}					//blt
		else
			temp_Rz=0;
		
	}
	else if(Control_line[index]==55)
	{
		cout << "bge" << "Ra:"<<Ra<<" Rb:"<<Rb<< " dtemp:" <<endl;
		if(Ra>=Rb)
		{
			temp_Rz=1;
		}					//bge					
		else
		{
			temp_Rz=0;
		}
	}	
	else if(Control_line[index]==56)
	{
		unsigned int Ra_temp = Ra, Rb_temp = Rb;
		if(Ra_temp<Rb_temp)
		{
			temp_Rz=1;
		}					//bltu
		else
		{
			temp_Rz=0;
		}
	}	
	else if(Control_line[index]==57)
	{
		unsigned int Ra_temp = Ra, Rb_temp = Rb;
		if(Ra_temp>=Rb_temp)
		{
			temp_Rz=1;
		}					//bgeu					
		else
		{
			temp_Rz=0;
		}
	}
	
	if(temp_Rz==1)
	{
		cout<<"-------HEY------"<< endl;
		//printf("%x\n",IRq[IRq.size()-1]);
		//cout << "IR-------------------" << IR << endl;
		if(b_terminate==0)
		{
			IRq.erase(IRq.end()-1);        //flush
			print_Control_line();
			Control_line.erase(Control_line.end()-1);
		}
		b_terminate=0,flag=0;
		//cout<<"-------HEY2------";
		//printf("%x\n",IRq[index]);
		clock_cycles++;
		//cout<<"-------------------------------clock_cycles:"<<clock_cycles<<endl;
		
		//IRq.push_back(0);
		//cout<<"PC intial:"<<PC<<endl;
		PC=PC+imm-8;
		//printf(" IR: %x, imm: %d\n", IR,imm);
		//cout<<"PC final:"<<PC<<endl;
		//printf("extract_sb_initial-----IR:%x\n",exit_IR);
		fetch();
		//printf("%x\n",IRq[IRq.size()-1]);
		//printf("%x\n",IR);
		//cout << IRq.size() << flag << endl;
		//printf("extract_sb_final-----IR:%x\n",exit_IR);
		branch_taken_flag=1;
		if(exit_IR!=0xEF000011)
		{
			//cout<<"extract_sb if case";
			//printf("%x\n",exit_IR);
				
		}
		
	}
	
	else if(temp_Rz==0)
	{		
		branch_taken_flag=0;
	}
	
}

void extract_u(int IR,int index,int inst_index)
{
	imm = imm_31_12(IR);
	int temp = 0x00080000;
	temp = temp & imm;
	if(temp == 0x80000)
	{
		temp = 0xfff00000;
		imm = imm | temp;
	}
	Rd = rd(IR);
	Rd_buffer.push_back(Rd);
	Rd_index.push_back(inst_index);
	int opc = opcode(IR);

	if(opc==23)
		Control_line[index] = 58;		//auipc
	
	else if(opc == 55)
		Control_line[index] = 59;		//lui 
}

void extract_uj(int IR,int index,int inst_index)
{
	Rd = rd(IR);
	Rd_buffer.push_back(Rd);
	Rd_index.push_back(inst_index);
	unsigned int temp = imm_20_10_1_11_19_12(IR);
	imm = temp & 0b00000000000011111111;
	imm = imm * 2048;
	temp = temp & 0b01111111111000000000;
	temp = temp/512;
	imm = imm | temp;
	temp = imm_20_10_1_11_19_12(IR);
	temp = temp & 0b00000000000100000000;
	temp = temp * 4;
	imm = imm | temp;
	temp = imm_20_10_1_11_19_12(IR);
	temp = temp & 0b10000000000000000000;
	imm = temp | imm;
	temp = 0x00080000;
	temp = temp & imm;
	if(temp == 0x80000)
	{
		temp = 0xfff00000;
		imm = imm | temp;
	}
	imm = imm << 1;
	Control_line[index] = 60;		//jal
	if(b_terminate==0)
	{
		IRq.erase(IRq.end()-1);        //flush			print_Control_line();
		Control_line.erase(Control_line.end()-1);
	}
	b_terminate=0,flag=0;
	PC-=4;
	temp_PC=PC;
	PC -= 4; 
	PC+=imm;
	fetch();
} 

void decode(int IR,int index)
{
	inst_index++;
	int opc = opcode(IR);
	int f3 = func3(IR);
	//printf("PC-------->%d                 decode Ir : %x\n",PC,IR );
	if( opc==3 || opc==19 || opc==27 || opc == 103)
		extract_i(IR,index,inst_index);
	else if( opc==51 || opc==59)
		extract_r(IR,index,inst_index);
	else if(opc==35)			//In Reference sheet sd is given I Format.    ?????
		extract_s(IR,index);
	else if(opc==99)
		extract_sb(IR,index);
	else if(opc==23 || opc ==55)
		extract_u(IR,index,inst_index);
	else if(opc==111)
		extract_uj(IR,index,inst_index);
}

// Registry update
void writeback(int IR,int index)
{
	if(Control_line[index]==49||Control_line[index]==50||Control_line[index]==51||Control_line[index]==52||Control_line[index]==53||Control_line[index]==54||Control_line[index]==55||Control_line[index]==56||Control_line[index]==57)
	{
		cout<<"No write required\n";
		return;
	}

	else if(Control_line[index]==48||Control_line[index]==60)
	{
		cout<<"temp_PC written\n";
		reg[Rd_buffer[0]]=temp_PC;
	}
	else
	{
		reg[Rd_buffer[0]] = Ry;
		cout << "reg:" << Rd_buffer[0] << "-" << reg[Rd_buffer[0]] << endl;
	}
	
	if(Rd_buffer.size()>=1)
	{
		Rd_buffer.erase(Rd_buffer.begin());	
	}
	if(Rd_index.size()>=1)
	{
		Rd_index.erase(Rd_index.begin());	
	}
	return;
}

void execute(int IR,int index)
{
	stall1=0;
	//only for pipelining , data forwading
	if(knob2==1)
	{
		if( (Control_line[index]<=27 && Control_line[index]>=1)||(Control_line[index]>=49 && Control_line[index]<=51)/* || (Control_line[index]>=49 && Control_line[index]<=57)*/ )
		{
			int inst_type = Control_line[index];
			int dtemp = dependency(rs1(IR),rs2(IR),inst_index,inst_type);
			if(dtemp==1)
			{
				Ra = Ry;
				Rb = reg[rs2(IR)];
			}
			else if(dtemp==2)
			{
				Rb = Ry;
				Ra = reg[rs1(IR)];
			}
			else
			{
				Ra = reg[rs1(IR)];
				Rb = reg[rs2(IR)];
			}
		}
		else if (Control_line[index]>=28 && Control_line[index]<=48)
		{
			int inst_type = Control_line[index];
			int dtemp = dependency(rs1(IR),inst_index,inst_type);
			if(dtemp==1)
			{
				Ra = Ry;
			}
			else
			{
				Ra = reg[rs1(IR)];
			}
			//cout << "execute----DATA FORWARDING ";
			//cout<<"rs1:"<<rs1(IR)<<" Ra"<<Ra<<endl;
		}
	}
	
	// stalling
	else if(knob2==2)
	{
		cout << "STALLING" << endl;
		if( (Control_line[index]<=27 && Control_line[index]>=1) || (Control_line[index]>=49 && Control_line[index]<=57) )
		{
			int inst_type = Control_line[index];
			int dtemp = dependency(rs1(IR),rs2(IR),inst_index,inst_type);
			if(dtemp==1 || dtemp==2)
			{
				//cout << "stall" << endl;
				if(IRq.size()!=1)
					writeback(IRq[index-1],index-1);
				clock_cycles++;
				stall=1;
			}
			Ra = reg[rs1(IR)];
			Rb = reg[rs2(IR)];
		}
		else if (Control_line[index]>=28 && Control_line[index]<=48)
		{
			int inst_type = Control_line[index];
			int dtemp = dependency(rs1(IR),inst_index,inst_type);
			if(dtemp==1)
			{
				//cout << "if----stall" << endl;
				if(IRq.size()!=1)
					writeback(IRq[index-1],index-1);
				clock_cycles++;
				stall=1;
			}
			else
			{
				//cout << "else-----stall" << endl;
			}
			Ra = reg[rs1(IR)];
		}
	}//

	if(Control_line[index]==1)
	{
		Rz = Ra + Rb;		//add
	}
	else if(Control_line[index]==2)
	{
		Rz = Ra - Rb;		//sub
	}
	else if(Control_line[index]==3)
	{
		Rz = Ra << Rb;      //sll
	}
	else if(Control_line[index]==4)
	{
		Rz=(Ra<Rb)?1:0;		//slt
	}
	else if(Control_line[index]==5)
	{
		unsigned int Ra_temp,Rb_temp;
		Ra_temp = Ra;
		Rb_temp = Rb;
		Rz=(Ra_temp<Rb_temp)?1:0;		//sltu
	}
	else if(Control_line[index]==6)
	{
		Rz=Ra^Rb;		//xor
	}
	else if(Control_line[index]==7)
	{
				Rz=(int)((unsigned int)Ra >> Rb);		//srl
	}
	else if(Control_line[index]==8)
	{

		Rz=Ra>>Rb;		//sra
	}
	else if(Control_line[index]==9)
	{
		Rz=Ra|Rb;		//or
	}
	else if(Control_line[index]==10)
	{
		Rz=Ra&Rb;		//and
	}
	else if(Control_line[index]==11)
	{
		Rz=Ra*Rb;		//mul
	}
	else if(Control_line[index]==12)
	{
				//mulh
	}
	else if(Control_line[index]==13)
	{
				//mulhsu
	}
	else if(Control_line[index]==14)
	{
				//mulhu
	}
	else if(Control_line[index]==15)
	{
		Rz = Ra/Rb;		//div
	}
	else if(Control_line[index]==16)
	{
		unsigned int Ra_temp,Rb_temp;
		Ra_temp = Ra;
		Rb_temp = Rb;
		Rz = Ra_temp/Rb_temp;		//divu
	}
	else if(Control_line[index]==17)
	{
		Rz = Ra%Rb;		//rem
	}
	else if(Control_line[index]==18)
	{
		unsigned int Ra_temp,Rb_temp;
		Ra_temp = Ra;
		Rb_temp = Rb;		//remu
		Rz = Ra_temp%Rb_temp;
	}
	else if(Control_line[index]==21)
	{
		Rz = Ra + Rb;		//addw
	}
	else if(Control_line[index]==22)
	{
		Rz = Ra - Rb;		//subw
	}
	else if(Control_line[index]==19)
	{
		Rz = Ra%Rb;		//remw
	}
	else if(Control_line[index]==20)
	{
		unsigned int Ra_temp,Rb_temp;
		Ra_temp = Ra;
		Rb_temp = Rb;		
		Rz = Ra_temp%Rb_temp;		//remuw
	}
	else if(Control_line[index] == 23)
	{
		Rz=Ra<<Rb;			//sllw inst	
	}	
	else if(Control_line[index] == 24)
	{
		Rz=(int)((unsigned int)Ra >> Rb);			//srlw inst
	}
	else if(Control_line[index] == 25)
	{
		Rz=Ra>>Rb;				//sraw inst
	}
	else if(Control_line[index] == 26)
	{
		Rz = Ra * Rb;					//mulw inst
	}
	else if(Control_line[index] == 27)
	{
		Rz = Ra/Rb;					//divw inst
	}
	else if(Control_line[index] == 28)
	{
		Rz = Ra + imm;				//lb inst
	}
	else if(Control_line[index] == 29)
	{
		Rz = Ra + imm;				//lh inst
	}
	else if(Control_line[index] == 30)
	{
		Rz = Ra + imm;			//lw inst
	}
	else if(Control_line[index]==31)
	{
							//ld inst
	}
	else if(Control_line[index]==32)
	{
		Rz=imm+Ra;		//lbu
	}
	else if(Control_line[index]==33)
	{
		Rz=imm+Ra;	//lhu
	}
	else if(Control_line[index]==34)
	{
		Rz=imm+Ra;	//lwu
	}
	else if(Control_line[index]==35)
	{
		Rz= Ra + imm;		// addi
	}
	else if(Control_line[index]==36)
	{
		Rz=Ra<<imm;				//slli
	}
	else if(Control_line[index]==37)
	{
		Rz=(Ra<imm)?1:0;			//slti
	}
	else if(Control_line[index]==38)
	{
		unsigned int Ra_temp = Ra;
		unsigned int imm_temp = imm & 0x00000fff;
		Rz=(Ra_temp<imm_temp)?1:0;			//sltiu
	}
	else if(Control_line[index]==39)
	{
		Rz=Ra^imm;				//xori	
	}
	else if(Control_line[index]==40)
	{
		Rz=(int)((unsigned int)Ra >> imm);				//srli inst		DOUBTFULL
	}
	else if(Control_line[index]==41)
	{
		Rz=Ra>>imm;					//srai inst DOUBTFULL
	}
	else if(Control_line[index]==42)
	{
		Rz=Ra|imm;				//ori
	}
	else if(Control_line[index]==43)
	{
		Rz=Ra & imm;			//andi
	}
	else if(Control_line[index]==44)
	{
		Rz=Ra + imm;			//addi
	}
	else if(Control_line[index]==45)
	{
		Rz=(int)((unsigned int)Ra >> imm);			//srliw	
	}
	else if(Control_line[index]==46)
	{
		Rz=Ra<<imm;				//slliw
	}
	else if(Control_line[index]==47)
	{
		Rz=Ra>>imm;					//sraiw inst
	}
	else if(Control_line[index]==48)
	{	
		Rz= Ra+imm;				//jalr inst
	}
	else if(Control_line[index]==49)
	{
		Rz = Ra + imm;					
		Rm = Rb;					//sb inst
	}
	else if(Control_line[index]==50)
	{
		Rz = Ra + imm;					
		Rm = Rb;					//sh inst
	}
	else if(Control_line[index]==51)
	{
		Rz = Ra + imm;					//sw inst
		Rm = Rb;
	}
/*	else if(Control_line[index]==52)
	{
		if(Ra==Rb)
		{
			Rz=1;
		}					//beq
		else
		{
			Rz=0;
		}
	}
	else if(Control_line[index]==53)
	{
		if(Ra!=Rb)
		{
			Rz=1;
		}					//bne
		else
		{
			Rz=0;
		}
	}
	else if(Control_line[index]==54)
	{
		if(Ra<Rb)
		{
			Rz=1;
		}					//blt
		else
		{
			Rz=0;
		}
	}
	else if(Control_line[index]==55)
	{
		if(Ra>=Rb)
		{
			Rz=1;
		}					//bge					
		else
		{
			Rz=0;
		}
	}	
	else if(Control_line[index]==56)
	{
		unsigned int Ra_temp = Ra, Rb_temp = Rb;
		if(Ra_temp<Rb_temp)
		{
			Rz=1;
		}					//bltu
		else
		{
			Rz=0;
		}
	}	
	else if(Control_line[index]==57)
	{
		unsigned int Ra_temp = Ra, Rb_temp = Rb;
		if(Ra_temp>=Rb_temp)
		{
			Rz=1;
		}					//bgeu					
		else
		{
			Rz=0;
		}
	}*/
	else if(Control_line[index]==58)
	{
		Rz = imm;			//auipc
	}
	else if(Control_line[index]==59)
	{
		Rz=imm<<12;					//lui
	}
	else if(Control_line[index]==60)
	{
							//jal
	}
}

void memory_access(int IR,int index)
{
	stall=0;
    if(Control_line[index]==28)
    {
        Ry=memory[Rz];
		int temp = 0x00000080;
		temp = temp & Ry;
		if(temp==0x80)
		{
			temp = 0xffffff00;
			Ry = Ry | temp;
		}
    }

    else if(Control_line[index]==29)
    {
    	int temp=read_word(memory,Rz);
    	Ry=temp & 0x0000ffff;
    	//printf("Ry:%x\n",Ry);
    	int temp2 = 0x00008000;
		temp2 = temp2 & Ry;
		if(temp2==0x8000)
		{
			temp2 = 0xffff0000;
			Ry = Ry | temp2;
		}
    }

	else if(Control_line[index]==30)
    {
		Ry = read_word(memory,Rz);
    }

    else if(Control_line[index]==32)
    {
        Ry=memory[Rz];
    }

    else if(Control_line[index]==33)
    {
    	int temp=read_word(memory,Rz);
    	Ry=temp & 0x0000ffff;
    }

    else if(Control_line[index]==34)
    {
		Ry = read_word(memory,Rz); 
    }

    else if(Control_line[index]==49)
    {
        memory[Rz]=Rm;
    }

    else if(Control_line[index]==50)
    {
        memory[Rz+1]=Rm%256;
        Rm_temp=Rm/256;
        memory[Rz]=Rm_temp%256;
    }

    else if(Control_line[index]==51)
    {
        Rm_temp=Rm;
		write_word(memory,Rz,Rm);
    }

    else if(Control_line[index]==48)
    {
    }

    else if(Control_line[index]==60)
    {
    }

    else if(Control_line[index]==52||Control_line[index]==53||Control_line[index]==54||Control_line[index]==55||Control_line[index]==56||Control_line[index]==57)
    {
    }

    else
    {
    	Ry=Rz;
    }
}

void wrap_up()
{
	IRq.erase(IRq.begin());
	Control_line.erase(Control_line.begin());
}

int main()
{
	PC = 0x0;reg[2]=sp;
	load_program_memory();
	cout << "Enter 1 for forwarding\nEnter 2 for stalling\n";
	cin >> knob2;
	/********************************* PIPELINING *****************************************/
	fetch();
	clock_cycles++;
	if(IR!=0xEF000011)
	{
		while(1)
		{
		if(IRq.size()==1 && flag==0)
		{
			fetch();
			if(IR==0)
				break;			
			if(IR!=0xEF000011)
			{
				decode(IRq[0],0);
				clock_cycles++;
			}
			else
			{
				decode(IRq[0],0);
				execute(IRq[0],0);
				memory_access(IRq[0],0);
				clock_cycles+=3;
			}	
		}
		else if(IRq.size()==2 && flag==0)
		{
			fetch();
			if(IR==0)
				break;
			if(IR!=0xEF000011)
			{
				execute(IRq[0],0);
				decode(IRq[1],1);
				clock_cycles++;
			}
			else
			{
				execute(IRq[0],0);
				decode(IRq[1],1);
				if(stall1==0)
				{
					memory_access(IRq[0],0);
				}
				execute(IRq[1],1);
				clock_cycles+=2;
			}
		}
		else if(IRq.size()==3 && flag==0)
		{
			fetch();
			if(IR==0)
				break;
			if(IR!=0xEF000011)
			{
				if(stall1==0)
				{
					memory_access(IRq[0],0);
				}
				execute(IRq[1],1);
				decode(IRq[2],2);
				clock_cycles++;
			}
			else
			{
				if(stall1==0)
				{
					memory_access(IRq[0],0);
				}
				execute(IRq[1],1);
				decode(IRq[2],2);
				clock_cycles++;
			}
		}
		else if(IRq.size()==4 && flag==0)
		{
			fetch();
			if(IR==0)
				break;
			if(IR!=0xEF000011)
			{
				if(stall==0 && stall1==0)
				{
					writeback(IRq[0],0);
				}
				if(stall1==0)
				{
					memory_access(IRq[1],1);
				}
				execute(IRq[2],2);
				decode(IRq[3],3);
				clock_cycles++;
			}
		}
		else if(IRq.size()==5 && flag==0)
		{
			while(1)
			{
				fetch();
				if(IR==0xEF000011 || IR==0)
					break;
				if(stall==0 && stall1==0)
				{
					writeback(IRq[0],0);
				}
				if(stall1==0)
				{
					memory_access(IRq[1],1);
				}
				execute(IRq[2],2);
				decode(IRq[3],3);
				clock_cycles++;
			}
			wrap_up();
		}
		else if(IRq.size()==4 && flag==1)
		{
			if(stall==0 && stall1==0)
			{
				writeback(IRq[0],0);
			}
			if(stall1==0)
			{
				memory_access(IRq[1],1);
			}
			execute(IRq[2],2);
			decode(IRq[3],3);

			wrap_up();
			clock_cycles++;
		}
		else if(IRq.size()==3 && flag==1)
		{
			if(stall==0 && stall1==0)
			{
				writeback(IRq[0],0);
			}
			if(stall1==0)
			{
				memory_access(IRq[1],1);
			}
			execute(IRq[2],2);

			wrap_up();
			clock_cycles++;
		}
		else if(IRq.size()==2 && flag==1)
		{
			if(stall==0)
			{
				writeback(IRq[0],0);
			}
			memory_access(IRq[1],1);

			wrap_up();
			clock_cycles++;
		}
		else if(IRq.size()==1 && flag==1)
		{
			//if(IR==0)
					//break;
			if(stall==0)
			{
				writeback(IRq[0],0);
			}
			wrap_up();
			clock_cycles++;
		}
		else
		{
			break;
		}
		}
	}
	/*************************** pipelining *******************************/
	write_data_memory();
	cout << "No. of clock cycles: " << clock_cycles << endl;
	cout << "Total instructions: " << inst_index << endl;
}
