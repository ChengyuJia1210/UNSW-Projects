Compiled from "t4.j"
public class t4 {
  static {};
    Code:
       0: return

  public t4();
    Code:
       0: aload_0
       1: invokespecial #21                 // Method java/lang/Object."<init>":()V
       4: return

  int f();
    Code:
       0: iconst_0
       1: istore_1
       2: iload_1
       3: pop
       4: iload_1
       5: bipush        10
       7: if_icmplt     14
      10: iconst_0
      11: goto          15
      14: iconst_1
      15: ifeq          28
      18: iload_1
      19: i2f
      20: fstore_2
      21: iload_1
      22: iconst_1
      23: iadd
      24: istore_1
      25: goto          4
      28: iload_1
      29: ireturn
      30: nop

  float g();
    Code:
       0: iconst_0
       1: istore_1
       2: iconst_1
       3: istore_1
       4: iload_1
       5: bipush        10
       7: if_icmplt     14
      10: iconst_0
      11: goto          15
      14: iconst_1
      15: ifeq          28
      18: iload_1
      19: i2f
      20: fstore_2
      21: iload_1
      22: iconst_1
      23: iadd
      24: istore_1
      25: goto          4
      28: fload_2
      29: freturn
      30: nop

  public static void main(java.lang.String[]);
    Code:
       0: new           #9                  // class t4
       3: dup
       4: invokespecial #5                  // Method "<init>":()V
       7: astore_1
       8: aload_1
       9: invokevirtual #28                 // Method g:()F
      12: fstore_2
      13: return
}
