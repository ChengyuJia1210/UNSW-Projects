/**
 * Checker.java   
 * Mar 29 15:57:55 AEST 2019
 **/

package VC.Checker;

import VC.ASTs.*;
import VC.Scanner.SourcePosition;
import VC.ErrorReporter;
import VC.StdEnvironment;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

public final class Checker implements Visitor {

  private String errMesg[] = {
          "*0: main function is missing",
          "*1: return type of main is not int",

          // defined occurrences of identifiers
          // for global, local and parameters
          "*2: identifier redeclared",
          "*3: identifier declared void",
          "*4: identifier declared void[]",

          // applied occurrences of identifiers
          "*5: identifier undeclared",

          // assignments
          "*6: incompatible type for =",
          "*7: invalid lvalue in assignment",

          // types for expressions
          "*8: incompatible type for return",
          "*9: incompatible type for this binary operator",
          "*10: incompatible type for this unary operator",

          // scalars
          "*11: attempt to use an array/function as a scalar",

          // arrays
          "*12: attempt to use a scalar/function as an array",
          "*13: wrong type for element in array initialiser",
          "*14: invalid initialiser: array initialiser for scalar",
          "*15: invalid initialiser: scalar initialiser for array",
          "*16: excess elements in array initialiser",
          "*17: array subscript is not an integer",
          "*18: array size missing",

          // functions
          "*19: attempt to reference a scalar/array as a function",

          // conditional expressions in if, for and while
          "*20: if conditional is not boolean",
          "*21: for conditional is not boolean",
          "*22: while conditional is not boolean",

          // break and continue
          "*23: break must be in a while/for",
          "*24: continue must be in a while/for",

          // parameters
          "*25: too many actual parameters",
          "*26: too few actual parameters",
          "*27: wrong type for actual parameter",

          // reserved for errors that I may have missed (J. Xue)
          "*28: misc 1",
          "*29: misc 2",

          // the following two checks are optional
          "*30: statement(s) not reached",
          "*31: missing return statement",
  };


  private SymbolTable idTable;
  private static SourcePosition dummyPos = new SourcePosition();
  private ErrorReporter reporter;

  //----------------------------------------------
  private boolean hasMain = false;
  private boolean mainIsNotInt = false;
  private int loopLevel = 0;
  private int countArrayElements;
  //----------------------------------------------

  // Checks whether the source program, represented by its AST, 
  // satisfies the language's scope rules and type rules.
  // Also decorates the AST as follows:
  //  (1) Each applied occurrence of an identifier is linked to
  //      the corresponding declaration of that identifier.
  //  (2) Each expression and variable is decorated by its type.

  public Checker (ErrorReporter reporter) {
      this.reporter = reporter;
      this.idTable = new SymbolTable ();
      establishStdEnvironment();
  }

  public void check(AST ast) {
      ast.visit(this, null);
  }


// ====================================auxiliary methods=======================================
  private void declareVariable(Ident ident, Decl decl) {
      IdEntry entry = idTable.retrieveOneLevel(ident.spelling);

      if (entry == null) {
          ; // no problem
      } else
          reporter.reportError(errMesg[2] + ": %", ident.spelling, ident.position);
      idTable.insert(ident.spelling, decl);
  }

  private Object coercions(Expr expr){
      if (expr.type.equals(StdEnvironment.floatType))
          return expr;

      AST oldParent = expr.parent;
      Operator operator = new Operator("i2f", expr.position);
      Expr coercionsedExpr = new UnaryExpr(operator, expr, expr.position);
      coercionsedExpr.parent = oldParent;
      return coercionsedExpr;
  }

  private boolean logicalBinaryChecker(Operator operator){
      switch (operator.spelling){
      case "&&":
          return true;
      case "||":
          return true;
//      case "!":
//          return true;
      default:
          return false;
      }
  }

  private boolean booleanBinaryOperatorChecker(Operator operator){
      switch (operator.spelling){
      case "==":
          return true;
      case "!=":
          return true;
      default:
          return false;
      }
  }

  private boolean BinaryOperatorChecker(Operator operator){
      switch (operator.spelling){
      case "+":
          return true;
      case "-":
          return true;
      case "*":
          return true;
      case "/":
          return true;
      default:
          return false;
      }
  }

  private void declVoidChecker(Decl ast){
      if (ast.T instanceof ArrayType) {
          if(((ArrayType) ast.T).T.isVoidType()) {
              reporter.reportError(errMesg[4] + ": %", ast.I.spelling, ast.I.position);
          }
      }
      else {
          if (ast.T.isVoidType()){
              reporter.reportError(errMesg[3] + ": %", ast.I.spelling, ast.I.position);
          }
      }
  }

  // this only be used on the array declaration, so count element can be putted here
  public void arrayChecker(ArrayType arrayType, Expr expr, Decl decl){
      Type declType = arrayType.T;
      // ArrayType: -> T and E; int A[10] -> T is int, E is 10
      IntExpr bound = null;
      if (!(arrayType.E instanceof EmptyExpr)) {
          bound = (IntExpr) arrayType.E;
      }

      // int a[] = {1,2,3}; or float a[5] = {1.1, 2, 3,3, 4, 5};
      if (!(expr instanceof EmptyExpr)) {
          countArrayElements = 0;
          expr.visit(this, declType);
          if (!(expr instanceof InitExpr)){
              reporter.reportError(errMesg[15] + ": %", decl.I.spelling, decl.I.position);
          }

          if (bound != null) {
              int numElement = Integer.parseInt(bound.IL.spelling);
              if (numElement < countArrayElements) {
                  // "*16: excess elements in array initialiser"
                  reporter.reportError(errMesg[16] + ": %", decl.I.spelling, decl.position);
              }
          }
          else {
              IntLiteral IL = new IntLiteral(String.valueOf(countArrayElements), arrayType.E.position);
              IntExpr intExpr = new IntExpr(IL, (arrayType.E.position));
              ArrayType at = new ArrayType(arrayType.T, intExpr, arrayType.position);
              decl.T = at;
          }
      }
      // only int a[10];
      else {
          // if bound == null, then: "int[];" report error[18]
          if (bound == null) {
              reporter.reportError(errMesg[18] + ": %", decl.I.spelling, decl.I.position);
          }
      }
  }

// ========================================Programs=============================================

  public Object visitProgram(Program ast, Object o) {
      ast.FL.visit(this, null);

      if (!hasMain){
          reporter.reportError(errMesg[0], "", ast.position);
      }
      else{
          if (mainIsNotInt){
              reporter.reportError(errMesg[1], "", ast.position);
          }
      }

      return null;
  }

// ======================================Declarations===========================================
  // Always returns null. Does not use the given object.
  public Object visitFuncDecl(FuncDecl ast, Object o) {
//      idTable.insert (ast.I.spelling, ast);
      declareVariable(ast.I, ast);  // check if there is some function or variable already declared
      // Your code goes here
      if (ast.I.spelling.equals("main")) {
          hasMain = true;
          if (!ast.T.isIntType()){
              mainIsNotInt = true;
          }
      }
      // HINT
      // Pass ast as the 2nd argument (as done below) so that the
      // formal parameters of the function an be extracted from ast when the
      // function body is later visited
      ast.S.visit(this, ast);
      return null;
  }

  public Object visitDeclList(DeclList ast, Object o) {
      ast.D.visit(this, null);
      ast.DL.visit(this, null);
      return null;
  }

  public Object visitGlobalVarDecl(GlobalVarDecl ast, Object o) {
      declareVariable(ast.I, ast);

      declVoidChecker(ast);
      // arrayType
      if (ast.T instanceof ArrayType) {
          arrayChecker((ArrayType) ast.T, ast.E, ast);
      }
      else {
          // normal variable
          ast.E.visit(this, ast.T);
          if (ast.E instanceof InitExpr){
              reporter.reportError(errMesg[14], "", ast.E.position);
          }
          else {
              if (ast.T.assignable(ast.E.type) && !ast.T.equals(ast.E.type)){
                  ast.E = (Expr) coercions(ast.E);
              }
              else if (!ast.T.assignable(ast.E.type) && !(ast.E instanceof EmptyExpr)){
                  reporter.reportError(errMesg[6], "", ast.position);
              }
          }
      }
      return null;
  }

  public Object visitLocalVarDecl(LocalVarDecl ast, Object o) {
      declareVariable(ast.I, ast);

      // fill the rest
      declVoidChecker(ast);
      if (ast.T instanceof ArrayType) {
          arrayChecker((ArrayType) ast.T, ast.E, ast);
      }
      else {
          ast.E.visit(this, ast.T);
          if (ast.E instanceof InitExpr){
              reporter.reportError(errMesg[14], "", ast.E.position);
          }
          else {
              if (ast.T.assignable(ast.E.type) && !ast.T.equals(ast.E.type)){
                  ast.E = (Expr) coercions(ast.E);
              }
              else if (!ast.T.assignable(ast.E.type) && !(ast.E instanceof EmptyExpr)){
                  reporter.reportError(errMesg[6], "", ast.position);
              }
          }
      }

      return null;
  }

  public Object visitEmptyDeclList(EmptyDeclList ast, Object o) {
      return null;
  }

// ========================================Statements=========================================
  public Object visitCompoundStmt(CompoundStmt ast, Object o) {
      idTable.openScope(); // we only can define the function on the 1st level
      // Two situations
      // 1st, this is the 2nd, and this compoundStmt is the functionDecl compoundStmt
      if (o instanceof FuncDecl){
          FuncDecl funcDecl = (FuncDecl) o;
          // visit the parameter list, it doesn't need obj,
          // cause they don't need to compare to the parent type
          funcDecl.PL.visit(this, null);
          // inherit the function type to the function, it can be used to compare the return type
          ast.DL.visit(this, funcDecl.T);
          ast.SL.visit(this, funcDecl.T);
      }
      // 2nd, this is the normal compoundStmt, belong IfStmt, WhileStmt, etc.
      else {
          ast.DL.visit(this, o);
          ast.SL.visit(this, o);
      }

      idTable.closeScope();
      return null;
  }

  public Object visitStmtList(StmtList ast, Object o) {
      ast.S.visit(this, o);
      if (ast.S instanceof ReturnStmt && ast.SL instanceof StmtList)
          reporter.reportError(errMesg[30], "", ast.SL.position);
      ast.SL.visit(this, o);
      return null;
  }

  @Override
  public Object visitIfStmt(IfStmt ast, Object o) {
      ast.E.visit(this, o);
      if (!ast.E.type.isBooleanType()){
          reporter.reportError(errMesg[20] + " %",
                  "(found: "+ast.E.type.toString()+")", ast.E.position);
      }
      ast.S1.visit(this, o);
      ast.S2.visit(this, o);
      return null;
  }

  @Override
  public Object visitWhileStmt(WhileStmt ast, Object o) {
      ast.E.visit(this, o);
      if (!ast.E.type.isBooleanType()){
          reporter.reportError(errMesg[22] + " %",
                  "(found: "+ast.E.type.toString()+")", ast.E.position);
      }
      loopLevel++;
      ast.S.visit(this, o);
      loopLevel--;
      return null;
  }

  @Override
  public Object visitForStmt(ForStmt ast, Object o) {
      ast.E1.visit(this, o);
      ast.E2.visit(this, o);
      if (!ast.E2.type.isBooleanType() && !(ast.E2 instanceof EmptyExpr)){
          reporter.reportError(errMesg[21] + " %",
                  "(found: "+ast.E2.type.toString()+")", ast.E2.position);
      }
      ast.E3.visit(this, o);
      loopLevel++;
      ast.S.visit(this, o);
      loopLevel--;
      return null;
  }

  @Override
  public Object visitBreakStmt(BreakStmt ast, Object o) {
      if (loopLevel == 0){
          reporter.reportError(errMesg[23], "", ast.position);
      }
      return null;
  }

  @Override
  public Object visitContinueStmt(ContinueStmt ast, Object o) {
      if (loopLevel == 0){
          reporter.reportError(errMesg[24], "", ast.position);
      }
      return null;
  }


  @Override
  public Object visitReturnStmt(ReturnStmt ast, Object o) {
      // 1st need check the function type
      // 2nd does the returnStmt's Expr match or assignable to the function type
      ast.E.visit(this, o);
      Type functionType = (Type) o;
      ast.E.type = functionType;
      // function type is void, then return nothing
      if (functionType.isVoidType()){
          // if return something, then report err[8]
          if (!(ast.E.type.isVoidType())){
              reporter.reportError(errMesg[8], "", ast.position);
          }
      }
      // function needs to return something,
      // check returnStmt's Expr's type is equal or assignable to the function type
      else{
          // 1st, E's type is void, report err[31]: miss return stmt
          if (ast.E.type.isVoidType()) {
              // "*31: missing return statement"
              reporter.reportError(errMesg[31] + ": %", ast.E.type.toString(), ast.position);
          }
          // 2nd, func's type is int, E's type is float
          // 3rd, func's type is boolean, E's type is int or float or string
          // 4th, func's type is int or float, E's type is boolean or string
          // then, report err[8]
          else if(!functionType.assignable(ast.E.type)){
              reporter.reportError(errMesg[8], "", ast.position);
          }
          else if (!functionType.equals(ast.E.type) && functionType.assignable(ast.E.type)){
              ast.E = (Expr) coercions(ast.E); // attention
          }
      }
      return null;
  }

  @Override
  public Object visitEmptyExprList(EmptyExprList ast, Object o) {
      return null;
  }

  @Override
  public Object visitEmptyCompStmt(EmptyCompStmt ast, Object o) {
      return null;
  }

  // arrayType must be in the declaration part
  // it may be initialised, if initialised then the E could be empty, otherwise must have E
  @Override
  public Object visitArrayType(ArrayType ast, Object o) {
      ast.T.visit(this, o);
      ast.E.visit(this, o);
      return ast.T;
  }

  @Override
  public Object visitSimpleVar(SimpleVar ast, Object o) {
      ast.type = (Type) ast.I.visit(this, o);
      return ast.type;
  }

  public Object visitExprStmt(ExprStmt ast, Object o) {
      ast.E.visit(this, o);
      return null;
  }

  public Object visitEmptyStmt(EmptyStmt ast, Object o) {
      return null;
  }

    public Object visitEmptyStmtList(EmptyStmtList ast, Object o) {
        return null;
    }

// ========================================Expressions===========================================
  // Returns the Type denoting the type of the expression. Does
  // not use the given object.
  @Override
  public Object visitUnaryExpr(UnaryExpr ast, Object o) {
      ast.E.type = (Type) ast.E.visit(this, o);
      if (ast.O.spelling.equals("!") && !ast.E.type.isBooleanType()){
              // "*10: incompatible type for this unary operator"
              reporter.reportError(errMesg[10] + ": %", ast.O.spelling, ast.position);
          ast.E.type = StdEnvironment.errorType;
      }
      else if ((ast.O.spelling.equals("+") || ast.O.spelling.equals("-")) // attention, 11/April 15:40pm
              && (!ast.E.type.isFloatType() && !ast.E.type.isIntType())){
              // "*10: incompatible type for this unary operator"
              reporter.reportError(errMesg[10] + ": %", ast.O.spelling, ast.position);
          ast.E.type = StdEnvironment.errorType;
      }

      if (ast.E.type.isFloatType())
        ast.O = new Operator("f"+ast.O.spelling, ast.O.position);
      else
        ast.O = new Operator("i"+ast.O.spelling, ast.O.position);


      return ast.E.type;
  }

  @Override
  public Object visitBinaryExpr(BinaryExpr ast, Object o) {
      Operator operator = ast.O;
      ast.E1.type = (Type) ast.E1.visit(this, o);
      ast.E2.type = (Type) ast.E2.visit(this, o);
      // if operator is logiacl operator, then the E1.type and E2.type should be boolean
      // logicalBinaryChecker(operator): ||, &&
      if (logicalBinaryChecker(operator)){
          if (!ast.E1.type.isBooleanType() || !ast.E2.type.isBooleanType()){
              reporter.reportError(errMesg[9] + ": %", ast.O.spelling, ast.position);
              ast.type = StdEnvironment.errorType;
              return ast.type;
          }
          ast.O = new Operator("i"+operator.spelling, operator.position);
          ast.type = StdEnvironment.booleanType;
          return ast.type;
      }
      // the operator should be "+, -, *, /" | "==, !=, >, >=, <, <="
      // BinaryOperatorChecker(operator): +, -, *, /   Only suitable for the number type
      // booleanBinaryOperatorChecker(operator): !=, == Only suitable for the boolean type
      else{
          if (ast.E1.type.isIntType() && ast.E2.type.isFloatType()){
              ast.E1 = (Expr) coercions(ast.E1);
              ast.O = new Operator("f"+operator.spelling, operator.position);
              if (BinaryOperatorChecker(operator)){
                  ast.type = StdEnvironment.floatType;
                  return ast.type;
              }else{
                  ast.type = StdEnvironment.booleanType;
                  return ast.type;
              }
          }
          else if (ast.E1.type.isFloatType() && ast.E2.type.isIntType()){
              ast.E2 = (Expr) coercions(ast.E2);
              ast.O = new Operator("f"+operator.spelling, operator.position);
              if (BinaryOperatorChecker(operator)){
                  ast.type = StdEnvironment.floatType;
                  return ast.type;
              }else{
                  ast.type = StdEnvironment.booleanType;
                  return ast.type;
              }          }
          else if (ast.E1.type.isIntType() && ast.E2.type.isIntType()){
              ast.O = new Operator("i"+operator.spelling, operator.position);
              if (BinaryOperatorChecker(operator)){
                  ast.type = StdEnvironment.intType;
                  return ast.type;
              }else{
                  ast.type = StdEnvironment.booleanType;
                  return ast.type;
              }
          }
          else if (ast.E1.type.isFloatType() && ast.E2.type.isFloatType()){
              ast.O = new Operator("f"+operator.spelling, operator.position);
              if (BinaryOperatorChecker(operator)){
                  ast.type = StdEnvironment.floatType;
                  return ast.type;
              }else{
                  ast.type = StdEnvironment.booleanType;
                  return ast.type;
              }
          }
          else if (ast.E1.type.isBooleanType() && ast.E2.type.isBooleanType()){
              if (booleanBinaryOperatorChecker(operator)){
                  ast.O = new Operator("i"+operator.spelling, operator.position);
                  ast.type = StdEnvironment.booleanType;
                  return ast.type;
              }
              else {
                  reporter.reportError(errMesg[9] + ": %", ast.O.spelling, ast.position);
                  ast.type = StdEnvironment.errorType;
                  return ast.type;
              }
          }
          else {
              reporter.reportError(errMesg[9] + ": %", ast.O.spelling, ast.position);
              ast.type = StdEnvironment.errorType;
              return ast.type;
          }
      }
  }

  @Override
  public Object visitInitExpr(InitExpr ast, Object o) {
//      countArrayElements = 0;
      Type type = (Type) ast.IL.visit(this, o);
      return type;
  }

  @Override
  public Object visitExprList(ExprList ast, Object o) {
      Type elementType = (Type)ast.E.visit(this, o);
      Type arrayDeclarationType = (Type) o;
      if (!arrayDeclarationType.assignable(elementType)){
          // "*13: wrong type for element in array initialiser"
          reporter.reportError(errMesg[13] + ": %",
                  "at position " + countArrayElements, ast.E.position);
          ast.E.type = StdEnvironment.errorType;
      }
      else if (arrayDeclarationType.assignable(elementType) && !arrayDeclarationType.equals(elementType)){
          ast.E = (Expr) coercions(ast.E);
      }

      if (!(ast.E instanceof EmptyExpr)){
          countArrayElements++;
      }
      ast.EL.visit(this, o);
      return arrayDeclarationType;
  }

  @Override
  public Object visitArrayExpr(ArrayExpr ast, Object o) {
      SimpleVar simpleVar = (SimpleVar) ast.V;
      Decl decl = idTable.retrieve(simpleVar.I.spelling);
      ((SimpleVar) ast.V).I.decl = decl;
      ast.type = decl.T;
      if (decl == null){
          reporter.reportError(errMesg[5] + ": %", simpleVar.I.spelling, simpleVar.I.position);
          return StdEnvironment.errorType;
      }

      if (!decl.T.isArrayType()){
          // "*12: attempt to use a scalar/function as an array",
          reporter.reportError(errMesg[12], "", ast.position);
          ast.type =  StdEnvironment.errorType;
//          return StdEnvironment.errorType;
      }
      ast.E.visit(this, o);
      if (!ast.E.type.isIntType()){
          // "*17: array subscript is not an integer",
          reporter.reportError(errMesg[17], "", ast.position);
          ast.type =  StdEnvironment.errorType;
//          return StdEnvironment.errorType;
      }
      // if ast.type already assign the error type, then return
      if (ast.type != null){
          return ast.type;
      }
      else {
          ast.type = ((ArrayType) decl.T).T;
          return ast.type;
      }
  }

  @Override
  public Object visitCallExpr(CallExpr ast, Object o) {
//      Type type = (Type) ast.I.visit(this, o); // would clash with the assign variable
      Decl decl = idTable.retrieve(ast.I.spelling);
      if (decl != null) {
          ast.I.decl = decl;
          Type type = decl.T;
          ast.type = type;
          if (ast.I.decl instanceof FuncDecl) {
              FuncDecl funcDecl = (FuncDecl) ast.I.decl;
              ast.AL.visit(this, funcDecl.PL);
              return type;
          } else {
              reporter.reportError(errMesg[19] + ": %", ast.I.spelling, ast.I.position);
              return StdEnvironment.errorType;
          }
      }
      else {
          reporter.reportError(errMesg[5] + ": %", ast.I.spelling, ast.position);
          ast.type = StdEnvironment.errorType;
          return StdEnvironment.errorType;
      }
  }

  @Override
  public Object visitAssignExpr(AssignExpr ast, Object o) {
      Type type1 = (Type) ast.E1.visit(this, o);
      Type type2 = (Type) ast.E2.visit(this, o);
      ast.E2.type = type2;
      // 1st, t1 bool, t2 bool -> correct
      // 2nd, t1 bool, t2 int or float or string -> error
      // 3rd, t1 int, t2 int  -> correct
      // 4th, t1 float, t2 int or float -> correct(coercion)
      // 5th, t1 int, t2 float -> error

      // need to check left Expr is Var or
      // Array(array can not be assigned, except declaration part)
      if (ast.E1 instanceof VarExpr){
          VarExpr varExpr = (VarExpr) ast.E1;
          SimpleVar simpleVar = (SimpleVar) varExpr.V;
          Ident ident = simpleVar.I;
          Decl decl = idTable.retrieve(ident.spelling);
          if ((!(decl instanceof LocalVarDecl) && !(decl instanceof GlobalVarDecl) && !(decl instanceof ParaDecl))
                  || decl.T instanceof ArrayType){
              // "*7: invalid lvalue in assignment"
              reporter.reportError(errMesg[7] + ": %", simpleVar.I.spelling, ast.position);
              ast.type = StdEnvironment.errorType;
              return ast.type;
          }
          else {
              if (!type1.equals(type2) && type1.assignable(type2)) { // 4th
                  ast.E2 = (Expr) coercions(ast.E2);
              }
              else if (!type1.assignable(type2)) {// 2nd, 5th
                  reporter.reportError(errMesg[6], "", ast.position);
                  ast.type = StdEnvironment.errorType;
                  return ast.type;
              }
          }
      }
      else{
          // "*7: invalid lvalue in assignment"
          reporter.reportError(errMesg[7], "", ast.position);
          ast.type = StdEnvironment.errorType;
          return ast.type;
      }
      ast.type = type1;
      return type1;
  }

  public Object visitEmptyExpr(EmptyExpr ast, Object o) {
      ast.type = StdEnvironment.voidType;
//      ast.type = StdEnvironment.errorType;
      return ast.type;
  }

  public Object visitBooleanExpr(BooleanExpr ast, Object o) {
      ast.type = StdEnvironment.booleanType;
      return ast.type;
  }

  public Object visitIntExpr(IntExpr ast, Object o) {
      ast.type = StdEnvironment.intType;
      return ast.type;
  }

  public Object visitFloatExpr(FloatExpr ast, Object o) {
      ast.type = StdEnvironment.floatType;
      return ast.type;
  }

  public Object visitStringExpr(StringExpr ast, Object o) {
      ast.type = StdEnvironment.stringType;
      return ast.type;
  }

  public Object visitVarExpr(VarExpr ast, Object o) {
      ast.type = (Type) ast.V.visit(this, null);
      return ast.type;
  }


// ======================================Parameters=======================================

 // Always returns null. Does not use the given object.

  public Object visitParaList(ParaList ast, Object o) {
      ast.P.visit(this, null);
      if ((ast.P instanceof ParaDecl)){
      }
      ast.PL.visit(this, null);
      return null;
  }

  public Object visitParaDecl(ParaDecl ast, Object o) {
      declareVariable(ast.I, ast);
      declVoidChecker(ast);
      return null;
  }

  public Object visitEmptyParaList(EmptyParaList ast, Object o) {
      return null;
  }

// =====================================Arguments=============================================

  // Your visitor methods for arguments go here
  @Override
  public Object visitArgList(ArgList ast, Object o) {
      ParaList paraList = (ParaList) o;
      ast.A.visit(this, paraList.P);
      if (!(ast.AL instanceof EmptyArgList) && (paraList.PL instanceof EmptyParaList)){
          reporter.reportError(errMesg[25], "", ast.AL.position);
          return StdEnvironment.errorType;
      }
      ast.AL.visit(this, paraList.PL);

      return null;
  }

  @Override
  public Object visitArg(Arg ast, Object o) {
      if (ast.E instanceof VarExpr){
          VarExpr varExpr = (VarExpr) ast.E;
          SimpleVar simpleVar = (SimpleVar) varExpr.V;
          simpleVar.I.decl = idTable.retrieve(simpleVar.I.spelling);
          ((SimpleVar)((VarExpr) ast.E).V).I.decl = simpleVar.I.decl;
          if (simpleVar.I.decl == null){
              reporter.reportError(errMesg[5] + ": %", simpleVar.I.spelling, simpleVar.I.position);
              return StdEnvironment.errorType;
          }
          ast.E.type = ((Decl) simpleVar.I.decl).T;
      }
      else {
          ast.E.visit(this, o);
      }
      ParaDecl paraDecl = (ParaDecl) o;
      if (!paraDecl.T.assignable(ast.E.type)){
          if (ast.E.type.isArrayType() && paraDecl.T.isArrayType()){
              ArrayType argArrayType = (ArrayType) ast.E.type;
              ArrayType paraDeclArrayType = (ArrayType) paraDecl.T;
              if (paraDeclArrayType.T.assignable(argArrayType.T)
                      && !paraDeclArrayType.T.equals(argArrayType.T)){
                  ast.E = (Expr) coercions(ast.E);
                  return paraDeclArrayType.T;
              }
              else if (paraDeclArrayType.T.equals(argArrayType.T)) {
                  return paraDeclArrayType.T;
              }
          }
          // "*27: wrong type for actual parameter",
          reporter.reportError(errMesg[27] + ": %", paraDecl.I.spelling, ast.position);
          ast.E.type = StdEnvironment.errorType;
      }
      else if (paraDecl.T.assignable(ast.E.type) && !(paraDecl.T.equals(ast.E.type))){
          ast.E = (Expr) coercions(ast.E);
      }
      return ast.E.type;
  }

//  private boolean compareTwoArrayType

  @Override
  public Object visitEmptyArgList(EmptyArgList ast, Object o) {
      if (o instanceof ParaList){
          reporter.reportError(errMesg[26], "", ast.parent.position);
      }
      return null;
  }

// =======================================Types===============================================
  // Returns the type predefined in the standard environment. 

  public Object visitErrorType(ErrorType ast, Object o) {
      return StdEnvironment.errorType;
  }

  public Object visitBooleanType(BooleanType ast, Object o) {
      return StdEnvironment.booleanType;
  }

  public Object visitIntType(IntType ast, Object o) {
      return StdEnvironment.intType;
  }

  public Object visitFloatType(FloatType ast, Object o) {
      return StdEnvironment.floatType;
  }

  public Object visitStringType(StringType ast, Object o) {
      return StdEnvironment.stringType;
  }

  public Object visitVoidType(VoidType ast, Object o) {
      return StdEnvironment.voidType;
  }

// ===============================Literals, Identifiers and Operators================================

  public Object visitIdent(Ident I, Object o) {
      Decl binding = idTable.retrieve(I.spelling);
      if (binding != null) {
          I.decl = binding;
          // attention 13_April_11:00
          if ((binding instanceof FuncDecl) || (binding.T instanceof ArrayType)){
              // "*11: attempt to use an array/fuction as a scalar"
              reporter.reportError(errMesg[11] + ": %", I.spelling, I.position);
              return StdEnvironment.errorType;
          }

          return binding.T;
      }
      else{
          reporter.reportError(errMesg[5] + ": %", I.spelling, I.position);
//          I.decl = StdEnvironment.errorType;
          return StdEnvironment.errorType;
      }
  }

  public Object visitBooleanLiteral(BooleanLiteral SL, Object o) {
      return StdEnvironment.booleanType;
  }

  public Object visitIntLiteral(IntLiteral IL, Object o) {
      return StdEnvironment.intType;
  }

  public Object visitFloatLiteral(FloatLiteral IL, Object o) {
      return StdEnvironment.floatType;
  }

  public Object visitStringLiteral(StringLiteral IL, Object o) {
      return StdEnvironment.stringType;
  }

  public Object visitOperator(Operator O, Object o) {
      return null;
  }


// ===================================Build-In Function==========================================
  // Creates a small AST to represent the "declaration" of each built-in
  // function, and enters it in the symbol table.

  private FuncDecl declareStdFunc (Type resultType, String id, List pl) {

      FuncDecl binding;

      binding = new FuncDecl(resultType, new Ident(id, dummyPos), pl,
              new EmptyStmt(dummyPos), dummyPos);
      idTable.insert (id, binding);
      return binding;
  }

  // Creates small ASTs to represent "declarations" of all
  // build-in functions.
  // Inserts these "declarations" into the symbol table.
  private final static Ident dummyI = new Ident("x", dummyPos);

  private void establishStdEnvironment () {

      // Define four primitive types
      // errorType is assigned to ill-typed expressions

      StdEnvironment.booleanType = new BooleanType(dummyPos);
      StdEnvironment.intType = new IntType(dummyPos);
      StdEnvironment.floatType = new FloatType(dummyPos);
      StdEnvironment.stringType = new StringType(dummyPos);
      StdEnvironment.voidType = new VoidType(dummyPos);
      StdEnvironment.errorType = new ErrorType(dummyPos);

      // enter into the declarations for built-in functions into the table
      StdEnvironment.getIntDecl = declareStdFunc( StdEnvironment.intType,
              "getInt", new EmptyParaList(dummyPos));
      StdEnvironment.putIntDecl = declareStdFunc( StdEnvironment.voidType,
              "putInt", new ParaList(
                      new ParaDecl(StdEnvironment.intType, dummyI, dummyPos),
                      new EmptyParaList(dummyPos), dummyPos));
      StdEnvironment.putIntLnDecl = declareStdFunc( StdEnvironment.voidType,
              "putIntLn", new ParaList(
                      new ParaDecl(StdEnvironment.intType, dummyI, dummyPos),
                      new EmptyParaList(dummyPos), dummyPos));
      StdEnvironment.getFloatDecl = declareStdFunc( StdEnvironment.floatType,
              "getFloat", new EmptyParaList(dummyPos));
      StdEnvironment.putFloatDecl = declareStdFunc( StdEnvironment.voidType,
              "putFloat", new ParaList(
                      new ParaDecl(StdEnvironment.floatType, dummyI, dummyPos),
                      new EmptyParaList(dummyPos), dummyPos));
      StdEnvironment.putFloatLnDecl = declareStdFunc( StdEnvironment.voidType,
              "putFloatLn", new ParaList(
                      new ParaDecl(StdEnvironment.floatType, dummyI, dummyPos),
                      new EmptyParaList(dummyPos), dummyPos));
      StdEnvironment.putBoolDecl = declareStdFunc( StdEnvironment.voidType,
              "putBool", new ParaList(
                      new ParaDecl(StdEnvironment.booleanType, dummyI, dummyPos),
                      new EmptyParaList(dummyPos), dummyPos));
      StdEnvironment.putBoolLnDecl = declareStdFunc( StdEnvironment.voidType,
              "putBoolLn", new ParaList(
                      new ParaDecl(StdEnvironment.booleanType, dummyI, dummyPos),
                      new EmptyParaList(dummyPos), dummyPos));

      StdEnvironment.putStringLnDecl = declareStdFunc( StdEnvironment.voidType,
              "putStringLn", new ParaList(
                      new ParaDecl(StdEnvironment.stringType, dummyI, dummyPos),
                      new EmptyParaList(dummyPos), dummyPos));

      StdEnvironment.putStringDecl = declareStdFunc( StdEnvironment.voidType,
              "putString", new ParaList(
                      new ParaDecl(StdEnvironment.stringType, dummyI, dummyPos),
                      new EmptyParaList(dummyPos), dummyPos));

      StdEnvironment.putLnDecl = declareStdFunc( StdEnvironment.voidType,
              "putLn", new EmptyParaList(dummyPos));

  }
}