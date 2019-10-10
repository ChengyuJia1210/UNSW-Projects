/*
 * src.VC.Parser.java
 *
 * This parser for a subset of the VC language is intended to 
 *  demonstrate how to create the AST nodes, including (among others): 
 *  [1] a list (of statements)
 *  [2] a function
 *  [3] a statement (which is an expression statement), 
 *  [4] a unary expression
 *  [5] a binary expression
 *  [6] terminals (identifiers, integer literals and operators)
 *
 * In addition, it also demonstrates how to use the two methods start 
 * and finish to determine the position information for the start and 
 * end of a construct (known as a phrase) corresponding an AST node.
 *
 * NOTE THAT THE POSITION INFORMATION WILL NOT BE MARKED. HOWEVER, IT CAN BE
 * USEFUL TO DEBUG YOUR IMPLEMENTATION.
 *
 * --- 12-March-2019 --- 


program       -> func-decl
func-decl     -> type identifier "(" ")" compound-stmt
type          -> void
identifier    -> ID
// statements
compound-stmt -> "{" stmt* "}" 
stmt          -> expr-stmt
expr-stmt     -> expr? ";"
// expressions 
expr                -> additive-expr
additive-expr       -> multiplicative-expr
                    |  additive-expr "+" multiplicative-expr
                    |  additive-expr "-" multiplicative-expr
multiplicative-expr -> unary-expr
	            |  multiplicative-expr "*" unary-expr
	            |  multiplicative-expr "/" unary-expr
unary-expr          -> "-" unary-expr
		    |  primary-expr

primary-expr        -> identifier
 		    |  INTLITERAL
		    | "(" expr ")"
 */

package VC.Parser;

import VC.Scanner.Scanner;
import VC.Scanner.SourcePosition;
import VC.Scanner.Token;
import VC.ErrorReporter;
import VC.ASTs.*;
//import com.sun.org.apache.xpath.internal.operations.And;

public class Parser {

  private Scanner scanner;
  private ErrorReporter errorReporter;
  private Token currentToken;
  private SourcePosition previousTokenPosition;
  private SourcePosition dummyPos = new SourcePosition();

  private int[] types = {Token.INT, Token.FLOAT, Token.BOOLEAN, Token.VOID};

  public Parser (Scanner lexer, ErrorReporter reporter) {
    scanner = lexer;
    errorReporter = reporter;

    previousTokenPosition = new SourcePosition();

    currentToken = scanner.getToken();
  }

// match checks to see f the current token matches tokenExpected.
// If so, fetches the next token.
// If not, reports a syntactic error.

  void match(int tokenExpected) throws SyntaxError {
    if (currentToken.kind == tokenExpected) {
      previousTokenPosition = currentToken.position;
      currentToken = scanner.getToken();
    } else {
      syntacticError("\"%\" expected here", Token.spell(tokenExpected));
    }
  }

  void accept() {
    previousTokenPosition = currentToken.position;
    currentToken = scanner.getToken();
  }

  void syntacticError(String messageTemplate, String tokenQuoted) throws SyntaxError {
    SourcePosition pos = currentToken.position;
    errorReporter.reportError(messageTemplate, tokenQuoted, pos);
    throw(new SyntaxError());
  }

// start records the position of the start of a phrase.
// This is defined to be the position of the first
// character of the first token of the phrase.

  void start(SourcePosition position) {
    position.lineStart = currentToken.position.lineStart;
    position.charStart = currentToken.position.charStart;
  }

// finish records the position of the end of a phrase.
// This is defined to be the position of the last
// character of the last token of the phrase.

  void finish(SourcePosition position) {
    position.lineFinish = previousTokenPosition.lineFinish;
    position.charFinish = previousTokenPosition.charFinish;
  }

  void copyStart(SourcePosition from, SourcePosition to) {
    to.lineStart = from.lineStart;
    to.charStart = from.charStart;
  }

// ========================== PROGRAMS ========================
  public Program parseProgram() {
    Program programAST = null;
    List dlAST = new EmptyDeclList(dummyPos);
    SourcePosition programPos = new SourcePosition();
    start(programPos);

    try {
      if (typeChecker(currentToken.kind)) {
          Type tAST = parseType();
          dlAST = parseFuncAndVarFactoring(tAST);
      }
      finish(programPos);
      programAST = new Program(dlAST, programPos);
      if (currentToken.kind != Token.EOF) {
        syntacticError("\"%\" unknown type", currentToken.spelling);
      }
    }
    catch (SyntaxError s) { return null; }
    return programAST;
  }

// ========================== DECLARATIONS ========================
  List parseFuncAndVarFactoring(Type tAST) throws SyntaxError {
    List lAST = null; //for store the recursive "parseFuncAndVarFactoring" List value
    List l2AST = null; //for "DeclList" List value
    Ident iAST = parseIdent();

    Decl dAST = null;
    SourcePosition funcPos = new SourcePosition();
    start(funcPos);
    // the function part
    if (currentToken.kind == Token.LPAREN){
        dAST = parseFuncDecl(tAST, iAST);
        if (currentToken.kind != Token.EOF) {
            Type ts2AST = parseType();
            lAST = parseFuncAndVarFactoring(ts2AST);
        }
        finish(funcPos);
    }
    // the global variable part
    else{
        if (currentToken.kind != Token.EOF) {
            dAST = parseGlobaleVarDecl(tAST, iAST);
            if (currentToken.kind == Token.COMMA){
                match(Token.COMMA);
                lAST = parseFuncAndVarFactoring(tAST);
            }
            else if (currentToken.kind == Token.SEMICOLON){
                match(Token.SEMICOLON);
                if (currentToken.kind != Token.EOF) {
                    Type tsAST = parseType();
                    lAST = parseFuncAndVarFactoring(tsAST);
                }
            }
            finish(funcPos);
        }
    }

    // if finish one round, then the current Token is type, start new round
    if (dAST != null && lAST != null){
        l2AST = new DeclList(dAST, lAST, funcPos);
    }
    else if (dAST != null) {
        l2AST = new DeclList(dAST, new EmptyDeclList(dummyPos), funcPos);
    }
    if (dAST == null) {
        l2AST = new EmptyDeclList(dummyPos);
    }

    return l2AST;
  }


  Decl parseFuncDecl(Type tAST, Ident iAST) throws SyntaxError {
    Decl fAST = null;
    
    SourcePosition funcPos = new SourcePosition();
    start(funcPos);

    List fplAST = parseParaList();
    Stmt cAST = parseCompoundStmt();
    finish(funcPos);
    fAST = new FuncDecl(tAST, iAST, fplAST, cAST, funcPos);

    return fAST;
  }

//  ======================== TYPES ==========================
  Type parseType() throws SyntaxError {
    Token privousToken = currentToken;
    Type typeAST = null;

    SourcePosition typePos = new SourcePosition();
    start(typePos);
    accept();
    finish(typePos);
    // modified
    switch (privousToken.kind) {
    case Token.VOID:
        typeAST = new VoidType(typePos);
        break;
    case Token.INT:
        typeAST = new IntType(typePos);
        break;
    case Token.FLOAT:
        typeAST = new FloatType(typePos);
        break;
    case Token.BOOLEAN:
        typeAST = new BooleanType(typePos);
        break;
    default:
        syntacticError("illegal type!", "");
        break;
    }

    return typeAST;
    }

// ======================= STATEMENTS ==============================
  Stmt parseCompoundStmt() throws SyntaxError {
    Stmt cAST = null;
    List locaVarList = new EmptyDeclList(dummyPos);
    SourcePosition stmtPos = new SourcePosition();
    start(stmtPos);

    match(Token.LCURLY);
    if (currentToken.kind != Token.RCURLY) {
        // Insert code here to build a DeclList node for variable declarations, need add parseVarDeclList()
        if (typeChecker(currentToken.kind)) {
            Type tAST = parseType();
            locaVarList = parseLocalVarDeclList(tAST);
        }

        List slAST = parseStmtList();
        match(Token.RCURLY);
        finish(stmtPos);
        cAST = new CompoundStmt(locaVarList, slAST, stmtPos);
    }
    else{
        match(Token.RCURLY);
        finish(stmtPos);
        cAST = new EmptyCompStmt(stmtPos);
    }

    return cAST;
  }


  List parseStmtList() throws SyntaxError {
    List slAST = null;

    SourcePosition stmtPos = new SourcePosition();
    start(stmtPos);

    if (currentToken.kind != Token.RCURLY) {
        Stmt sAST = parseStmt();
      {
        if (currentToken.kind != Token.RCURLY) {
          slAST = parseStmtList();
          finish(stmtPos);
          slAST = new StmtList(sAST, slAST, stmtPos);
        } else {
          finish(stmtPos);
          slAST = new StmtList(sAST, new EmptyStmtList(dummyPos), stmtPos);
        }
      }
    }
    else
      slAST = new EmptyStmtList(dummyPos);
    
    return slAST;
  }

  Stmt parseStmt() throws SyntaxError {
    Stmt sAST = null;
    switch (currentToken.kind){
      case Token.IF:
          sAST = parseIfStmt();
          break;
      case Token.FOR:
          sAST = parseForStmt();
          break;
      case Token.WHILE:
          sAST = parseWhileStmt();
          break;
      case Token.BREAK:
          sAST = parseBreakStmt();
          break;
      case Token.CONTINUE:
          sAST = parseContinueStmt();
          break;
      case Token.RETURN:
          sAST = parseReturnStmt();
          break;
      case Token.LCURLY:
          sAST = parseCompoundStmt();
          break;
      default:
          sAST = parseExprStmt();
          break;
      }
    return sAST;
  }

  // -------------------- IF, WHILE, FOR, CONTINUE, BREAK, RETURN------------------------
  Stmt parseIfStmt() throws SyntaxError {
    Expr eAST = null;
    Stmt sAST1 = null;
    Stmt sAST2 = null;
    SourcePosition ifStmtPos = new SourcePosition();
    start(ifStmtPos);

    match(Token.IF); // accept Token "IF"
    match(Token.LPAREN);
    eAST = parseExpr();
    match(Token.RPAREN);
    sAST1 = parseStmt();
    if (currentToken.kind == Token.ELSE){
        match(Token.ELSE);
        sAST2 = parseStmt();
        finish(ifStmtPos);
        return new IfStmt(eAST, sAST1, sAST2, ifStmtPos);
    }
    else {
        finish(ifStmtPos);
        return new IfStmt(eAST, sAST1, ifStmtPos);
    }

  }

  // for-stmt -> for "(" expr? ";" expr? ";" expr? ")" stmt
  Stmt parseForStmt() throws SyntaxError {
      Expr eAST1 = null;
      Expr eAST2 = null;
      Expr eAST3 = null;
      Stmt sAST = null;
      SourcePosition forStmtPos = new SourcePosition();
      start(forStmtPos);

      match(Token.FOR); // accept Token "FOR"
      match(Token.LPAREN);
      if (currentToken.kind != Token.SEMICOLON)
          eAST1 = parseExpr();
      match(Token.SEMICOLON);
      if (currentToken.kind != Token.SEMICOLON)
          eAST2 = parseExpr();
      match(Token.SEMICOLON);
      if (currentToken.kind != Token.RPAREN)
          eAST3 = parseExpr();
      match(Token.RPAREN);
      sAST = parseStmt();

      if (eAST1 == null)
          eAST1 = new EmptyExpr(dummyPos);
      if (eAST2 == null)
          eAST2 = new EmptyExpr(dummyPos);
      if (eAST3 == null)
          eAST3 = new EmptyExpr(dummyPos);

      finish(forStmtPos);
      return new ForStmt(eAST1,eAST2, eAST3, sAST, forStmtPos);
  }

  // while-stmt -> while "(" expr ")" stmt
  Stmt parseWhileStmt() throws SyntaxError {
      Expr eAST = null;
      Stmt sAST = null;
      SourcePosition whileStmtPos = new SourcePosition();
      start(whileStmtPos);

      match(Token.WHILE); // accept Token "WHILE"
      match(Token.LPAREN);
      eAST = parseExpr();
      match(Token.RPAREN);
      sAST = parseStmt();
      finish(whileStmtPos);
      return new WhileStmt(eAST, sAST, whileStmtPos);
  }

  // break-stmt -> break ";"
  Stmt parseBreakStmt() throws SyntaxError {
      SourcePosition breakStmtPos = new SourcePosition();
      start(breakStmtPos);

      match(Token.BREAK); // accept Token "BREAK"
      match(Token.SEMICOLON);
      finish(breakStmtPos);
      return new BreakStmt(breakStmtPos);
  }

  // continue-stmt -> continue ";"
  Stmt parseContinueStmt() throws SyntaxError {
      SourcePosition continueStmtPos = new SourcePosition();
      start(continueStmtPos);

      match(Token.CONTINUE); // accept Token "CONTINUE"
      match(Token.SEMICOLON);
      finish(continueStmtPos);
      return new ContinueStmt(continueStmtPos);
  }

  // return-stmt -> return expr? ";"
  Stmt parseReturnStmt() throws SyntaxError {
      Expr eAST = null;
      SourcePosition returnStmtPos = new SourcePosition();
      start(returnStmtPos);

      match(Token.RETURN);
      if (currentToken.kind != Token.SEMICOLON){
          eAST = parseExpr();
      }
      match(Token.SEMICOLON);
      finish(returnStmtPos);
      if (eAST == null){
          eAST = new EmptyExpr(dummyPos);
      }

      return new ReturnStmt(eAST, returnStmtPos);
  }
  //-------------------------------------------------------------------------------------

  // expr-stmt -> expr? ";"
  Stmt parseExprStmt() throws SyntaxError {
    Stmt sAST = null;
    SourcePosition stmtPos = new SourcePosition();
    start(stmtPos);

    if (currentToken.kind != Token.SEMICOLON){ //attention
        Expr eAST = parseExpr();
        match(Token.SEMICOLON);
        finish(stmtPos);
        sAST = new ExprStmt(eAST, stmtPos);
    } else {
      match(Token.SEMICOLON);
      finish(stmtPos);
      sAST = new ExprStmt(new EmptyExpr(dummyPos), stmtPos);
    }
    return sAST;
  }


// ======================= PARAMETERS =======================
// parameters
//    para-list           -> "(" proper-para-list? ")"
//    proper-para-list    -> para-decl ( "," para-decl )*
//    para-decl           -> type declarator
//    arg-list            -> "(" proper-arg-list? ")"
//    proper-arg-list     -> arg ( "," arg )*
//    arg                 -> expr


  List parseParaList() throws SyntaxError {
    List formalsAST = null;
    SourcePosition formalsPos = new SourcePosition();
    start(formalsPos);

    match(Token.LPAREN);
    if (currentToken.kind != Token.RPAREN){
        formalsAST = parseProperParaList();
    }
    match(Token.RPAREN);
    finish(formalsPos);
    if (formalsAST == null) { // attention, may (formalsAST instanceof EmptyParaList)
        formalsAST = new EmptyParaList(formalsPos);
    }
    return formalsAST;
  }

   //proper-para-list -> para-decl ( "," para-decl )*
   List parseProperParaList() throws SyntaxError {
       ParaDecl properDeclAST = null;
       List properParaList = null;

       SourcePosition properDeclPos = new SourcePosition();
       start(properDeclPos);
       properDeclAST = parseParaDecl();

       if (properDeclAST != null){
           if (currentToken.kind == Token.COMMA){
               match(Token.COMMA);
               properParaList = parseProperParaList();
               finish(properDeclPos);
               properParaList = new ParaList(properDeclAST, properParaList, properDeclPos);
           }
           else if (currentToken.kind == Token.RPAREN){
               finish(properDeclPos);
               properParaList = new ParaList(properDeclAST, new EmptyParaList(dummyPos), properDeclPos);
           }
       }
       else{
           properParaList = new EmptyParaList(dummyPos);
       }

       return properParaList;
   }

  // para-decl -> type declarator
  ParaDecl parseParaDecl() throws SyntaxError {
      Type tAST = null;
      Ident idAST = null;
      ParaDecl paraDeclAST = null;

      SourcePosition paraDeclPos = new SourcePosition();
      start(paraDeclPos);
      tAST = parseType();
      idAST = parseIdent();
      if (currentToken.kind == Token.LBRACKET){
          tAST = parseArrayType(tAST);
      }
      finish(paraDeclPos);

      paraDeclAST = new ParaDecl(tAST, idAST, paraDeclPos);
      return paraDeclAST;
  }


  // ----------------------------------attention--------------------------------
  // arg-list -> "(" proper-arg-list? ")"
  List parseArgList() throws SyntaxError {
//      Arg aAST =null;
      List lAST = null;
      SourcePosition argListPos = new SourcePosition();
      start(argListPos);
      match(Token.LPAREN);
      if (currentToken.kind != Token.RPAREN){
          lAST = parseProperArgList();
      }
      match(Token.RPAREN);
      if (lAST == null){
          lAST = new EmptyArgList(dummyPos);
      }
      return lAST;
  }

  // proper-arg-list -> arg ( "," arg )*
  List parseProperArgList() throws SyntaxError {
      Arg aAST =null;
      List lAST = null;
      SourcePosition properArgListPos = new SourcePosition();
      start(properArgListPos);
      aAST = parseArg();
      if (aAST != null) {
          if (currentToken.kind == Token.COMMA) {
            match(Token.COMMA);
            lAST = parseProperArgList();
            finish(properArgListPos);
            lAST = new ArgList(aAST, lAST, properArgListPos);
          }
          else if (currentToken.kind == Token.RPAREN){
              finish(properArgListPos);
              lAST = new ArgList(aAST, new EmptyArgList(dummyPos), properArgListPos);
          }
      }
      else {
          lAST = new EmptyArgList(dummyPos);
      }

      return lAST;
  }

  // arg -> expr
  Arg parseArg() throws SyntaxError {
      Expr eAST = null;
      Arg aAST = null;
      SourcePosition argPos = new SourcePosition();
      start(argPos);
      eAST = parseExpr();
      finish(argPos);

      aAST = new Arg(eAST, argPos);

      return aAST;
  }
  //----------------------------------------------------------------------------


// ======================= EXPRESSIONS ======================


  Expr parseExpr() throws SyntaxError {
    Expr exprAST = null;
//    exprAST = parseAdditiveExpr();
    exprAST = parseAssignmentExpr();
    if (exprAST == null){
        exprAST = new EmptyExpr(dummyPos);
    }

    return exprAST;
  }

  // assignment-expr -> ( cond-or-expr "=" )* cond-or-expr
  Expr parseAssignmentExpr() throws SyntaxError {
      Expr exprAST = null;
      Expr e2AST = null;
      SourcePosition assignStartPos = new SourcePosition();
      start(assignStartPos);

      exprAST = parseCondOrExpr();
      if (currentToken.kind == Token.EQ){
          acceptOperator();
          e2AST = parseAssignmentExpr();
      }
      finish(assignStartPos);
      if (e2AST != null){
          exprAST = new AssignExpr(exprAST, e2AST, assignStartPos);
      }
      else if (exprAST == null){
          syntacticError("illegal assignment", "");
      }
      return exprAST;
  }

  /* cond-or-expr -> cond-and-expr
   *              |  cond-or-expr "||" cond-and-expr
   *
   * C1  -> C2C1*
   * C1* -> "||"C2C1* | epsilon
   *
   * include: ||, &&, ==, !=, >=, >, <=, <
   */
  Expr parseCondOrExpr() throws SyntaxError {
      Expr exprAST = null;
      SourcePosition orStartPos = new SourcePosition();
      start(orStartPos);

      exprAST = parseCondAndExpr();
      while (currentToken.kind == Token.OROR){
          Operator opAst = acceptOperator();
          Expr e2AST = parseCondAndExpr();
          SourcePosition orPos = new SourcePosition();

          copyStart(orStartPos, orPos);
          finish(orPos);
          exprAST = new BinaryExpr(exprAST, opAst, e2AST, orPos);
      }
      return exprAST;
  }

  /* cond-and-expr -> equality-expr
   *               |  cond-and-expr "&&" equality-expr
   *
   * C2  -> EC2*
   * C2* -> "&&"EC2* | epsilon
   */
  Expr parseCondAndExpr() throws SyntaxError {
      Expr exprAST = null;
      SourcePosition andStartPos = new SourcePosition();
      start(andStartPos);

      exprAST = parseEqualityExpr();
      while (currentToken.kind == Token.ANDAND){
          Operator opAst = acceptOperator();
          Expr e2AST = parseEqualityExpr();
          SourcePosition andPos = new SourcePosition();

          copyStart(andStartPos, andPos);
          finish(andPos);
          exprAST = new BinaryExpr(exprAST, opAst, e2AST, andPos);
      }
      return exprAST;
  }

    /*equality-expr -> rel-expr
     *              |  equality-expr "==" rel-expr
     *              |  equality-expr "!=" rel-expr
     *
     * E  -> RE*
     * E* -> "=="RE* | "!="RE* | epsilon
     *
     */
  Expr parseEqualityExpr() throws SyntaxError {
      Expr exprAST = null;
      SourcePosition equalityStartPos = new SourcePosition();
      start(equalityStartPos);

      exprAST = parseRelExpr();
      while (currentToken.kind == Token.EQEQ || currentToken.kind == Token.NOTEQ){
          Operator opAst = acceptOperator();
          Expr e2AST = parseRelExpr();
          SourcePosition equalityPos = new SourcePosition();

          copyStart(equalityStartPos, equalityPos);
          finish(equalityPos);
          exprAST = new BinaryExpr(exprAST, opAst, e2AST, equalityPos);
      }
      return exprAST;
  }

  /*rel-expr -> additive-expr
   *         |  rel-expr "<" additive-expr
   *         |  rel-expr "<=" additive-expr
   *         |  rel-expr ">" additive-expr
   *         |  rel-expr ">=" additive-expr
   *
   *
   * R  -> AR*
   * R* -> "<"AR* | "<="AR* | ">"AR* | ">="AR* | epsilon
   */
  Expr parseRelExpr() throws SyntaxError {
      Expr exprAST = null;
      SourcePosition relStartPos = new SourcePosition();
      start(relStartPos);

      exprAST = parseAdditiveExpr();
      while (currentToken.kind == Token.LT || currentToken.kind == Token.LTEQ
            || currentToken.kind == Token.GT || currentToken.kind == Token.GTEQ){
          Operator opAst = acceptOperator();
          Expr e2AST = parseAdditiveExpr();
          SourcePosition relPos = new SourcePosition();

          copyStart(relStartPos, relPos);
          finish(relPos);
          exprAST = new BinaryExpr(exprAST, opAst, e2AST, relPos);
      }
      return exprAST;
  }

  /* additive-expr -> multiplicative-expr
                   |  additive-expr "+" multiplicative-expr
                   |  additive-expr "-" multiplicative-expr

   * A  -> MA*
   * A* -> "+"MA*  | "-"MA* | epsilon
   */
  Expr parseAdditiveExpr() throws SyntaxError {
    Expr exprAST = null;

    SourcePosition addStartPos = new SourcePosition();
    start(addStartPos);

    exprAST = parseMultiplicativeExpr();
    while (currentToken.kind == Token.PLUS
           || currentToken.kind == Token.MINUS) {
      Operator opAST = acceptOperator();
      Expr e2AST = parseMultiplicativeExpr();

      SourcePosition addPos = new SourcePosition();
      copyStart(addStartPos, addPos);
      finish(addPos);
      exprAST = new BinaryExpr(exprAST, opAST, e2AST, addPos);
    }
    return exprAST;
  }

  Expr parseMultiplicativeExpr() throws SyntaxError {
    Expr exprAST = null;

    SourcePosition multStartPos = new SourcePosition();
    start(multStartPos);

    exprAST = parseUnaryExpr();
    while (currentToken.kind == Token.MULT
           || currentToken.kind == Token.DIV) {
      Operator opAST = acceptOperator();
      Expr e2AST = parseUnaryExpr();
      SourcePosition multPos = new SourcePosition();
      copyStart(multStartPos, multPos);
      finish(multPos);
      exprAST = new BinaryExpr(exprAST, opAST, e2AST, multPos);
    }
    return exprAST;
  }

  Expr parseUnaryExpr() throws SyntaxError {
    Expr exprAST = null;

    SourcePosition unaryPos = new SourcePosition();
    start(unaryPos);

    switch (currentToken.kind) {
      case Token.MINUS:
        {
          Operator opAST1 = acceptOperator();
          Expr e2AST1 = parseUnaryExpr();
          finish(unaryPos);
          exprAST = new UnaryExpr(opAST1, e2AST1, unaryPos);
        }
        break;
      case Token.PLUS:
      {
          Operator opAST2 = acceptOperator();
          Expr e2AST2 = parseUnaryExpr();
          finish(unaryPos);
          exprAST = new UnaryExpr(opAST2, e2AST2, unaryPos);
      }
      break;
      case Token.NOT:
      {
          Operator opAST3 = acceptOperator();
          Expr e2AST3 = parseUnaryExpr();
          finish(unaryPos);
          exprAST = new UnaryExpr(opAST3, e2AST3, unaryPos);
      }
      break;

      default:
        exprAST = parsePrimaryExpr();
        break;
       
    }
    return exprAST;
  }

  /*primary-expr -> identifier arg-list?
   *             | identifier "[" expr "]"
   *             | "(" expr ")"
   *             | INTLITERAL
   *             | FLOATLITERAL
   *             | BOOLLITERAL
   *             | STRINGLITERAL
   */
  Expr parsePrimaryExpr() throws SyntaxError {
    Expr exprAST = null;

    SourcePosition primPos = new SourcePosition();
    start(primPos);

    switch (currentToken.kind) {

      // problem may be solved
      case Token.ID: {
          Ident iAST = parseIdent();
          SourcePosition subPos = new SourcePosition();
          copyStart(primPos, subPos);
          finish(subPos);
          Var simVAST = new SimpleVar(iAST, subPos);
          if (currentToken.kind == Token.LBRACKET) {
              match(Token.LBRACKET);
              exprAST = parseExpr();
              finish(primPos);
              exprAST = new ArrayExpr(simVAST, exprAST, primPos);
              match(Token.RBRACKET);
          } else if (currentToken.kind == Token.LPAREN) {
              List argListAST = parseArgList();
              finish(primPos);
              exprAST = new CallExpr(iAST, argListAST, primPos);
          } else {
              exprAST = new VarExpr(simVAST, subPos);
          }

          break;
      }

      case Token.LPAREN:
        {
          match(Token.LPAREN);
          exprAST = parseExpr();
          match(Token.RPAREN);
        }
        break;

      case Token.INTLITERAL:
        IntLiteral ilAST1 = parseIntLiteral();
        finish(primPos);
        exprAST = new IntExpr(ilAST1, primPos);
        break;
      case Token.FLOATLITERAL:
          FloatLiteral ilAST2 = parseFloatLiteral();
          finish(primPos);
          exprAST = new FloatExpr(ilAST2, primPos);
          break;
      case Token.BOOLEANLITERAL:
          BooleanLiteral ilAST3 = parseBooleanLiteral();
          finish(primPos);
          exprAST = new BooleanExpr(ilAST3, primPos);
          break;
      case Token.STRINGLITERAL:
          StringLiteral ilAST4 = parseStringLiteral();
          finish(primPos);
          exprAST = new StringExpr(ilAST4, primPos);
          break;
      default:
        syntacticError("illegal primary expression", currentToken.spelling);
    }
    return exprAST;
  }

// ========================== ID, OPERATOR and LITERALS ========================

  Ident parseIdent() throws SyntaxError {
    Ident I = null; 

    if (currentToken.kind == Token.ID) {
      previousTokenPosition = currentToken.position;
      String spelling = currentToken.spelling;
      I = new Ident(spelling, previousTokenPosition);
      currentToken = scanner.getToken();
    } else 
      syntacticError("identifier expected here", "");
    return I;
  }

// acceptOperator parses an operator, and constructs a leaf AST for it

  Operator acceptOperator() throws SyntaxError {
    Operator O = null;
    previousTokenPosition = currentToken.position;
    String spelling = currentToken.spelling;
    O = new Operator(spelling, previousTokenPosition);
    currentToken = scanner.getToken();
    return O;
  }


  IntLiteral parseIntLiteral() throws SyntaxError {
    IntLiteral IL = null;
    if (currentToken.kind == Token.INTLITERAL) {
      String spelling = currentToken.spelling;
      accept();
      IL = new IntLiteral(spelling, previousTokenPosition);
    } else 
      syntacticError("integer literal expected here", "");
    return IL;
  }

  FloatLiteral parseFloatLiteral() throws SyntaxError {
    FloatLiteral FL = null;

    if (currentToken.kind == Token.FLOATLITERAL) {
      String spelling = currentToken.spelling;
      accept();
      FL = new FloatLiteral(spelling, previousTokenPosition);
    } else 
      syntacticError("float literal expected here", "");
    return FL;
  }

  BooleanLiteral parseBooleanLiteral() throws SyntaxError {
    BooleanLiteral BL = null;

    if (currentToken.kind == Token.BOOLEANLITERAL) {
      String spelling = currentToken.spelling;
      accept();
      BL = new BooleanLiteral(spelling, previousTokenPosition);
    } else 
      syntacticError("boolean literal expected here", "");
    return BL;
  }

  StringLiteral parseStringLiteral() throws SyntaxError {
      StringLiteral SL = null;

      if (currentToken.kind == Token.STRINGLITERAL) {
          String spelling = currentToken.spelling;
          accept();
          SL = new StringLiteral(spelling, previousTokenPosition);
      } else
          syntacticError("string literal expected here", "");
      return SL;
  }
//   ======================= DECLARATIONS ======================
// declarations
//    func-decl           -> type identifier para-list compound-stmt
//    var-decl            -> type init-declarator-list ";"
//    init-declarator-list-> init-declarator ( "," init-declarator )*
//    init-declarator     -> declarator ( "=" initialiser )?
//    declarator          -> identifier
//                    |  identifier "[" INTLITERAL? "]"
//    initialiser         -> expr
//                    |  "{" expr ( "," expr )* "}"

  // var-decl -> type init-declarator-list ";"
  // global variable delaration
  Decl parseGlobaleVarDecl(Type tAST, Ident iAST) throws SyntaxError {
    Decl dAST = null;
    Expr eAST =null;
    SourcePosition globalVarPos = new SourcePosition();

    start(globalVarPos);

    tAST = parseArrayType(tAST);
    if (currentToken.kind == Token.EQ){
        match(Token.EQ);
        eAST = parseInitialiserExpr();
    }
//    else{
//        eAST = new EmptyExpr(dummyPos);
//    }
    finish(globalVarPos);

    if (iAST != null && eAST != null){
        dAST = new GlobalVarDecl(tAST, iAST, eAST, globalVarPos);
    }
    else if (iAST != null){
        dAST = new GlobalVarDecl(tAST, iAST, new EmptyExpr(dummyPos), globalVarPos);
    }
//    if (iAST == null){
//        dAST = new EmptyDeclList(dummyPos);
//    }

    return dAST;
  }

  Type parseArrayType(Type T) throws SyntaxError {
      Type tAST = null;
      Expr eAST = null;
      IntLiteral IL = null;
      SourcePosition typePos = new SourcePosition();
      SourcePosition intPos = new SourcePosition();
      start(typePos);

      if (currentToken.kind == Token.LBRACKET){
          match(Token.LBRACKET);
          if (currentToken.kind == Token.INTLITERAL){
              IL = parseIntLiteral();
          }
          match(Token.RBRACKET);
          finish(typePos);
          if (IL == null) {
              eAST = new EmptyExpr(dummyPos);
          }
          else {
              eAST = new IntExpr(IL, typePos);
          }
          tAST = new ArrayType(T, eAST, typePos);
      }
      else{
          finish(typePos);
          tAST = T;
      }

      return tAST;
  }

  // initialiser -> expr
  //             |  "{" expr ( "," expr )* "}"
  Expr parseInitialiserExpr() throws SyntaxError {
      Expr eAST = null;
      List lAST = null;
      SourcePosition initialiserPos = new SourcePosition();
      start(initialiserPos);

      if (currentToken.kind == Token.LCURLY){
          match(Token.LCURLY);
          if (currentToken.kind == Token.RCURLY){
              match(Token.RCURLY);
              syntacticError("illegal initialiser", "");
          }
          lAST = parseRecursiveExpr();
          finish(initialiserPos);
          eAST = new InitExpr(lAST, initialiserPos);
          match(Token.RCURLY);
      }
      else {
          eAST = parseExpr();
      }
      return eAST;
  }


  List parseRecursiveExpr() throws SyntaxError {
      List lAST = null;
      List l2AST = null;
      Expr eAST = null;
      SourcePosition exprPos = new SourcePosition();
      start(exprPos);
      eAST = parseExpr();

      if (currentToken.kind == Token.COMMA){
          match(Token.COMMA);
          lAST = parseRecursiveExpr();
      }
      finish(exprPos);


      if (lAST == null){
          l2AST = new ExprList(eAST, new EmptyExprList(dummyPos), exprPos);
      }
      else {
          l2AST = new ExprList(eAST, lAST, exprPos);
      }
//      if (eAST == null){
//          l2AST = new EmptyExprList(dummyPos);
//      }

      return l2AST;
  }


  // ------------------------------------local variable----------------------------------------------------------
  List parseLocalVarDeclList(Type tAST) throws SyntaxError {
      List lAST = null;
      List l2AST = null;
      Expr eAST = null;
      Decl dAST = null;
      Ident iAST = parseIdent();
      SourcePosition localListPos = new SourcePosition();
      start(localListPos);

      dAST = parseLocalVarDecl(tAST, iAST);
      if (currentToken.kind == Token.COMMA){
          match(Token.COMMA);
          lAST = parseLocalVarDeclList(tAST);
      }
      else if (currentToken.kind == Token.SEMICOLON){
          match(Token.SEMICOLON);
          if (currentToken.kind != Token.EOF && currentToken.kind != Token.RCURLY){
              if (typeChecker(currentToken.kind)) {
                  Type t2AST = parseType();
                  lAST = parseLocalVarDeclList(t2AST);
              }
          }
      }
      finish(localListPos);

      if (dAST != null && lAST != null){
          l2AST = new DeclList(dAST, lAST, localListPos);
      }
      else if (dAST != null){
          l2AST = new DeclList(dAST, new EmptyDeclList(dummyPos), localListPos);
      }
      if (dAST == null){
          l2AST = new EmptyDeclList(dummyPos);
      }

      return l2AST;
  }

  Decl parseLocalVarDecl(Type tAST, Ident iAST) throws SyntaxError {
      Decl dAST = null;
      Expr eAST =null;
      SourcePosition locallVarPos = new SourcePosition();

      start(locallVarPos);
      tAST = parseArrayType(tAST);
      if (currentToken.kind == Token.EQ){
          match(Token.EQ);
          eAST = parseInitialiserExpr();
      }

      finish(locallVarPos);
      if (iAST != null && eAST != null){
          dAST = new LocalVarDecl(tAST, iAST, eAST, locallVarPos);
      }
      else if (iAST != null){
          dAST = new LocalVarDecl(tAST, iAST, new EmptyExpr(dummyPos), locallVarPos);
      }

      return dAST;
  }

  boolean typeChecker(int type){
      for(int i = 0; i < 4; i++){
          if (type == types[i]){
              return true;
          }
      }
      return false;
  }
}