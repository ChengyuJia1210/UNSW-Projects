.class public ./src/VC/CodeGen/test/t7
.super java/lang/Object
	
.field static i I
	
	; standard class static initializer 
.method static <clinit>()V
	
	iconst_0
	putstatic ./src/VC/CodeGen/test/t7/i I
	
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
.method public static main([Ljava/lang/String;)V
L0:
.var 0 is argv [Ljava/lang/String; from L0 to L1
.var 1 is vc$ L./src/VC/CodeGen/test/t7; from L0 to L1
	new ./src/VC/CodeGen/test/t7
	dup
	invokenonvirtual ./src/VC/CodeGen/test/t7/<init>()V
	astore_1
.var 2 is j F from L0 to L1
	fconst_0
	fstore 2
.var 3 is b Z from L0 to L1
	iconst_1
	istore_3
.var 4 is c Z from L0 to L1
	iconst_0
	istore 4
L1:
	return
	
	; set limits used by this method
.limit locals 5
.limit stack 2
.end method
