.class public ./src/VC/CodeGen/test/t6
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
.var 0 is this L./src/VC/CodeGen/test/t6; from L0 to L1
.var 1 is a [I from L0 to L1
	bipush 7
	newarray int
	dup
	iconst_0
	bipush 6
	iastore
	dup
	iconst_1
	iconst_5
	iastore
	dup
	iconst_2
	iconst_4
	iastore
	dup
	iconst_3
	iconst_1
	iastore
	dup
	iconst_4
	iconst_3
	iastore
	dup
	iconst_5
	iconst_1
	iastore
	dup
	bipush 6
	iconst_2
	iastore
	astore_1
.var 2 is i I from L0 to L1
	iconst_0
	istore 2
	iconst_0
	istore_2
L2:
	iload_2
	bipush 7
	if_icmplt L4
	iconst_0
	goto L5
L4:
	iconst_1
L5:
	ifeq L3
	iaload_1
	iload_2
	invokestatic VC/lang/System.putInt(I)V
	iload_2
	iconst_1
	iadd
	istore_2
	goto L2
L3:
	iaload_1
	iconst_1
	istore_2
	iload_2
	ireturn
L1:
	nop
	
	; set limits used by this method
.limit locals 3
.limit stack 4
.end method
.method public static main([Ljava/lang/String;)V
L0:
.var 0 is argv [Ljava/lang/String; from L0 to L1
.var 1 is vc$ L./src/VC/CodeGen/test/t6; from L0 to L1
	new ./src/VC/CodeGen/test/t6
	dup
	invokenonvirtual ./src/VC/CodeGen/test/t6/<init>()V
	astore_1
	return
L1:
	return
	
	; set limits used by this method
.limit locals 2
.limit stack 2
.end method
