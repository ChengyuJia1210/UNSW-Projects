/*
 * FuncDecl.java      
 */

package VC.ASTs;

import VC.Scanner.SourcePosition;

public class FuncDecl extends Decl {

  public List PL; // function parameter list
  public Stmt S;  // function statement

  public AST entryNode;

  public FuncDecl(Type tAST, Ident idAST, List fplAST, 
         Stmt cAST, SourcePosition Position) {
    super (Position);
    T = tAST;   // function return type
    I = idAST;  // function identifier
    PL = fplAST;
    S = cAST;
    T.parent = I.parent = PL.parent = S.parent = this;
  }

  public Object visit (Visitor v, Object o) {
    return v.visitFuncDecl(this, o);
  }

}
