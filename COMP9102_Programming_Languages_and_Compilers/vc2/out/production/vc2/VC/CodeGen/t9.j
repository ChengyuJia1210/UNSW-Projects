.class public t9
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
.var 0 is this Lt9; from L0 to L1
.var 1 is b1 Z from L0 to L1
	iconst_0
	istore_1
.var 2 is b2 Z from L0 to L1
	iconst_1
	istore_2
	iload_1
	ifeq L3
	iload_2
	ifeq L3
	iconst_1
	goto L4
L3:
	iconst_0
L4:
	ifeq L2
	iconst_1
	istore_1
L2:
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
