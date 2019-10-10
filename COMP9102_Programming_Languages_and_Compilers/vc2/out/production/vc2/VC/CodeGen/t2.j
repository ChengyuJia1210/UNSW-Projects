.class public ./src/VC/CodeGen/t2
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
.var 0 is this L./src/VC/CodeGen/t2; from L0 to L1
.var 1 is i I from L0 to L1
	iconst_0
	istore_+1
.var 2 is j Z from L0 to L1
	iconst_0
	istore_+2
L2:
	iconst_1
	ifeq L3
L4:
	iload_1
	pop
L5:
	goto L2
L3:
	iconst_0
	ireturn
L1:
	nop
	
	; set limits used by this method
.limit locals 3
.limit stack 1
.end method
.method public static main([Ljava/lang/String;)V
	return
	
	; set limits used by this method
.limit locals 2
.limit stack 0
.end method
