.class public ./src/VC/CodeGen/test/t3
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
.var 0 is this L./src/VC/CodeGen/test/t3; from L0 to L1
.var 1 is i I from L0 to L1
	iconst_1
	istore_1
L2:
	iload_1
	bipush 10
	if_icmplt L4
	iconst_0
	goto L5
L4:
	iconst_1
L5:
	ifeq L3
L6:
	iload_1
	iconst_2
	iadd
	istore_1
L7:
	goto L2
L3:
	iload_1
	ireturn
L1:
	nop
	
	; set limits used by this method
.limit locals 2
.limit stack 2
.end method
.method public static main([Ljava/lang/String;)V
	return
	
	; set limits used by this method
.limit locals 2
.limit stack 0
.end method
