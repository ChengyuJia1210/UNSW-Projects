.class public ./src/VC/CodeGen/t7
.super java/lang/Object
	
.field static c Z
	
	; standard class static initializer 
.method static <clinit>()V
	
	iconst_1
	putstatic ./src/VC/CodeGen/t7/c Z
	
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
.method f(Z)I
L0:
.var 0 is this L./src/VC/CodeGen/t7; from L0 to L1
.var 1 is b Z from L0 to L1
.var 2 is i I from L0 to L1
.var 3 is j I from L0 to L1
.var 4 is k I from L0 to L1
.var 5 is a Z from L0 to L1
	iconst_1
	istore 5
	iconst_1
	dup
	istore 4
	dup
	istore_3
	istore_2
	iconst_1
	iconst_2
	dup
	istore_3
	dup
	istore_2
	iadd
	pop
	invokestatic VC/lang/System/putBool(Z)V
L1:
	nop
	
	; set limits used by this method
.limit locals 6
.limit stack 3
.end method
.method g()V
	
	; return may not be present in a VC function returning void
	; The following return inserted by the VC compiler
	return
	
	; set limits used by this method
.limit locals 1
.limit stack 0
.end method
.method public static main([Ljava/lang/String;)V
L0:
.var 0 is argv [Ljava/lang/String; from L0 to L1
.var 1 is vc$ L./src/VC/CodeGen/t7; from L0 to L1
	new ./src/VC/CodeGen/t7
	dup
	invokenonvirtual ./src/VC/CodeGen/t7/<init>()V
	astore_1
	aload_1
	iconst_1
	invokevirtual ./src/VC/CodeGen/t7/f(Z)I
	pop
	aload_1
	invokevirtual ./src/VC/CodeGen/t7/g()V
L1:
	return
	
	; set limits used by this method
.limit locals 2
.limit stack 2
.end method
