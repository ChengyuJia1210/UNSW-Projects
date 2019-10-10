.class public t1
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
.method f(IFI)F
L0:
.var 0 is this Lt1; from L0 to L1
.var 1 is a I from L0 to L1
.var 2 is b F from L0 to L1
.var 3 is i I from L0 to L1
	iload_3
	iconst_0
	if_icmplt L4
	iconst_0
	goto L5
L4:
	iconst_1
L5:
	ifeq L2
L6:
	fload_2
	iload_1
	i2f
	fadd
	fstore_2
	fload_2
	invokestatic VC/lang/System/putFloat(F)V
L7:
	goto L3
L2:
L8:
	fload_2
	iload_1
	i2f
	fsub
	fstore_2
	fload_2
	invokestatic VC/lang/System/putFloat(F)V
L9:
L3:
	fload_2
	freturn
L1:
	nop
	
	; set limits used by this method
.limit locals 4
.limit stack 2
.end method
.method public static main([Ljava/lang/String;)V
L0:
.var 0 is argv [Ljava/lang/String; from L0 to L1
.var 1 is vc$ Lt1; from L0 to L1
	new t1
	dup
	invokenonvirtual t1/<init>()V
	astore_1
	aload_1
	iconst_5
	bipush 6
	i2f
	iconst_1
	invokevirtual t1/f(IFI)F
	pop
	return
L1:
	return
	
	; set limits used by this method
.limit locals 2
.limit stack 4
.end method
