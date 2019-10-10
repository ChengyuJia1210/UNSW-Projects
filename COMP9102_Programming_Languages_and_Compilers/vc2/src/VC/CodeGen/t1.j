.class public t1
.super java/lang/Object
	
.field static a I
	
	; standard class static initializer 
.method static <clinit>()V
	
	iconst_0
	putstatic t1/a I
	
	; set limits used by this method
.limit locals 0
.limit stack 1
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
.var 0 is this Lt1; from L0 to L1
.var 1 is i I from L0 to L1
	iconst_2
	istore_1
	iload_1
	iconst_1
	if_icmpeq L3
	iconst_0
	goto L4
L3:
	iconst_1
L4:
	ifeq L2
L5:
	iconst_5
	bipush 6
	iadd
	istore_1
L6:
L2:
	iload_1
	ireturn
L1:
	nop
	
	; set limits used by this method
.limit locals 2
.limit stack 2
.end method
.method g()I
L0:
.var 0 is this Lt1; from L0 to L1
	getstatic t1/a I
	iconst_2
	if_icmpeq L3
	iconst_0
	goto L4
L3:
	iconst_1
L4:
	ifeq L2
	iconst_5
	putstatic t1/a I
L2:
L1:
	nop
	
	; set limits used by this method
.limit locals 1
.limit stack 2
.end method
.method public static main([Ljava/lang/String;)V
	return
	
	; set limits used by this method
.limit locals 2
.limit stack 0
.end method
