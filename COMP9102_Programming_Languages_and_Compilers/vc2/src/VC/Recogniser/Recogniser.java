/***
 * *
 * * src.VC.Recogniser.java
 * *
 ***/

/* At this stage, this parser accepts a subset of VC defined	by
 * the following grammar. 
 *
 * You need to modify the supplied parsing methods (if necessary) and 
 * add the missing ones to obtain a parser for the VC language.
 *
 * (27---Feb---2019)

program       -> func-decl

// declaration

func-decl     -> void identifier "(" ")" compound-stmt

identifier    -> ID

// statements 
compound-stmt -> "{" stmt* "}" 
stmt          -> continue-stmt
    	      |  expr-stmt
continue-stmt -> continue ";"
expr-stmt     -> expr? ";"

// expressions 
expr                -> assignment-expr
assignment-expr     -> additive-expr
additive-expr       -> multiplicative-expr
                    |  additive-expr "+" multiplicative-expr
                    |  additive-expr "-" multiplicative-expr
multiplicative-expr -> unary-expr
	            |  multiplicative-expr "*" unary-expr
	            |  multiplicative-expr "/" unary-expr

unary-expr  -> "-" unary-expr
            |  "+" unary-expr
		    |  primary-expr

primary-expr -> identifier
 		    |  INTLITERAL
		    | "(" expr ")"
*/

package VC.Recogniser;

import VC.Scanner.Scanner;
import VC.Scanner.SourcePosition;
import VC.Scanner.Token;
import VC.ErrorReporter;

public class Recogniser {

  private Scanner scanner;
  private ErrorReporter errorReporter;
  private Token currentToken;

  private boolean funcOrVarFlag = false;

  private int[] types = {Token.INT, Token.FLOAT, Token.BOOLEAN, Token.VOID};

  public Recogniser (Scanner lexer, ErrorReporter reporter) {
    scanner = lexer;
    errorReporter = reporter;

    currentToken = scanner.getToken();
  }

// match checks to see f the current token matches tokenExpected.
// If so, fetches the next token.
// If not, reports a syntactic error.

  void match(int tokenExpected) throws SyntaxError {
    if (currentToken.kind == tokenExpected) {
        currentToken = scanner.getToken();
    } else {
      syntacticError("\"%\" expected here", Token.spell(tokenExpected));
    }
  }

 // accepts the current token and fetches the next
  void accept() {
    currentToken = scanner.getToken();
  }

  void syntacticError(String messageTemplate, String tokenQuoted) throws SyntaxError {
    SourcePosition pos = currentToken.position;
    errorReporter.reportError(messageTemplate, tokenQuoted, pos);
    throw(new SyntaxError());
  }


// ========================== PROGRAMS ========================

  public void parseProgram() {
    try {
      while (typeChecker(currentToken.kind)) {
          parseFuncAndVarFactoring();
      }
      if (currentToken.kind != Token.EOF) {
        syntacticError("\"%\" wrong result type for a function", currentToken.spelling);
      }
    }
    catch (SyntaxError s) {  }
  }

// ========================== DECLARATIONS ========================
  boolean varDeclNotFirstId = true;
  void parseFuncAndVarFactoring() throws SyntaxError{
      parseType();
      parseIdent();
      if (currentToken.kind == Token.LPAREN){
          parseFuncDecl();
      }
      else{
          varDeclNotFirstId = false;
          parseVarDecl();
      }
  }

  // func-decl -> type identifier para-list compound-stmt
  void parseFuncDecl() throws SyntaxError {
      parseParaList();
      parseCompoundStmt();
  }

  // var-decl -> type init-declarator-list ";"
  void parseVarDecl() throws SyntaxError {
      parseInitDeclaratorList();
      match(Token.SEMICOLON);
  }

  // init-declarator-list -> init-declarator ( "," init-declarator )*
  void parseInitDeclaratorList() throws SyntaxError {
      parseInitDeclarator();
      while (currentToken.kind == Token.COMMA){
          match(Token.COMMA);
          parseInitDeclarator();
      }
  }

  // init-declarator -> declarator ( "=" initialiser )?
  void parseInitDeclarator() throws SyntaxError {
      parseDeclarator();
      if (currentToken.kind == Token.EQ){
          match(Token.EQ);
          parseInitialiser();
      }

  }

  // declarator -> identifier
  //            |  identifier "[" INTLITERAL? "]"
  void parseDeclarator() throws SyntaxError {
      if (varDeclNotFirstId) {
          parseIdent();
      }else{
          varDeclNotFirstId = true;
      }
      if (currentToken.kind == Token.LBRACKET){
          match(Token.LBRACKET);
          if (currentToken.kind == Token.INTLITERAL){
              parseIntLiteral();
          }
          match(Token.RBRACKET);
      }
  }

  // initialiser -> expr
  //             |  "{" expr ( "," expr )* "}"
  void parseInitialiser() throws SyntaxError {
      if (currentToken.kind == Token.LCURLY){
          match(Token.LCURLY);
          parseExpr();
          while (currentToken.kind == Token.COMMA){
              match(Token.COMMA);
              parseExpr();
          }
          match(Token.RCURLY);
      }
      else{
          parseExpr();
      }
  }

// ======================= PRIMARY TYPES ==============================
//type -> void | boolean | int | float
  void parseType() throws SyntaxError {
      switch (currentToken.kind){
      case Token.VOID:
          match(Token.VOID);
          break;
      case Token.INT:
          match(Token.INT);
          break;
      case Token.FLOAT:
          match(Token.FLOAT);
          break;
      case Token.BOOLEAN:
          match(Token.BOOLEAN);
          break;
      default:
          syntacticError("type expected here", "");
          break;
      }
  }


// ======================= STATEMENTS ==============================
//    stmt                -> compound-stmt
//                    |  if-stmt
//                    |  for-stmt
//                    |  while-stmt
//                    |  break-stmt
//                    |  continue-stmt
//                    |  return-stmt
//                    |  expr-stmt
//
  //-----------------------------------------------------
  void parseStmt() throws SyntaxError {
      switch (currentToken.kind){
        case Token.IF:
            parseIfStmt();
            break;
        case Token.FOR:
            parseForStmt();
            break;
        case Token.WHILE:
            parseWhileStmt();
            break;
        case Token.BREAK:
            parseBreakStmt();
            break;
        case Token.CONTINUE:
            parseContinueStmt();
            break;
        case Token.RETURN:
            parseReturnStmt();
            break;
        case Token.LCURLY:
            parseCompoundStmt();
            break;
        default:
            parseExprStmt();
            break;
      }
  }

  // for-stmt -> for "(" expr? ";" expr? ";" expr? ")" stmt
  void parseForStmt() throws SyntaxError { // attention!!!
      match(Token.FOR);
      match(Token.LPAREN);
      if (currentToken.kind != Token.SEMICOLON)
        parseExpr();
      match(Token.SEMICOLON);
      if (currentToken.kind != Token.SEMICOLON)
          parseExpr();
      match(Token.SEMICOLON);
      if (currentToken.kind != Token.RPAREN)
          parseExpr();
      match(Token.RPAREN);
      parseStmt();
  }

  // if-stmt -> if "(" expr ")" stmt ( else stmt )?
  void parseIfStmt() throws SyntaxError {
      match(Token.IF);
      match(Token.LPAREN);
      parseExpr();
      match(Token.RPAREN);
      parseStmt();
      if (currentToken.kind == Token.ELSE){
          match(Token.ELSE);
          parseStmt();
      }
  }

  // while-stmt -> while "(" expr ")" stmt
  void parseWhileStmt() throws SyntaxError {
      match(Token.WHILE);
      match(Token.LPAREN);
      parseExpr();
      match(Token.RPAREN);
      parseStmt();
  }

  // break-stmt -> break ";"
  void parseBreakStmt() throws SyntaxError {
      match(Token.BREAK);
      match(Token.SEMICOLON);
  }

  // return-stmt -> return expr? ";"
  void parseReturnStmt() throws SyntaxError {
      match(Token.RETURN);
      if (currentToken.kind != Token.SEMICOLON){
          parseExpr();
      }
      match(Token.SEMICOLON);
  }
  //-----------------------------------------------------
  // continue-stmt -> continue ";"
  void parseContinueStmt() throws SyntaxError {
      match(Token.CONTINUE);
      match(Token.SEMICOLON);
  }

  // compound-stmt -> "{" var-decl* stmt* "}"
  void parseCompoundStmt() throws SyntaxError {
    match(Token.LCURLY);
    if (currentToken.kind != Token.RCURLY) {
        parseVarDeclList();
        parseStmtList();
    }
    match(Token.RCURLY);
  }

 // Here, a new nontermial has been introduced to define stmt*
  void parseStmtList() throws SyntaxError {
    while (currentToken.kind != Token.RCURLY)
      parseStmt();
  }

  // Here, a new nontermial has been introduced to define var-decl*
  void parseVarDeclList() throws SyntaxError {
      while (typeChecker(currentToken.kind)) {
          parseType();
          parseVarDecl();
      }
  }

  // modified
  // expr-stmt -> expr? ";"
  void parseExprStmt() throws SyntaxError {
      if (currentToken.kind != Token.SEMICOLON){ //attention
          parseExpr();
          match(Token.SEMICOLON);
      } else {
          match(Token.SEMICOLON);
      }
  }

// ======================= IDENTIFIERS ======================

 // Call parseIdent rather than match(Token.ID). 
 // In Assignment 3, an Identifier node will be constructed in here.


  void parseIdent() throws SyntaxError {
    if (currentToken.kind == Token.ID) {
        currentToken = scanner.getToken();
    } else
      syntacticError("identifier expected here", "");
  }

// ======================= OPERATORS ======================

 // Call acceptOperator rather than accept(). 
 // In Assignment 3, an Operator Node will be constructed in here.

  void acceptOperator() throws SyntaxError {
    currentToken = scanner.getToken();
  }


// ======================= EXPRESSIONS ======================
//// expressions
//    expr                -> assignment-expr
//    assignment-expr     -> ( cond-or-expr "=" )* cond-or-expr
//    cond-or-expr        -> cond-and-expr
//                    |  cond-or-expr "||" cond-and-expr
//    cond-and-expr       -> equality-expr
//                    |  cond-and-expr "&&" equality-expr
//    equality-expr       -> rel-expr
//                    |  equality-expr "==" rel-expr
//                    |  equality-expr "!=" rel-expr
//    rel-expr            -> additive-expr
//                    |  rel-expr "<" additive-expr
//                    |  rel-expr "<=" additive-expr
//                    |  rel-expr ">" additive-expr
//                    |  rel-expr ">=" additive-expr
//    additive-expr       -> multiplicative-expr
//                    |  additive-expr "+" multiplicative-expr
//                    |  additive-expr "-" multiplicative-expr
//    multiplicative-expr -> unary-expr
//                    |  multiplicative-expr "*" unary-expr
//                    |  multiplicative-expr "/" unary-expr
//    unary-expr          -> "+" unary-expr
//                    |  "-" unary-expr
//                    |  "!" unary-expr
//                    |  primary-expr
//
//    primary-expr        -> identifier arg-list?
//            | identifier "[" expr "]"
//            | "(" expr ")"
//            | INTLITERAL
//                    | FLOATLITERAL
//                    | BOOLLITERAL
//                    | STRINGLITERAL

  // expr -> assignment-expr
  void parseExpr() throws SyntaxError {
      parseAssignExpr();
  }

  // assignment-expr -> ( cond-or-expr "=" )* cond-or-expr
  void parseAssignExpr() throws SyntaxError {
      parseCondOrExpr();
      while (currentToken.kind == Token.EQ){
          acceptOperator();
          parseCondOrExpr();
      }
  }

    /* cond-or-expr -> cond-and-expr
     *              |  cond-or-expr "||" cond-and-expr
     *
     * C1  -> C2C1*
     * C1* -> "||"C2C1* | epsilon
     */

  void parseCondOrExpr() throws SyntaxError {
      parseCondAndExpr();
      extraCondOrExpr();
  }

  void extraCondOrExpr() throws SyntaxError {
      while (currentToken.kind == Token.OROR){
          acceptOperator();
          parseCondAndExpr();
      }
  }
  /* cond-and-expr -> equality-expr
   *               |  cond-and-expr "&&" equality-expr
   *
   * C2  -> EC2*
   * C2* -> "&&"EC2* | epsilon
   */
  void parseCondAndExpr() throws SyntaxError {
      parseEqualityExpr();
      extraCondAndExpr();
  }

  void extraCondAndExpr() throws SyntaxError {
      while (currentToken.kind == Token.ANDAND){
          acceptOperator();
          parseEqualityExpr();
      }
  }

    /*equality-expr -> rel-expr
     *              |  equality-expr "==" rel-expr
     *              |  equality-expr "!=" rel-expr
     *
     * E  -> RE*
     * E* -> "=="RE* | "!="RE* | epsilon
     *
     */
  void parseEqualityExpr() throws SyntaxError {
      parseRelExpr();
      extraEqualityExpr();
  }

  void extraEqualityExpr() throws SyntaxError {
      while ((currentToken.kind == Token.EQEQ) || (currentToken.kind == Token.NOTEQ)){
          acceptOperator();
          parseRelExpr();
      }
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
  void parseRelExpr() throws SyntaxError {
      parseAdditiveExpr();
      extraRelExpr();
  }

  void extraRelExpr() throws SyntaxError {
      while ((currentToken.kind == Token.LT) || (currentToken.kind == Token.LTEQ)
                || (currentToken.kind == Token.GT) || (currentToken.kind == Token.GTEQ)){
          acceptOperator();
          parseAdditiveExpr();
      }
  }

    /*additive-expr -> multiplicative-expr
                    |  additive-expr "+" multiplicative-expr
                    |  additive-expr "-" multiplicative-expr

       A  -> MA*
       A* -> "+"MA* | "-"MA* | epsilon
     */
  void parseAdditiveExpr() throws SyntaxError {
    parseMultiplicativeExpr();
    extraAdditiveExpr();
  }

  void extraAdditiveExpr() throws  SyntaxError {
      while ((currentToken.kind == Token.PLUS) || (currentToken.kind == Token.MINUS)){
          acceptOperator();
          parseMultiplicativeExpr();
      }
  }

    /* multiplicative-expr -> unary-expr
     *                     |  multiplicative-expr "*" unary-expr
     *                     |  multiplicative-expr "/" unary-expr
     *
     * M  -> UM*
     * M* -> "*"UM* | "/"UM* | epsilon
     *
     */
  void parseMultiplicativeExpr() throws SyntaxError {
      parseUnaryExpr();
      extraMultiplicativeExpr();
  }

  void extraMultiplicativeExpr() throws SyntaxError {
      while ((currentToken.kind == Token.MULT) || (currentToken.kind == Token.DIV)) {
          acceptOperator();
          parseUnaryExpr();
      }
  }

  /* unary-expr -> "+" unary-expr
   *            |  "-" unary-expr
   *            |  "!" unary-expr
   *            |  primary-expr
   */
  void parseUnaryExpr() throws SyntaxError {
    switch (currentToken.kind) {
      case Token.PLUS:
        {
          acceptOperator();
          parseUnaryExpr();
        }
        break;
      case Token.MINUS:
      {
          acceptOperator();
          parseUnaryExpr();
      }
      break;
      case Token.NOT:
      {
          acceptOperator();
          parseUnaryExpr();
      }
      break;

    default:
        parsePrimaryExpr();
        break;
    }
  }

  /*primary-expr -> identifier arg-list?
   *             | identifier "[" expr "]"
   *             | "(" expr ")"
   *             | INTLITERAL
   *             | FLOATLITERAL
   *             | BOOLLITERAL
   *             | STRINGLITERAL
   */
  void parsePrimaryExpr() throws SyntaxError {
    switch (currentToken.kind) {
      case Token.ID:
        parseIdent();
        if (currentToken.kind == Token.LPAREN){ // identifier arg-list?
            parseArgList();
        }
        else if (currentToken.kind == Token.LBRACKET) { // identifier "[" expr "]"
            match(Token.LBRACKET);
            parseExpr();
            match(Token.RBRACKET);
        }
        break;
      case Token.LPAREN:
        {
          match(Token.LPAREN);
          parseExpr();
          match(Token.RPAREN);
        }
        break;
      case Token.INTLITERAL:
        parseIntLiteral();
        break;
      case Token.FLOATLITERAL:
        parseFloatLiteral();
        break;
      case Token.BOOLEANLITERAL:
        parseBooleanLiteral();
        break;
      case Token.STRINGLITERAL:
        parseStringLiteral();
        break;
      default:
        syntacticError("illegal parimary expression", currentToken.spelling);
        break;
    }
  }

// ========================== LITERALS ========================

  // Call these methods rather than accept().  In Assignment 3, 
  // literal AST nodes will be constructed inside these methods. 

  void parseIntLiteral() throws SyntaxError {
    if (currentToken.kind == Token.INTLITERAL) {
        currentToken = scanner.getToken();
    } else
      syntacticError("integer literal expected here", "");
  }

  void parseFloatLiteral() throws SyntaxError {
    if (currentToken.kind == Token.FLOATLITERAL) {
        currentToken = scanner.getToken();
    } else
      syntacticError("float literal expected here", "");
  }

  void parseBooleanLiteral() throws SyntaxError {
    if (currentToken.kind == Token.BOOLEANLITERAL) {
        currentToken = scanner.getToken();
    } else
      syntacticError("boolean literal expected here", "");
  }

  void parseStringLiteral() throws SyntaxError {
    if (currentToken.kind == Token.STRINGLITERAL) {
        currentToken = scanner.getToken();
    }
    else
        syntacticError("string literal expected here", "");
  }
// ========================== PARAMETERS ========================

// parameters
//    para-list           -> "(" proper-para-list? ")"
//    proper-para-list    -> para-decl ( "," para-decl )*
//    para-decl           -> type declarator
//    arg-list            -> "(" proper-arg-list? ")"
//    proper-arg-list     -> arg ( "," arg )*
//    arg                 -> expr

  // para-list -> "(" proper-para-list? ")"
  void parseParaList() throws SyntaxError {
    match(Token.LPAREN);
    if (currentToken.kind != Token.RPAREN){
        parseProperParaList();
    }
    match(Token.RPAREN);
  }

  // proper-para-list -> para-decl ( "," para-decl )*
  void parseProperParaList() throws SyntaxError {
      parseParaDecl();
      while (currentToken.kind == Token.COMMA){
          match(Token.COMMA);
          parseParaDecl();
      }
  }

  // para-decl -> type declarator
  void parseParaDecl() throws SyntaxError {
      parseType();
      parseDeclarator();
  }

  // arg-list -> "(" proper-arg-list? ")"
  void parseArgList() throws SyntaxError {
      match(Token.LPAREN);
      if (currentToken.kind != Token.RPAREN){
          parseProperArgList();
      }
      match(Token.RPAREN);
  }

  // proper-arg-list -> arg ( "," arg )*
  void parseProperArgList() throws SyntaxError {
      parseArg();
      while (currentToken.kind == Token.COMMA){
          match(Token.COMMA);
          parseArg();
      }
  }
  // arg -> expr
  void parseArg() throws SyntaxError {
    parseExpr();
  }

// ========================== OTHER ========================
  boolean typeChecker(int type){
    for(int i = 0; i < 4; i++){
        if (type == types[i]){
            return true;
        }
    }
    return false;
  }
}