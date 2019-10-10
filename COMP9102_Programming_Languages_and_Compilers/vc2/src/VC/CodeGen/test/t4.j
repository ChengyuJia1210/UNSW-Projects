.class public ./src/VC/CodeGen/test/t4
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
.method f(I)F
L0:
.var 0 is this L./src/VC/CodeGen/test/t4; from L0 to L1
.var 1 is i I from L0 to L1
.var 2 is j F from L0 to L1
	bipush 7
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
	fload_2
	fneg
	pop
	iconst_1
	ifne L8
	iconst_1
	goto L9
L8:
	iconst_0
L9:
	pop
	iload_1
	invokestatic VC/lang/System.putInt(I)V
L7:
	iload_1
	iconst_1
	iadd
	istore_1
	goto L2
L3:
	ldc 0.1
	freturn
L1:
	nop
	
	; set limits used by this method
.limit locals 3
.limit stack 2
.end method
.method public static main([Ljava/lang/String;)V
L0:
.var 0 is argv [Ljava/lang/String; from L0 to L1
.var 1 is vc$ L./src/VC/CodeGen/test/t4; from L0 to L1
	new ./src/VC/CodeGen/test/t4
	dup
	invokenonvirtual ./src/VC/CodeGen/test/t4/<init>()V
	astore_1
	return
L1:
	return
	
	; set limits used by this method
.limit locals 2
.limit stack 2
.end method
