Compiled from "t10.java"
public class t10 {
  public t10();
    Code:
       0: aload_0
       1: invokespecial #1                  // Method java/lang/Object."<init>":()V
       4: return

  int f();
    Code:
       0: iconst_1
       1: istore_1
       2: iconst_1
       3: istore_2
       4: bipush        10
       6: istore_3
       7: iload_1
       8: ifne          15
      11: iload_2
      12: ifeq          20
      15: iconst_1
      16: istore_1
      17: goto          23
      20: bipush        11
      22: istore_3
      23: iload_3
      24: ireturn

  int g();
    Code:
       0: iconst_1
       1: istore_1
       2: iload_1
       3: ifne          10
       6: iconst_1
       7: goto          11
      10: iconst_0
      11: istore_1
      12: bipush        10
      14: ireturn

  public static void main(java.lang.String[]);
    Code:
       0: return
}
