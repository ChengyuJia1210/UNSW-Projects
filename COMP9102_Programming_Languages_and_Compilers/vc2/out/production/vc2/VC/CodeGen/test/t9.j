.class public ./src/VC/CodeGen/test/t9
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
.method public static main([Ljava/lang/String;)V
L0:
.var 0 is argv [Ljava/lang/String; from L0 to L1
.var 1 is vc$ L./src/VC/CodeGen/test/t9; from L0 to L1
	new ./src/VC/CodeGen/test/t9
	dup
	invokenonvirtual ./src/VC/CodeGen/test/t9/<init>()V
	astore_1
.var 2 is i I from L0 to L1
	iconst_0
	istore 2
.var 3 is j I from L0 to L1
	iconst_0
	istore 3
.var 4 is h I from L0 to L1
	iconst_0
	istore 4
	iconst_5
	dup
	istore 4
	dup
	istore_3
	istore_2
	iload_2
	iload 4
	isub
	iload 4
	iadd
	istore_2
	iload_2
	iload_3
	isub
	iload 4
	iload_2
	iload_3
	isub
	imul
	iload_2
	idiv
	iadd
	istore_3
	return
L1:
	return
	
	; set limits used by this method
.limit locals 5
.limit stack 4
.end method
