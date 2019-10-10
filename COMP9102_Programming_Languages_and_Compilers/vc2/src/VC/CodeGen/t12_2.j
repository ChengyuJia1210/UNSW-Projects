Compiled from "t12.java"
public class t12 {
  public t12();
    Code:
       0: aload_0
       1: invokespecial #1                  // Method java/lang/Object."<init>":()V
       4: return

  int f();
    Code:
       0: iconst_3
       1: newarray       float
       3: dup
       4: iconst_0
       5: fconst_1
       6: fastore
       7: dup
       8: iconst_1
       9: fconst_2
      10: fastore
      11: dup
      12: iconst_2
      13: ldc           #2                  // float 3.0f
      15: fastore
      16: astore_1
      17: aload_1
      18: iconst_2
      19: ldc           #3                  // float 5.0f
      21: fastore
      22: iconst_5
      23: ireturn

  void h();
    Code:
       0: iconst_3
       1: newarray       boolean
       3: dup
       4: iconst_0
       5: iconst_0
       6: bastore
       7: dup
       8: iconst_1
       9: iconst_1
      10: bastore
      11: dup
      12: iconst_2
      13: iconst_0
      14: bastore
      15: astore_1
      16: return

  int g();
    Code:
       0: iconst_5
       1: newarray       boolean
       3: astore_1
       4: aload_1
       5: iconst_0
       6: iconst_0
       7: bastore
       8: bipush        10
      10: ireturn

  public static void main(java.lang.String[]);
    Code:
       0: return
}
