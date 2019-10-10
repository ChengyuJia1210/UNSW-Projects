.class public ./src/VC/CodeGen/t8
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
.method f(Z)I
L0:
.var 0 is this L./src/VC/CodeGen/t8; from L0 to L1
.var 1 is c Z from L0 to L1
.var 2 is a Z from L0 to L1
	iconst_1
	istore_2
.var 3 is b Z from L0 to L1
	iconst_1
	istore_3
	iconst_0
	istore_3
	iload_2
	dup
	istore_3
	istore_1
L1:
	nop
	
	; set limits used by this method
.limit locals 4
.limit stack 2
.end method
.method public static main([Ljava/lang/String;)V
	return
	
	; set limits used by this method
.limit locals 2
.limit stack 0
.end method
