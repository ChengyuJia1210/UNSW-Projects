.class public ./src/VC/CodeGen/t12
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
.method g([I)F
L0:
.var 0 is this L./src/VC/CodeGen/t12; from L0 to L1
.var 1 is a [I from L0 to L1
	iconst_1
	newarray int
	dup
	iconst_0
	iconst_0
	iastore
	astore_1
.var 2 is b [F from L0 to L1
	iconst_3
	newarray float
	dup
	iconst_0
	iconst_1
	i2f
	fastore
	dup
	iconst_1
	iconst_2
	i2f
	fastore
	dup
	iconst_2
	iconst_3
	i2f
	fastore
	astore_2
.var 3 is i F from L0 to L1
	iconst_1
	freturn
L1:
	nop
	
	; set limits used by this method
.limit locals 4
.limit stack 4
.end method
.method l()I
L0:
.var 0 is this L./src/VC/CodeGen/t12; from L0 to L1
.var 1 is c [B from L0 to L1
	iconst_3
	newarray boolean
	dup
	iconst_0
	iconst_1
	ifne L2
	iconst_1
	goto L3
L2:
	iconst_0
L3:
	bastore
	dup
	iconst_1
	iconst_1
	bastore
	dup
	iconst_2
	iconst_0
	bastore
	astore_1
	iconst_1
	ireturn
L1:
	nop
	
	; set limits used by this method
.limit locals 2
.limit stack 4
.end method
.method k()I
L0:
.var 0 is this L./src/VC/CodeGen/t12; from L0 to L1
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
	iconst_1
	ireturn
L1:
	nop
	
	; set limits used by this method
.limit locals 2
.limit stack 4
.end method
.method f([B)I
L0:
.var 0 is this L./src/VC/CodeGen/t12; from L0 to L1
.var 1 is b [B from L0 to L1
	iconst_5
	newarray boolean
	dup
	iconst_0
	iconst_0
	bastore
	dup
	iconst_1
	iconst_0
	bastore
	dup
	iconst_2
	iconst_0
	bastore
	dup
	iconst_3
	iconst_0
	bastore
	dup
	iconst_4
	iconst_0
	bastore
	astore_1
.var 2 is a [I from L0 to L1
	bipush 7
	newarray int
	dup
	iconst_0
	iconst_1
	iastore
	dup
	iconst_1
	iconst_2
	iastore
	dup
	iconst_2
	iconst_3
	iastore
	dup
	iconst_3
	iconst_4
	iastore
	dup
	iconst_4
	iconst_5
	iastore
	dup
	iconst_5
	bipush 6
	iastore
	dup
	bipush 6
	bipush 7
	iastore
	astore_2
.var 3 is c [F from L0 to L1
	iconst_5
	newarray float
	dup
	iconst_0
	fconst_0
	fastore
	dup
	iconst_1
	fconst_0
	fastore
	dup
	iconst_2
	fconst_0
	fastore
	dup
	iconst_3
	fconst_0
	fastore
	dup
	iconst_4
	fconst_0
	fastore
	astore_3
	iconst_2
	ireturn
L1:
	nop
	
	; set limits used by this method
.limit locals 4
.limit stack 4
.end method
.method public static main([Ljava/lang/String;)V
L0:
.var 0 is argv [Ljava/lang/String; from L0 to L1
.var 1 is vc$ L./src/VC/CodeGen/t12; from L0 to L1
	new ./src/VC/CodeGen/t12
	dup
	invokenonvirtual ./src/VC/CodeGen/t12/<init>()V
	astore_1
.var 2 is a [I from L0 to L1
	iconst_1
	newarray int
	dup
	iconst_0
	iconst_1
	iastore
	astore_2
.var 3 is j F from L0 to L1
	aload_1
	iaload_2
	invokevirtual ./src/VC/CodeGen/t12/g([I)F
	fstore_3
L1:
	return
	
	; set limits used by this method
.limit locals 4
.limit stack 4
.end method
