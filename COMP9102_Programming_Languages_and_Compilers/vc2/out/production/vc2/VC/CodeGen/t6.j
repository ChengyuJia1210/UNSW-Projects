.class public ./src/VC/CodeGen/t6
.super java/lang/Object
	
	
	; standard class static initializer 
.method static <clinit>()V
	
	
	; set limits used by this method
.limit locals 0
.limit stack 0
	return
.end method
	
	; standard constructor initializer 
.method public <init>()V
.limit stack 1
.limit locals 1
	aload_0
	invokespecial java/lang/Object/<init>()V
	return
.end method
.method f()I
L0:
.var 0 is this L./src/VC/CodeGen/t6; from L0 to L1
.var 1 is i I from L0 to L1
	iconst_0
	istore_1
.var 2 is j I from L0 to L1
	iconst_5
	istore_2
.var 3 is c F from L0 to L1
	fconst_1
	fstore_3
	iload_1
	iload_2
	iadd
	fload_3
	iconst_2
	fmul
	fadd
	fstore_3
L1:
	nop
	
	; set limits used by this method
.limit locals 4
.limit stack 3
.end method
.method public static main([Ljava/lang/String;)V
	return
	
	; set limits used by this method
.limit locals 2
.limit stack 0
.end method
