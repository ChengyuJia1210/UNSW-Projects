.class public ./src/VC/CodeGen/test/t10
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
.var 1 is vc$ L./src/VC/CodeGen/test/t10; from L0 to L1
	new ./src/VC/CodeGen/test/t10
	dup
	invokenonvirtual ./src/VC/CodeGen/test/t10/<init>()V
	astore_1
	iconst_0
	ifeq L2
	iconst_1
	ifne L4
	iconst_1
	ifne L4
	iconst_0
	goto L5
L4:
	iconst_1
L5:
	ifeq L2
	iconst_1
	goto L3
L2:
	iconst_0
L3:
	ifne L6
	iconst_1
	goto L7
L6:
	iconst_0
L7:
	pop
L1:
	return
	
	; set limits used by this method
.limit locals 2
.limit stack 2
.end method
