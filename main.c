#include <stdio.h>
#include <stdlib.h>
#include <string.h>
///指令
/*
逻辑运算指令：+-* / 求余数
bit运算指令：
跳转指令：jmp 
堆栈指令：
赋值指令：=
中断：

数据表示： [@address, 4]

@address1 = @address2+@address3
@address1 = @address2  | #immediate


寄存器？ 内存地址？
默认都是32bit操作。
寄存器和内存使用统一的地址；
没有特殊寄存器。
地址指针。
变量地址。
read pc;
set pc;
halt(exit);
sleep;
int

所有运算都是32bit的。

运算指令：
0x00000010 ADD @address1, @address2; @address1=@address1+@address2
0x00000011 MINUS @address1, @address2;@address1=@address1-@address2
0x00000012 MUL @address1, @address2; @address1=@address1 * @address2
0x00000013 DIV @address1, @address2; @address1=@address1 / @address2

0x000000014 OR @address1, @address2;
0x00000015 AND @address1, @address2;
0x00000016 NOT @address1, @address2;
TEST @address1
SHL @address1  @address2
SHR @address1  @address2


堆栈指令：
0x00000017 PUSH @address1
0x00000018 POP @address1
0x00000019 SETSP @address1
0x0000001A GETSP @address1

跳转指令：
0x0000001B JMP @address1
0x0000001C CALL @address1
0x0000001D RET


赋值：
0x0000001E MOV @address1, @address2; @address1=@address2
0x0000001F MOVI @address1, val1; @address1=val1

虚拟机专用指令：
0x00000020 SETMEMBYTESIZE @address1
SLEEP @address1(ms)
PUTC @address1
GETC @address1



*/

typedef enum TAG_E_OPRS
{
	OPRS_ADD=0x00000010,
	OPRS_MINUS,
	OPRS_MUL,
	OPRS_DIV,

	OPRS_OR,
	OPRS_AND,
	OPRS_NOT,
	OPRS_TEST,
	OPRS_SHR,
	OPRS_SHL,

	OPRS_PUSH,
	OPRS_POP,
	OPRS_SETSP,
	OPRS_GETSP,

	OPRS_JMP,
	OPRS_CALL,
	OPRS_RET,

	OPRS_MOV,
	OPRS_MOVI,


	OPRS_SETMEMBYTESIZE,
	OPRS_SLEEP,
	OPRS_PUTCHAR,
	OPRS_GETCHAR,

	OPRS_EXIT,



}E_OPRS;

typedef unsigned  uint32;

typedef struct tag_t_tiny_vm
{
	uint32 pc;
	uint32 sp;
	uint32 *mem;
	uint32 mem_byte_len;

}t_tiny_vm;

uint32 tiny_vm_init(t_tiny_vm* t)
{
	t->mem = NULL;
	t->mem_byte_len = 0;
	t->pc = 0;
	t->sp = 0;

	return 0;
}

uint32 tiny_vm_free(t_tiny_vm* t)
{
	if(t->mem!=NULL)
	{
		free(t->mem);
		t->mem = NULL;
		t->mem_byte_len = 0;
	}

	return 0;
}

uint32 tiny_vm_set_program(t_tiny_vm* t, uint32 *program, uint32 program_byte_len)
{
	
	t->mem_byte_len =program_byte_len+1024*4;
	t->mem = (uint32*)malloc(t->mem_byte_len);
	memcpy(t->mem, program, program_byte_len);
	//t->program = program;
	//t->program_byte_len = program_byte_len;
	return 0;
}
uint32 tiny_vm_resize(t_tiny_vm* t, uint32 program_byte_len)
{
	if(program_byte_len  > t->mem_byte_len)
	{
		uint32* nmem = (uint32*)malloc(program_byte_len);
		memcpy(nmem, t->mem, t->mem_byte_len);
		free(t->mem);
		t->mem_byte_len =program_byte_len;
		t->mem = nmem;
	}
	return 0;
}


uint32 tiny_vm_fetch(t_tiny_vm* t)
{

	uint32 mRet = t->mem[t->pc/4];
	t->pc+=4;
	return mRet;
}
uint32 tiny_vm_get(t_tiny_vm* t, uint32 addr)
{

	return t->mem[addr/4];
}
uint32 tiny_vm_set(t_tiny_vm* t, uint32 addr, uint32 val)
{

	return t->mem[addr/4] = val;
}

uint32 tiny_vm_step(t_tiny_vm* t)
{
	uint32 pc = t->pc;//current pc
	E_OPRS mOpr = (E_OPRS)tiny_vm_fetch(t);
	uint32 mD1;
	uint32 mD2;
	uint32 i;
	switch(mOpr)
	{
	case OPRS_ADD:
		mD1 = tiny_vm_fetch(t);
		mD2 = tiny_vm_fetch(t);
		tiny_vm_set(t,mD1, tiny_vm_get(t, mD1) + tiny_vm_get(t, mD2));

		break;
	case OPRS_MINUS:
		mD1 = tiny_vm_fetch(t);
		mD2 = tiny_vm_fetch(t);
		tiny_vm_set(t,mD1, tiny_vm_get(t, mD1) - tiny_vm_get(t, mD2));
		break;
	case OPRS_MUL:
		mD1 = tiny_vm_fetch(t);
		mD2 = tiny_vm_fetch(t);
		tiny_vm_set(t,mD1, tiny_vm_get(t, mD1) * tiny_vm_get(t, mD2));
		break;
	case OPRS_DIV:	//此处应该对于mD2数据为0时做处理。
		mD1 = tiny_vm_fetch(t);
		mD2 = tiny_vm_fetch(t);
		tiny_vm_set(t,mD1, tiny_vm_get(t, mD1) / tiny_vm_get(t, mD2));
		break;

	case OPRS_OR:
		mD1 = tiny_vm_fetch(t);
		mD2 = tiny_vm_fetch(t);
		tiny_vm_set(t,mD1, tiny_vm_get(t, mD1) | tiny_vm_get(t, mD2));
		break;
	case OPRS_AND:
		mD1 = tiny_vm_fetch(t);
		mD2 = tiny_vm_fetch(t);
		tiny_vm_set(t,mD1, tiny_vm_get(t, mD1) & tiny_vm_get(t, mD2));
		break;
	case OPRS_NOT:
		mD1 = tiny_vm_fetch(t);
		tiny_vm_set(t,mD1, ~ tiny_vm_get(t, mD1) );
		break;
	case OPRS_TEST:	 //test @address1, @address2; if(@address2) @address1=1;else @address1=0
		mD1 = tiny_vm_fetch(t);
		mD2 = tiny_vm_fetch(t);
		tiny_vm_set(t,mD1, tiny_vm_get(t, mD2)?1:0  );
		break;


	case OPRS_SHR:
		mD1 = tiny_vm_fetch(t);
		mD2 = tiny_vm_fetch(t);
		tiny_vm_set(t,mD1, tiny_vm_get(t, mD1) >> tiny_vm_get(t, mD2)  );
		break;

	case OPRS_SHL:
		mD1 = tiny_vm_fetch(t);
		mD2 = tiny_vm_fetch(t);
		tiny_vm_set(t,mD1, tiny_vm_get(t, mD1) << tiny_vm_get(t, mD2)  );
		break;
	case OPRS_PUSH://push @address1, @address2. @address1=data start address. @address2=data byte number
		mD1 = tiny_vm_fetch(t);
		mD2 = tiny_vm_fetch(t);
		for(i=0;i<tiny_vm_get(t, mD2);i+=4)
		{
			tiny_vm_set(t,t->sp, tiny_vm_get(t, mD1+i));
			t->sp-=4;
		}
		break;

	case OPRS_POP://pop @address1, @address2. @address1=data start address. @address2=data byte number
		mD1 = tiny_vm_fetch(t);
		mD2 = tiny_vm_fetch(t);
		for(i=0;i<tiny_vm_get(t, mD2);i+=4)
		{
			tiny_vm_set(t,mD1+i, tiny_vm_get(t, t->sp));
			t->sp+=4;
		}
		break;
	case OPRS_SETSP:
		mD1 = tiny_vm_fetch(t);
		t->sp = tiny_vm_get(t, mD1);
		break;

	case OPRS_GETSP:
		mD1 = tiny_vm_fetch(t);
		tiny_vm_set(t, mD1, t->sp);
		break;
	case OPRS_JMP:
		mD1 = tiny_vm_fetch(t);
		t->pc = tiny_vm_get(t, mD1);
		break;

	case OPRS_CALL:	//call @address1
		mD1 = tiny_vm_fetch(t);
		tiny_vm_set(t,t->sp, pc+8);
		t->sp-=4;
		t->pc = tiny_vm_get(t, mD1);
		break;

	case OPRS_RET:

		t->pc = tiny_vm_get(t,t->sp);
		t->sp+=4;
		break;
	case OPRS_MOV://mov @address1, @address2
		mD1 = tiny_vm_fetch(t);
		mD2 = tiny_vm_fetch(t);
		tiny_vm_set(t,mD1, tiny_vm_get(t, mD2));
		break;

	case OPRS_MOVI:
		mD1 = tiny_vm_fetch(t);
		mD2 = tiny_vm_fetch(t);
		tiny_vm_set(t,mD1,mD2);
		break;

	case OPRS_SETMEMBYTESIZE:
		mD1 = tiny_vm_fetch(t);
		tiny_vm_resize(t, tiny_vm_get(t, mD1));
		break;


	case OPRS_SLEEP:
		break;


	case OPRS_PUTCHAR:
		mD1 = tiny_vm_fetch(t);
		printf("%c",(unsigned char)tiny_vm_get(t, mD1));
		break;


	case OPRS_GETCHAR:
		{
		uint32 mD = getchar();
		mD1 = tiny_vm_fetch(t);
		tiny_vm_set(t, mD1, mD);
		}
		break;

	case OPRS_EXIT:
		return 0;
		break;
	default:
		return -1;
		break;

	}

	return 1;
}




uint32 tiny_vm_run(t_tiny_vm* t)
{

	while(1)
	{
		uint32 mRet = tiny_vm_step(t);
		switch(mRet)
		{
		case 0://exit
			return 0;
			break;
		case 1://exe one command ok.
			break;
		default:
			return mRet;
			break;	
		}
	}
	return 0;
}

int main( int argc, const char * argv[] )
{
	if(argc<2)
	{
		printf("usage: cmd program_file.bin");
		return 0;
	}
	{
	FILE *fp = fopen(argv[1], "rb");
	uint32 mFileSize = 0;
	uint32* mFileBuf = NULL;
	t_tiny_vm m_t_vm;
	
	if(fp==NULL)return 0;

	fseek(fp,0, SEEK_END);
	mFileSize = ftell(fp);
	fseek(fp,0,SEEK_SET);
	mFileBuf = (uint32*)malloc(mFileSize);
	fread(mFileBuf, mFileSize, 1, fp);
	fclose(fp);
	tiny_vm_init(&m_t_vm);
	tiny_vm_set_program(&m_t_vm, mFileBuf, mFileSize);
	tiny_vm_run(&m_t_vm);

	}
  return 0;
}

