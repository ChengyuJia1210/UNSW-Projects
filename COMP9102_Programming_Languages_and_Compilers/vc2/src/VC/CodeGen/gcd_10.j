Compiled from "gcd.j"
public class gcd {
  static int i;

  static int j;

  static {};
    Code:
       0: iconst_0
       1: putstatic     #16                 // Field i:I
       4: iconst_0
       5: putstatic     #19                 // Field j:I
       8: return

  public gcd();
    Code:
       0: aload_0
       1: invokespecial #35                 // Method java/lang/Object."<init>":()V
       4: return

  int gcd(int, int);
    Code:
       0: iload_2
       1: iconst_0
       2: if_icmpeq     9
       5: iconst_0
       6: goto          10
       9: iconst_1
      10: ifeq          18
      13: iload_1
      14: ireturn
      15: goto          31
      18: aload_0
      19: iload_2
      20: iload_1
      21: iload_1
      22: iload_2
      23: idiv
      24: iload_2
      25: imul
      26: isub
      27: invokevirtual #36                 // Method gcd:(II)I
      30: ireturn
      31: nop

  public static void main(java.lang.String[]);
    Code:
       0: new           #12                 // class gcd
       3: dup
       4: invokespecial #37                 // Method "<init>":()V
       7: astore_1
       8: invokestatic  #10                 // Method VC/lang/System.getInt:()I
      11: putstatic     #16                 // Field i:I
      14: invokestatic  #10                 // Method VC/lang/System.getInt:()I
      17: putstatic     #19                 // Field j:I
      20: aload_1
      21: getstatic     #16                 // Field i:I
      24: getstatic     #19                 // Field j:I
      27: invokevirtual #36                 // Method gcd:(II)I
      30: invokestatic  #27                 // Method VC/lang/System.putIntLn:(I)V
      33: return
}
