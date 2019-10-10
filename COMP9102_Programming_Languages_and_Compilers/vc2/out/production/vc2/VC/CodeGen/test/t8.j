.class public ./src/VC/CodeGen/test/t8
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
.method f(I[B)F
L0:
.var 0 is this L./src/VC/CodeGen/test/t8; from L0 to L1
.var 1 is a I from L0 to L1
	iconst_0
	istore 1
.var 2 is b [B from L0 to L1
	bipush 10
	newarray boolean
	dup
	iconst_0
	iconst_0
	bastore
	dup
	iconst_1
	iconst_0
	bastore
	dup
	iconst_2
	iconst_0
	bastore
	dup
	iconst_3
	iconst_0
	bastore
	dup
	iconst_4
	iconst_0
	bastore
	dup
	iconst_5
	iconst_0
	bastore
	dup
	bipush 6
	iconst_0
	bastore
	dup
	bipush 7
	iconst_0
	bastore
	dup
	bipush 8
	iconst_0
	bastore
	dup
	bipush 9
	iconst_0
	bastore
	astore_2
.var 3 is c F from L0 to L1
	fconst_0
	fstore 3
	baload_2
	iconst_5
	pop
L1:
	nop
	
	; set limits used by this method
.limit locals 4
.limit stack 4
.end method
.method public static main([Ljava/lang/String;)V
L0:
.var 0 is argv [Ljava/lang/String; from L0 to L1
.var 1 is vc$ L./src/VC/CodeGen/test/t8; from L0 to L1
	new ./src/VC/CodeGen/test/t8
	dup
	invokenonvirtual ./src/VC/CodeGen/test/t8/<init>()V
	astore_1
	return
L1:
	return
	
	; set limits used by this method
.limit locals 2
.limit stack 2
.end method
