.class public ./src/VC/CodeGen/t15
.super java/lang/Object
	
.field static a V
.field static b V
	
	; standard class static initializer 
.method static <clinit>()V
	
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
	putstatic ./src/VC/CodeGen/t15/a [I
	iconst_3
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
	putstatic ./src/VC/CodeGen/t15/b [I
	
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
.var 1 is vc$ L./src/VC/CodeGen/t15; from L0 to L1
	new ./src/VC/CodeGen/t15
	dup
	invokenonvirtual ./src/VC/CodeGen/t15/<init>()V
	astore_1
	getstatic ./src/VC/CodeGen/t15/a [I
	iconst_0
	pop
L1:
	return
	
	; set limits used by this method
.limit locals 2
.limit stack 2
.end method
