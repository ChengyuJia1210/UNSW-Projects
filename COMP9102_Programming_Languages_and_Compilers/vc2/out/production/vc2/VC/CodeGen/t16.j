.class public ./src/VC/CodeGen/t16
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
.method add(II)I
L0:
.var 0 is this L./src/VC/CodeGen/t16; from L0 to L1
.var 1 is i1 I from L0 to L1
.var 2 is i2 I from L0 to L1
	iload_1
	iload_2
	iadd
	ireturn
L1:
	nop
	
	; set limits used by this method
.limit locals 3
.limit stack 2
.end method
.method public static main([Ljava/lang/String;)V
L0:
.var 0 is argv [Ljava/lang/String; from L0 to L1
.var 1 is vc$ L./src/VC/CodeGen/t16; from L0 to L1
	new ./src/VC/CodeGen/t16
	dup
	invokenonvirtual ./src/VC/CodeGen/t16/<init>()V
	astore_1
	aload_1
	iconst_1
	iconst_2
	invokevirtual ./src/VC/CodeGen/t16/add(II)I
	pop
L1:
	return
	
	; set limits used by this method
.limit locals 2
.limit stack 3
.end method
