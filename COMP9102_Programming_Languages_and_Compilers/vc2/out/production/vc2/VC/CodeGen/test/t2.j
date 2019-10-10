.class public ./src/VC/CodeGen/test/t2
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
.method f([I)I
L0:
.var 0 is this L./src/VC/CodeGen/test/t2; from L0 to L1
.var 1 is a [I from L0 to L1
	bipush 10
	newarray int
	dup
	iconst_0
	iconst_0
	iastore
	dup
	iconst_1
	iconst_0
	iastore
	dup
	iconst_2
	iconst_0
	iastore
	dup
	iconst_3
	iconst_0
	iastore
	dup
	iconst_4
	iconst_0
	iastore
	dup
	iconst_5
	iconst_0
	iastore
	dup
	bipush 6
	iconst_0
	iastore
	dup
	bipush 7
	iconst_0
	iastore
	dup
	bipush 8
	iconst_0
	iastore
	dup
	bipush 9
	iconst_0
	iastore
	astore_1
.var 2 is i I from L0 to L1
	iconst_0
	istore_2
	iconst_1
	istore_2
L2:
	iload_2
	bipush 10
	if_icmplt L4
	iconst_0
	goto L5
L4:
	iconst_1
L5:
	ifeq L3
L6:
	iaload_1
	iload_2
	pop
L7:
	iload_2
	iconst_1
	iadd
	istore_2
	goto L2
L3:
L1:
	nop
	
	; set limits used by this method
.limit locals 3
.limit stack 4
.end method
.method public static main([Ljava/lang/String;)V
	return
	
	; set limits used by this method
.limit locals 2
.limit stack 0
.end method
