.class public ./src/VC/CodeGen/t10
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
.var 0 is this L./src/VC/CodeGen/t10; from L0 to L1
.var 1 is b1 Z from L0 to L1
	iconst_1
	istore_1
.var 2 is b2 Z from L0 to L1
	iconst_1
	istore_2
.var 3 is j F from L0 to L1
.var 4 is i I from L0 to L1
	bipush 10
	istore 4
	iload_1
	ifne L4
	iload_2
	ifne L4
	iconst_0
	goto L5
L4:
	iconst_1
L5:
	ifeq L2
	iconst_1
	istore_1
	goto L3
L2:
	iload 4
	i2f
	fstore_3
L3:
L1:
	nop
	
	; set limits used by this method
.limit locals 5
.limit stack 1
.end method
.method g()I
L0:
.var 0 is this L./src/VC/CodeGen/t10; from L0 to L1
.var 1 is i Z from L0 to L1
	iconst_1
	istore_1
	iload_1
	ifne L2
	iconst_1
	goto L3
L2:
	iconst_0
L3:
	ifne L4
	iconst_1
	goto L5
L4:
	iconst_0
L5:
	istore_1
	iconst_1
	ifne L6
	iconst_1
	goto L7
L6:
	iconst_0
L7:
	istore_1
L1:
	nop
	
	; set limits used by this method
.limit locals 2
.limit stack 1
.end method
.method public static main([Ljava/lang/String;)V
	return
	
	; set limits used by this method
.limit locals 2
.limit stack 0
.end method
