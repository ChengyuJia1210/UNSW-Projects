/*
 * ArrayExpr.java       
 */
// this class is used to do the array expressing, not for definition
// but Var V still keep the type


package VC.ASTs;

import VC.Scanner.SourcePosition;

public class ArrayExpr extends Expr {

  public Var V;
  public Expr E; // index of array var

  public ArrayExpr (Var idAST, Expr indexAST, SourcePosition position) {
    super (position);
    V = idAST;
    E = indexAST;
    V.parent = E.parent = this;
  }

  public Object visit(Visitor v, Object o) {
    return v.visitArrayExpr(this, o);
  }

}
