.class public ./src/VC/CodeGen/test/t5
.super java/lang/Object
	
.field static i I
.field static b V
	
	; standard class static initializer 
.method static <clinit>()V
	
	bipush 15
	putstatic ./src/VC/CodeGen/test/t5/i I
	iconst_2
	newarray boolean
	dup
	iconst_0
	iconst_1
	bastore
	dup
	iconst_1
	iconst_1
	bastore
	putstatic ./src/VC/CodeGen/test/t5/b [B
	
	; set limits used by this method
.limit locals 0
.limit stack 4
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
.var 1 is vc$ L./src/VC/CodeGen/test/t5; from L0 to L1
	new ./src/VC/CodeGen/test/t5
	dup
	invokenonvirtual ./src/VC/CodeGen/test/t5/<init>()V
	astore_1
.var 2 is a [F from L0 to L1
	bipush 6
	newarray float
	dup
	iconst_0
	iconst_1
	ineg
	ineg
	ineg
	i2f
	fastore
	dup
	iconst_1
	iconst_5
	i2f
	fastore
	dup
	iconst_2
	bipush 6
	ineg
	i2f
	fastore
	dup
	iconst_3
	bipush 9
	ineg
	i2f
	fastore
	dup
	iconst_4
	bipush 7
	i2f
	fastore
	dup
	iconst_5
	bipush 8
	i2f
	fastore
	astore_2
.var 3 is b [B from L0 to L1
	iconst_3
	newarray boolean
	dup
	iconst_0
	iconst_1
	bastore
	dup
	iconst_1
	iconst_0
	bastore
	dup
	iconst_2
	iconst_0
	bastore
	astore_3
.var 4 is i F from L0 to L1
	iconst_1
	i2f
	fstore 4
L1:
	return
	
	; set limits used by this method
.limit locals 5
.limit stack 4
.end method
