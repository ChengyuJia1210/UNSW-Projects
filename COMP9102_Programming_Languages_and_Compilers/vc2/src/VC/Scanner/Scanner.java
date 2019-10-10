/**
 **	src.VC.Scanner.java
 **/

package VC.Scanner;

import VC.ErrorReporter;

public final class Scanner { 

  private SourceFile sourceFile;
  private boolean debug;

  private ErrorReporter errorReporter;
  private StringBuffer currentSpelling;
  private char currentChar;
  private SourcePosition sourcePos;

  //----------------------------------------
  private boolean currentScanningToken;
  private int charStart, charFinish;
  private char[] escapeChar = {'b', 'f', 'n', 'r', 't', '\'', '\"', '\\'};
  private char[] escapeAndChar = {'\b', '\f', '\n', '\r', '\t', '\'', '\"', '\\'};
  private String[] keyWordList = {"boolean", "break", "continue", "else", "float",
          "for", "if", "int", "return", "void", "while", "false", "true"};
  private int lineNum;
  //----------------------------------------

// =========================================================

  public Scanner(SourceFile source, ErrorReporter reporter) {
    sourceFile = source;
    errorReporter = reporter;
//    currentChar = sourceFile.getNextChar();
    debug = false;
    lineNum = 1;
//    charFinish = charStart = 1;
    charStart = 0;
    // you may initialise your counters for line and column numbers here
    currentScanningToken = false;
    accept();
  }

  public void reportError(String message, String tokenName, SourcePosition posi){
      errorReporter.reportError(message, tokenName, posi);
  }

  public void enableDebugging() {
    debug = true;
  }

  // accept gets the next character from the source program.

  private void accept() {
    if (currentScanningToken)
        currentSpelling.append(currentChar);

    getNextChar();
  }

  public void getNextChar(){
    currentChar = sourceFile.getNextChar();
    charFinish = charStart;

    if (currentChar == '\n'){
        lineNum++;
        charStart = 0;
    }
    else if (currentChar == '\t'){
        charStart = 8 - charStart%8 + charStart;
    }
    else{
        charStart++;
    }
  }

  private char inspectChar(int nthChar) {
    return sourceFile.inspectChar(nthChar);
  }

  private int nextToken() {
  // Tokens: separators, operators, literals, identifiers and keyworods
    switch (currentChar) {
       // separators 
    case '(':
	accept();
	return Token.LPAREN;
    case ')':
        accept();
        return Token.RPAREN;
    case '{':
        accept();
        return Token.LCURLY;
    case '}':
        accept();
        return Token.RCURLY;
    case '[':
        accept();
        return Token.LBRACKET;
    case ']':
        accept();
        return Token.RBRACKET;
    case ',':
        accept();
        return Token.COMMA;
    case ';':
        accept();
        return Token.SEMICOLON;
    // operators
    case '+':
        accept();
        return Token.PLUS;
    case '-':
        accept();
        return Token.MINUS;
    case '*':
        accept();
        return Token.MULT;
    case '/':
        accept();
        return Token.DIV;
    case '!':
        accept();
        return twoCharOperatorDecision('=', Token.NOTEQ, Token.NOT);
    case '=':
        accept();
        return twoCharOperatorDecision('=', Token.EQEQ, Token.EQ);
    case '<':
        accept();
        return twoCharOperatorDecision('=', Token.LTEQ, Token.LT);
    case '>':
        accept();
        return twoCharOperatorDecision('=', Token.GTEQ, Token.GT);
    case '&':
        accept();
        return twoCharOperatorDecision('&', Token.ANDAND, Token.ERROR);
    case '|':	
       	accept();
        return twoCharOperatorDecision('|', Token.OROR, Token.ERROR);
    case '.':
        accept();
        if (Character.isDigit(currentChar)){
            return isFraction();
        }
        else {
            return Token.ERROR;
        }
    // String
    case '\"':
        getNextChar();
        return isString();
    // ....
    case SourceFile.eof:	
	currentSpelling.append(Token.spell(Token.EOF));
	return Token.EOF;
    default:
	break;
    }
    // Intger
    if(Character.isDigit(currentChar)) {
        accept();
        return isInteger();
    }
    // Identifiers
    else if (checkIdStart()){
        accept();
        return isIdentifier();
    }
    accept(); 
    return Token.ERROR;
  }

  int twoCharOperatorDecision(char secondChar, int ifToken, int elseToken){
      if (currentChar == secondChar) {
          accept();
          return ifToken;
      } else {
          return elseToken;
      }
  }

  int isString(){
      boolean escape = false;
      int pos;

      while (escape || currentChar != '\"'){
          if (currentChar == '\n'){
              sourcePos.charFinish = sourcePos.charStart;
              reportError("%: unterminated string", currentSpelling.toString(), sourcePos);
              return Token.STRINGLITERAL;
          }
          if (escape){
              escape = false;
              pos = escapePosition();
              if (pos >= 0){
                  currentChar = escapeAndChar[pos];
                  accept();
              }
              else {
                  StringBuffer b = new StringBuffer("\\");
                  b.append(currentChar);
                  sourcePos.charFinish = charFinish;
                  reportError("%: illegal escape character", b.toString(), sourcePos);
                  currentSpelling.append('\\');
                  accept();
              }
          }
          else {
              if (currentChar == '\\') {
                  escape = true;
                  getNextChar();
              }
              else {
                  accept();
              }
          }
      }
      getNextChar();
      return Token.STRINGLITERAL;
  }

  int escapePosition(){
      int i;
      for(i = 0; i < 8; i++){
          if (currentChar == escapeChar[i])
              return i;
      }
      return -1;
  }

  boolean checkIdStart(){
      if ((currentChar >= 'A' && currentChar <= 'Z') || (currentChar >= 'a'
              && currentChar <= 'z') || currentChar == '_')
          return true;
      return false;
  }

  boolean checkIdPart(){
    if ((currentChar >= 'A' && currentChar <= 'Z') || (currentChar >= 'a'
            && currentChar <= 'z') || currentChar == '_' || Character.isDigit(currentChar))
        return true;
    return false;
  }

  public int isIdentifier(){
      while (checkIdPart()){
          accept();
      }
      return Token.ID;
  }

  public int isInteger() {
      // has 'e', 'E' or '.', then folat literal
      while (Character.isDigit(currentChar)) {
          accept();
      }
      if (currentChar == '.') {
          accept();
          return isFraction();
      } else if (currentChar == 'E' || currentChar == 'e') {
          return FolatExponent(Token.FLOATLITERAL);
      }
      //else int litreal
      else {
          return Token.INTLITERAL;
      }
  }

  public int isFraction(){
      while (Character.isDigit(currentChar))
          accept();
      if (currentChar == 'E' || currentChar == 'e'){
        return FolatExponent(Token.FLOATLITERAL);
      }
      return Token.FLOATLITERAL;

  }

  public int FolatExponent(int currentToken){
      char next1 = inspectChar(1);
      char next2 = inspectChar(2);

      if (Character.isDigit(next1) ||
              (next1 == '+' || next1 == '-') && Character.isDigit(next2)){
          accept();
          if (currentChar == '+' || currentChar == '-')
              accept();
          while (Character.isDigit(currentChar))
              accept();
          return Token.FLOATLITERAL;
      }
      else
//          return Token.ERROR;//attention!
      return currentToken;
  }

  boolean keyWord(String token, String key){
      if (token.compareTo(key) == 0){
          return true;
      }
      return false;
  }

  int keyWordsCompare(String token, int originalKind){
    for (int i = 0; i < 13; i++){
        if (i < 11 && keyWord(token, keyWordList[i])) {
            return i;
        }
        else if (keyWord(token, keyWordList[i])){
            return Token.BOOLEANLITERAL;
        }
    }
    return originalKind;
  }

  void abitroyComments(int linePos, int charPos){
      while (true){
          if(currentChar == '*' && inspectChar(1) == '/') {
              getNextChar();
              getNextChar();
              if (currentChar == '/' || currentChar <= 32 || currentChar >= 126){
                  skipSpaceAndComments();
              }
              return;
          }
          else if (currentChar == SourceFile.eof){
              sourcePos.charStart = sourcePos.charFinish = charPos;
              sourcePos.lineStart = sourcePos.lineFinish = linePos;
              reportError("%: unterminated comment", "", sourcePos);
              return;
          }
          else {
              getNextChar();
          }
      }
  }

  void twoSlashComment(){
      while (currentChar != '\n'){
          getNextChar();
      }
      getNextChar();
      if (currentChar == '/' || currentChar == '\n' || currentChar == ' '){//2019/3/11,add "'\n'" and "' '" conditions
          skipSpaceAndComments();
      }
  }

  void skipSpaceAndComments() {
      int linePos = lineNum;
      int charPos = charStart;
      char next = inspectChar(1);
      if (currentChar == '/'){
          // 2 slash comment, only one line
          if (next == '/'){
              twoSlashComment();
          }
          // "/**/" can have multiple lines
          else if (next == '*'){
                abitroyComments(linePos, charPos);
          }
      }
      else {
          while ((currentChar <= 32 || currentChar >= 126) && currentChar != SourceFile.eof)
            getNextChar();
          if (currentChar == '/'){
              skipSpaceAndComments();
          }
      }
  }

  public Token getToken() {
    Token tok;
    int kind;

   sourcePos = new SourcePosition();
   skipSpaceAndComments();
   currentSpelling = new StringBuffer("");

   // You must record the position of the current token somehow
   currentScanningToken = true;
   sourcePos.charStart = charStart;
   sourcePos.lineStart = sourcePos.lineFinish = lineNum;
   kind = nextToken();
   if (kind == 39){// attention!!!
       charFinish++;
   }
   sourcePos.charFinish = charFinish;
   currentScanningToken = false;
   kind = keyWordsCompare(currentSpelling.toString(), kind);

   tok = new Token(kind, currentSpelling.toString(), sourcePos);
   // * do not remove these three lines
   if (debug)
     System.out.println(tok);
   return tok;
   }
}