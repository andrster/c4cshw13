#define NUMMEMORY 65536 /* maximum number of data words in memory */
#define NUMREGS 8 /* number of machine registers */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#define ADD 0
#define NOR 1
#define LW 2
#define SW 3
#define BEQ 4
#define JALR 5 /* JALR will not implemented for Project 3 */
#define HALT 6
#define NOOP 7
#define MAXLINELENGTH 1000
#define NOOPINSTRUCTION 0x1c00000

typedef struct IFIDStruct {
	int instr;
	int pcPlus1;
} IFIDType;

typedef struct IDEXStruct {
	int instr;
	int pcPlus1;
	int readRegA;
	int readRegB;
	int offset;
} IDEXType;

typedef struct EXMEMStruct {
	int instr;
	int branchTarget;
	int aluResult;
	int readRegB;
} EXMEMType;

typedef struct MEMWBStruct {
	int instr;
	int writeData;
} MEMWBType;

typedef struct WBENDStruct {
	int instr;
	int writeData;
} WBENDType;

typedef struct stateStruct {
	int pc;
	int instrMem[NUMMEMORY];
	int dataMem[NUMMEMORY];
	int reg[NUMREGS];
	int numMemory;
	IFIDType IFID;
	IDEXType IDEX;
	EXMEMType EXMEM;
	MEMWBType MEMWB;
	WBENDType WBEND;
	int cycles; /* number of cycles run so far */
} stateType;
int
field0(int instruction)
{
	return((instruction >> 19) & 0x7);
}

int
field1(int instruction)
{
	return((instruction >> 16) & 0x7);
}

int
field2(int instruction)
{
	return(instruction & 0xFFFF);
}

int
opcode(int instruction)
{
	return(instruction >> 22);
}

void
printInstruction(int instr)
{

	char opcodeString[10];

	if (opcode(instr) == ADD) {
		strcpy(opcodeString, "add");
	}
	else if (opcode(instr) == NOR) {
		strcpy(opcodeString, "nor");
	}
	else if (opcode(instr) == LW) {
		strcpy(opcodeString, "lw");
	}
	else if (opcode(instr) == SW) {
		strcpy(opcodeString, "sw");
	}
	else if (opcode(instr) == BEQ) {
		strcpy(opcodeString, "beq");
	}
	else if (opcode(instr) == JALR) {
		strcpy(opcodeString, "jalr");
	}
	else if (opcode(instr) == HALT) {
		strcpy(opcodeString, "halt");
	}
	else if (opcode(instr) == NOOP) {
		strcpy(opcodeString, "noop");
	}
	else {
		strcpy(opcodeString, "data");
	}
	printf("%s %d %d %d\n", opcodeString, field0(instr), field1(instr),
		field2(instr));
}

void printState(stateType *);
void binaryConvert(char arr[], int num)
{
	for (int i = 31; i >= 0; i--)
	{
		if (num / (int)pow(2.0, (double)i) == 1)
		{
			arr[i] = '1';
			num = num - (int)pow(2.0, (double)i);
		}
		else
		{
			arr[i] = '0';
		}
	}
}
int offsetnum(char arr[])
{
	int num = 0;
	//int j = 0;
	for (int i = 0; i < 16; i++)
	{
		//fprintf(stderr, "%d\n", i);
		if (arr[i] == '1')
		{
			num = num + (int)pow(2.0, (double)i);
		}

		//j++;
	}
	//fprintf(stderr, "%d\n", num);
	return num;
}
int offset(int num)
{
	/* convert a 16-bit number into a 32-bit Linux integer */
	if (num & (1 << 15)) {
		num -= (1 << 16);
	}
	return(num);
}
int reg_convert(char first, char second, char third)
{
	int sum = 0;
	if (third == '1')
	{
		sum = sum + 1;
	}
	if (second == '1')
	{
		sum = sum + 2;
	}
	if (first == '1')
	{
		sum = sum + 4;
	}
	return sum;
}
//void add(stateType *statePtr, char arr[])
//{
//	int regA = reg_convert(arr[21], arr[20], arr[19]);
//	int regB = reg_convert(arr[18], arr[17], arr[16]);
//	int destreg = reg_convert(arr[2], arr[1], arr[0]);
//	statePtr->reg[destreg] = statePtr->reg[regA] + statePtr->reg[regB];
//	statePtr->pc++;
//}
//void nor(stateType *statePtr, char arr[])
//{
//	int regA = reg_convert(arr[21], arr[20], arr[19]);
//	int regB = reg_convert(arr[18], arr[17], arr[16]);
//	int destreg = reg_convert(arr[2], arr[1], arr[0]);
//	statePtr->reg[destreg] = ~(statePtr->reg[regA] | statePtr->reg[regB]);
//	statePtr->pc++;
//	//fprintf(stderr, "%d\n", statePtr->reg[regB]);
//}
//void jalr(stateType *statePtr, char arr[])
//{
//	int regA = reg_convert(arr[21], arr[20], arr[19]);
//	int regB = reg_convert(arr[18], arr[17], arr[16]);
//	statePtr->reg[regB] = statePtr->pc + 1;
//	statePtr->pc = statePtr->reg[regA];
//}
//void beq(stateType *statePtr, char arr[], int num)
//{
//	int regA = reg_convert(arr[21], arr[20], arr[19]);
//	int regB = reg_convert(arr[18], arr[17], arr[16]);
//	if (statePtr->reg[regA] == statePtr->reg[regB])
//	{
//		//fprintf(stderr, "%d\n", num);
//		statePtr->pc = 1 + statePtr->pc + offset(num);
//	}
//	else
//	{
//		statePtr->pc++;
//	}
//}
//void lw(stateType *statePtr, char arr[], int num)
//{
//	int regA = reg_convert(arr[21], arr[20], arr[19]);
//	int regB = reg_convert(arr[18], arr[17], arr[16]);
//	//fprintf(stderr, "%d\n", num);
//	statePtr->reg[regB] = statePtr->mem[statePtr->reg[regA] + offset(num)];
//	statePtr->pc++;
//}
//void sw(stateType *statePtr, char arr[], int num)
//{
//	int regA = reg_convert(arr[21], arr[20], arr[19]);
//	int regB = reg_convert(arr[18], arr[17], arr[16]);
//	statePtr->mem[statePtr->reg[regA] + offset(num)] = statePtr->reg[regB];
//	statePtr->pc++;
//}
void ifstage()
{

}
void IDStage()
{

}
void EXEStage()
{

}
void MEMStage()
{

}
void WBStage()
{

}
int
main(int argc, char *argv[])
{
	char line[MAXLINELENGTH];
	stateType state;
	stateType newstate;
	FILE *filePtr;


	if (argc != 2) {
		printf("error: usage: %s <machine-code file>\n", argv[0]);
		exit(1);
	}

	filePtr = fopen(argv[1], "r");
	if (filePtr == NULL) {
		printf("error: can't open file %s", argv[1]);
		perror("fopen");
		exit(1);
	}

	/* read in the entire machine-code file into memory */
	for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL;
		state.numMemory++) {

		if (sscanf(line, "%d", state.dataMem + state.numMemory) != 1) {
			printf("error in reading address %d\n", state.numMemory);
			exit(1);
		}
		printf("memory[%d]=%d\n", state.numMemory, state.dataMem[state.numMemory]);
	}
	for (int i = 0; i < state.numMemory; i++)
	{
		state.instrMem[i] = state.dataMem[i];
	}
	int op;
	char current_instruction[32];
	int offset_number;
	//printState(&state);
	int count = 0;
	int lwcount = 0;

	state.IFID.instr = 29360128;
	state.IDEX.instr = 29360128;
	state.EXMEM.instr = 29360128;
	state.MEMWB.instr = 29360128;
	state.WBEND.instr = 29360128;
	while (1)
	{
		//state = newstate;
		printState(&state);

		/* check for halt */
		if (opcode(state.MEMWB.instr) == HALT) {
			printf("machine halted\n");
			printf("total of %d cycles executed\n", state.cycles);
			exit(0);
		}

		newstate = state;
		newstate.cycles++;

		count++;
		//binaryConvert(current_instruction, state.mem[state.pc]);
		op = reg_convert(current_instruction[24], current_instruction[23], current_instruction[22]);
		//fprintf(stderr, "%d\n", ~(5| 3));
		//fprintf(stderr, "%d\n", opcode);
		//fprintf(stderr,"error in opening %s\n", current_instruction)	;
		newstate.IFID.instr = state.instrMem[state.pc];
		newstate.IFID.pcPlus1 = state.pc + 1;
		newstate.pc += 1;
		/*
		21], arr[20], arr[19]);
		int regB = reg_convert(arr[18], arr[17], arr[16]
		*/

		newstate.IDEX.instr = state.IFID.instr;
		newstate.IDEX.pcPlus1 = state.IFID.pcPlus1;
		binaryConvert(current_instruction, state.IFID.instr);
		newstate.IDEX.readRegA = state.reg[field0(state.IFID.instr)];//reg_convert(current_instruction[21],current_instruction[20],current_instruction[19]);
		newstate.IDEX.readRegB = state.reg[field1(state.IFID.instr)]; //reg_convert(current_instruction[18], current_instruction[17], current_instruction[16]);
		newstate.IDEX.offset = offset(field2(state.IFID.instr));
		if ((opcode(state.WBEND.instr) == NOR || opcode(state.WBEND.instr) == ADD) && (field2(state.WBEND.instr) == field0(state.IDEX.instr) || field2(state.WBEND.instr) == field1(state.IDEX.instr)))
		{
			if (field2(state.WBEND.instr) == field0(state.IDEX.instr))
			{
				state.IDEX.readRegA = state.WBEND.writeData;
			}
			if (field2(state.WBEND.instr) == field1(state.IDEX.instr))
			{
				state.IDEX.readRegB = state.WBEND.writeData;
			}
		}
		if ((opcode(state.MEMWB.instr) == NOR || opcode(state.MEMWB.instr) == ADD) && (field2(state.MEMWB.instr) == field0(state.IDEX.instr) || field2(state.MEMWB.instr) == field1(state.IDEX.instr)))
		{
			if (field2(state.MEMWB.instr) == field0(state.IDEX.instr))
			{
				state.IDEX.readRegA = state.MEMWB.writeData;
			}
			if (field2(state.MEMWB.instr) == field1(state.IDEX.instr))
			{
				state.IDEX.readRegB = state.MEMWB.writeData;
			}
		}
		if ((opcode(state.EXMEM.instr) == NOR || opcode(state.EXMEM.instr) == ADD) && (field2(state.EXMEM.instr) == field0(state.IDEX.instr) || field2(state.EXMEM.instr) == field1(state.IDEX.instr)))
		{
			if (field2(state.EXMEM.instr) == field0(state.IDEX.instr))
			{


				state.IDEX.readRegA = state.EXMEM.aluResult;

			}
			if (field2(state.EXMEM.instr) == field1(state.IDEX.instr))
			{
				state.IDEX.readRegB = state.EXMEM.aluResult;
			}
		}
	/*	if ((opcode(state.IDEX.instr) == NOR || opcode(state.IDEX.instr) == ADD) && (field2(state.IDEX.instr) == field0(state.IDEX.instr) || field2(state.IDEX.instr) == field1(state.IDEX.instr)))
		{
			if (field2(state.IDEX.instr) == field0(state.IDEX.instr))
			{

				if (opcode(state.IDEX.instr) == ADD)
				{
					state.IDEX.readRegA = state.IDEX.readRegA + state.IDEX.readRegB;
				}
				else
				{
					state.IDEX.readRegA = ~(state.IDEX.readRegA | state.IDEX.readRegB);
				}
			}
			if (field2(state.IDEX.instr) == field1(state.IDEX.instr))
			{
				if (opcode(state.IDEX.instr) == ADD)
				{
					state.IDEX.readRegB = state.IDEX.readRegA + state.IDEX.readRegB;
				}
				else
				{
					state.IDEX.readRegB = ~(field0(state.IDEX.instr) | field1(state.IDEX.instr));
				}
			}
		}
*/

		if (opcode(state.WBEND.instr) == LW && (field1(state.WBEND.instr) == field0(state.IDEX.instr) || field1(state.WBEND.instr) == field1(state.IDEX.instr)))
		{
			if (field1(state.WBEND.instr) == field0(state.IDEX.instr))
			{
				state.IDEX.readRegA = state.WBEND.writeData;
				//printf("hiiiiiiiiiiiii\n");
			}
			if(field1(state.WBEND.instr) == field1(state.IDEX.instr))
			{
				state.IDEX.readRegB = state.WBEND.writeData;
			}
		}
		if (state.IDEX.pcPlus1 == 5)
		{
			/* code */
			//printf("%d\n", field1(state.MEMWB.instr));
		}
		if (opcode(state.MEMWB.instr) == LW && (field1(state.MEMWB.instr) == field0(state.IDEX.instr) || field1(state.MEMWB.instr) == field1(state.IDEX.instr)))
		{
			//printf("%d\n", field1(state.MEMWB.instr));
			//printf("%d\n", state.MEMWB.writeData);
			if (field1(state.MEMWB.instr) == field0(state.IDEX.instr))
			{
				state.IDEX.readRegA = state.MEMWB.writeData;
			}
			if (field1(state.MEMWB.instr) == field1(state.IDEX.instr))
			{
				//printf("%d\n", field0(newstate.IDEX.instr));

				state.IDEX.readRegB = state.MEMWB.writeData;
			}
		}
		if (opcode(state.EXMEM.instr) == LW && (field1(state.EXMEM.instr) == field0(state.IDEX.instr) || field1(state.EXMEM.instr) == field1(state.IDEX.instr)))
		{
			if (field1(state.EXMEM.instr) == field0(state.IDEX.instr))
			{
				state.IDEX.readRegA = state.dataMem[state.EXMEM.aluResult];
			}
			if (field1(state.EXMEM.instr) == field1(state.IDEX.instr))
			{
				state.IDEX.readRegB = state.dataMem[state.EXMEM.aluResult];
			}
		}
		if (opcode(state.IDEX.instr) == LW && (field1(state.IDEX.instr) == field0(state.IFID.instr) || field1(state.IDEX.instr) == field1(state.IFID.instr)))
		{
			//printf("%s\n", "hi");
			newstate.IFID.instr = state.IFID.instr;
			newstate.IFID.pcPlus1 = state.IFID.pcPlus1;
			newstate.IDEX.instr = 29360128;
			newstate.pc--;
		}

		newstate.EXMEM.instr = state.IDEX.instr;
		//newstate.EXMEM.readRegB = field2(state.IDEX.instr);
		if (opcode(state.IDEX.instr) == SW || opcode(state.IDEX.instr) == LW)
		{
			newstate.EXMEM.aluResult = state.IDEX.readRegA + state.IDEX.offset;
		}
		else if (opcode(state.IDEX.instr) == ADD)
		{
			newstate.EXMEM.aluResult = state.IDEX.readRegA + state.IDEX.readRegB;
		}
		else if (opcode(state.IDEX.instr) == NOR)
		{
			newstate.EXMEM.aluResult = ~(state.IDEX.readRegA | state.IDEX.readRegB);
		}
		else if (opcode(state.IDEX.instr) == BEQ)
		{
			if (state.IDEX.readRegA == state.IDEX.readRegB)
			{
				newstate.EXMEM.aluResult = 1;
			}
			else
			{
				newstate.EXMEM.aluResult = 0;
			}
		}
		
			newstate.EXMEM.readRegB = state.IDEX.readRegB;
		
		//newstate.EXMEM.aluResult;
		newstate.EXMEM.branchTarget = state.IDEX.pcPlus1 + state.IDEX.offset;
		// if (state.IDEX.offset > 32767)
		// {
		// 	/* code */
		// 	newstate.EXMEM.branchTarget = state.IDEX.pcPlus1 + ( -65536|offset(state.IDEX.offset));
		// }

		if (opcode(state.EXMEM.instr) == SW)
		{
			newstate.dataMem[state.EXMEM.aluResult] = state.EXMEM.readRegB;
		}
		//branch and halt
		if (opcode(state.EXMEM.instr) == BEQ  && state.EXMEM.aluResult == 1)
		{
			newstate.pc = state.EXMEM.branchTarget;
			newstate.IFID.instr = 29360128;
			newstate.IDEX.instr = 29360128;
			newstate.EXMEM.instr = 29360128;
		}





		newstate.MEMWB.instr = state.EXMEM.instr;
		if (opcode(state.EXMEM.instr) == LW || opcode(state.EXMEM.instr) == ADD || opcode(state.EXMEM.instr) == NOR)
		{
			/* code */
			newstate.MEMWB.writeData = state.EXMEM.aluResult;
		}

		if (opcode(state.EXMEM.instr) == LW)
		{
			/* code */
			newstate.MEMWB.writeData = state.dataMem[newstate.MEMWB.writeData];
		}
		if (opcode(state.MEMWB.instr) == HALT)
		{
			break;
		}
		if (opcode(state.MEMWB.instr) == ADD || opcode(state.MEMWB.instr) == NOR)
		{
			newstate.reg[field2(state.MEMWB.instr)] = state.MEMWB.writeData;
		}
		if (opcode(state.MEMWB.instr) == LW)
		{
			newstate.reg[field1(state.MEMWB.instr)] = state.MEMWB.writeData;
		}

		newstate.WBEND.instr = state.MEMWB.instr;
		newstate.WBEND.writeData = state.MEMWB.writeData;



		state = newstate;
	}
	//printf("end state\n");
	//printState(&state);
	printf("machine halted\n");
	printf("total of %d instructions executed\n", count);
	printf("final state of machine:\n");
	printState(&state);
	printf("%d\n", lwcount);
	return(0);
}













void
printState(stateType *statePtr)
{
	int i;
	printf("\n@@@\nstate before cycle %d starts\n", statePtr->cycles);
	printf("\tpc %d\n", statePtr->pc);

	printf("\tdata memory:\n");
	for (i = 0; i<statePtr->numMemory; i++) {
		printf("\t\tdataMem[ %d ] %d\n", i, statePtr->dataMem[i]);
	}
	printf("\tregisters:\n");
	for (i = 0; i<NUMREGS; i++) {
		printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
	}
	printf("\tIFID:\n");
	printf("\t\tinstruction ");
	printInstruction(statePtr->IFID.instr);
	printf("\t\tpcPlus1 %d\n", statePtr->IFID.pcPlus1);
	printf("\tIDEX:\n");
	printf("\t\tinstruction ");
	printInstruction(statePtr->IDEX.instr);
	printf("\t\tpcPlus1 %d\n", statePtr->IDEX.pcPlus1);
	printf("\t\treadRegA %d\n", statePtr->IDEX.readRegA);
	printf("\t\treadRegB %d\n", statePtr->IDEX.readRegB);
	printf("\t\toffset %d\n", statePtr->IDEX.offset);
	printf("\tEXMEM:\n");
	printf("\t\tinstruction ");
	printInstruction(statePtr->EXMEM.instr);
	printf("\t\tbranchTarget %d\n", statePtr->EXMEM.branchTarget);
	printf("\t\taluResult %d\n", statePtr->EXMEM.aluResult);
	printf("\t\treadRegB %d\n", statePtr->EXMEM.readRegB);
	printf("\tMEMWB:\n");
	printf("\t\tinstruction ");
	printInstruction(statePtr->MEMWB.instr);
	printf("\t\twriteData %d\n", statePtr->MEMWB.writeData);
	printf("\tWBEND:\n");
	printf("\t\tinstruction ");
	printInstruction(statePtr->WBEND.instr);
	printf("\t\twriteData %d\n", statePtr->WBEND.writeData);
}

