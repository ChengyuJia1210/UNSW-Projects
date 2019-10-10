/*
 * ArrayType.java               
 */
// this clss is used in the array declaration
// and the identifier only be kept in the variable declaration

package VC.ASTs;

import VC.Scanner.SourcePosition;

public class ArrayType extends Type {

  public Type T;    // type of this array
  public Expr E;    // the scale of the array, et. a[5], Expr.5 is E

  public ArrayType (Type tAST, Expr dAST, SourcePosition Position) {
    super (Position);
    T = tAST;
    E = dAST;
    T.parent = E.parent = this;
  }

  public Object visit (Visitor v, Object o) {
    return v.visitArrayType(this, o);
  }

  public boolean equals(Object obj) {
    return false; // An array name cannot appear alone in VC.
                  // For example, a = b is not allowed if a and b are
                  // array names. Thus, this method should never be called
  }

  public boolean assignable(Object obj) {
    return false; // a = b is not a legal assignment in VC when a and b are arrays
                  // But an array can be passed as an argument in a function call. So 
                  // you need to handle this as a special case without relying on this method!
  }

  public String toString() {
    if (T instanceof IntType)
      return "[I";
    else if (T instanceof FloatType)
      return "[F";
    else if (T instanceof BooleanType)
      return "[B";
    else if (T instanceof VoidType)
      return "[V";   // will be detected as semantic errors
    else  // should not happen
      return "[*";
  }

}
