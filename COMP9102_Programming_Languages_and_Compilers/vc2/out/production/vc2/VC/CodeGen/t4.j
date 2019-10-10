.class public t4
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
.var 0 is this Lt4; from L0 to L1
.var 1 is i I from L0 to L1
	iconst_0
	istore_1
.var 2 is j F from L0 to L1
	iload_1
	pop
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
	iload_1
	i2f
	fstore_2
L7:
	iload_1
	iconst_1
	iadd
	istore_1
	goto L2
L3:
	iload_1
	ireturn
L1:
	nop
	
	; set limits used by this method
.limit locals 3
.limit stack 2
.end method
.method g()F
L0:
.var 0 is this Lt4; from L0 to L1
.var 1 is i I from L0 to L1
	iconst_0
	istore_1
.var 2 is j F from L0 to L1
	iconst_1
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
	iload_1
	i2f
	fstore_2
L7:
	iload_1
	iconst_1
	iadd
	istore_1
	goto L2
L3:
	fload_2
	fneg
	fneg
	fstore_2
	fload_2
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
.var 1 is vc$ Lt4; from L0 to L1
	new t4
	dup
	invokenonvirtual t4/<init>()V
	astore_1
.var 2 is i Z from L0 to L1
	iconst_1
	istore_2
.var 3 is j F from L0 to L1
	aload_1
	invokevirtual t4/g()F
	fstore_3
	iload_2
	fneg
	istore_2
L1:
	return
	
	; set limits used by this method
.limit locals 4
.limit stack 2
.end method
