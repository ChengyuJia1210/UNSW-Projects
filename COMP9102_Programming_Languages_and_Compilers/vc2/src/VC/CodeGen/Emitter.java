/*
 * Emitter.java 
 * Sun  7 Apr 23:21:06 AEST 2019
 * Jingling Xue, School of Computer Science, UNSW, Australia
 */

// A new frame object is created for every function just before the
// function is being translated in visitFuncDecl.
//
// All the information about the translation of a function should be
// placed in this Frame object and passed across the AST nodes as the
// 2nd argument of every visitor method in Emitter.java.

package VC.CodeGen;

import java.util.LinkedList;
import java.util.Enumeration;
import java.util.ListIterator;
import java.util.Stack;

import VC.ASTs.*;
import VC.ErrorReporter;
import VC.StdEnvironment;


public final class Emitter implements Visitor {

    private ErrorReporter errorReporter;
    private String inputFilename;
    private String classname;
    private String outputFilename;

    //------------------------------------------------------
    private Stack<String> loopStart = new Stack<String>();
    private Stack<String> loopEnd = new Stack<String>();
    //------------------------------------------------------

    public Emitter(String inputFilename, ErrorReporter reporter) {
        this.inputFilename = inputFilename;
        errorReporter = reporter;
    
        int i = inputFilename.lastIndexOf('.');
        if (i > 0)
            classname = inputFilename.substring(0, i);
        else
            classname = inputFilename;
    
    }

    // PRE: ast must be a Program node

    public final void gen(AST ast) {
        ast.visit(this, null);
        JVM.dump(classname + ".j");
    }
    
    // =============================================Programs========================================
    public Object visitProgram(Program ast, Object o) {
        /** This method works for scalar variables only. You need to modify
         it to handle all array-related declarations and initialisations.
         **/

        // Generates the default constructor initialiser
        emit(JVM.CLASS, "public", classname);
        emit(JVM.SUPER, "java/lang/Object");

        emit("");

        // Three subpasses:

        // (1) Generate .field definition statements since
        //     these are required to appear before method definitions
        List list = ast.FL;
        while (!list.isEmpty()) {
            DeclList dlAST = (DeclList) list;
            if (dlAST.D instanceof GlobalVarDecl) {
                GlobalVarDecl vAST = (GlobalVarDecl) dlAST.D;
                emit(JVM.STATIC_FIELD, vAST.I.spelling, VCtoJavaType(vAST.T));
            }
            list = dlAST.DL;
        }

        emit("");

        // (2) Generate <clinit> for global variables (assumed to be static)
 
        emit("; standard class static initializer ");
        emit(JVM.METHOD_START, "static <clinit>()V");
        emit("");

        // create a Frame for <clinit>

        Frame frame = new Frame(false);

        list = ast.FL;
        while (!list.isEmpty()) {
            DeclList dlAST = (DeclList) list;
            if (dlAST.D instanceof GlobalVarDecl) {
                GlobalVarDecl vAST = (GlobalVarDecl) dlAST.D;

                if (vAST.T instanceof ArrayType){
                    ArrayType arrayType = (ArrayType) vAST.T;
                    if (!vAST.E.isEmptyExpr()) {
                        IntExpr range = (IntExpr) arrayType.E;
                        range.visit(this, frame); // emit iconst range and push into stack
                        frame.pop();// pop the range
                        String at = VCtoArrayType(arrayType.toString());
                        emit("newarray", at);
                        frame.push(); // ***push this newarray into the stack
                        processArray(vAST.E, frame); // initializer is not empty so put element into the slots
                    }
                    else{
                        // no initializer part so we initialize all the slots as 0(or 0.0)
                        // and the pop would be done at the end of this "if else" compound statement
                        localAndParaArrayDecl(arrayType, frame);
                    }
                    emitPUTSTATIC(vAST.T.toString(), vAST.I.spelling); // store initializer part into the gloabal variable
                }
                else {
                    if (!vAST.E.isEmptyExpr()) {
                        vAST.E.visit(this, frame);
                    } else {
                        // if code does initialize the global variable, then auto initialize 0 or 0.0
                        if (vAST.T.equals(StdEnvironment.floatType))
                            emit(JVM.FCONST_0);
                        else
                            emit(JVM.ICONST_0);
                        frame.push();
                    }
                    emitPUTSTATIC(VCtoJavaType(vAST.T), vAST.I.spelling);
                }
                frame.pop();
            }
            list = dlAST.DL;
        }
   
        emit("");
        emit("; set limits used by this method");
        emit(JVM.LIMIT, "locals", frame.getNewIndex());

        emit(JVM.LIMIT, "stack", frame.getMaximumStackSize());
        emit(JVM.RETURN);
        emit(JVM.METHOD_END, "method");

        emit("");

        // (3) Generate Java bytecode for the VC program

        emit("; standard constructor initializer ");
        emit(JVM.METHOD_START, "public <init>()V");
        emit(JVM.LIMIT, "stack 1");
        emit(JVM.LIMIT, "locals 1");
        emit(JVM.ALOAD_0);
        emit(JVM.INVOKESPECIAL, "java/lang/Object/<init>()V");
        emit(JVM.RETURN);
        emit(JVM.METHOD_END, "method");

        return ast.FL.visit(this, o);
    }

    // =====================================Declarations=============================================

    public Object visitDeclList(DeclList ast, Object o) {
        ast.D.visit(this, o);
        ast.DL.visit(this, o);
        return null;
    }

    public Object visitEmptyDeclList(EmptyDeclList ast, Object o) {
        return null;
    }

    public Object visitFuncDecl(FuncDecl ast, Object o) {

        Frame frame;

        if (ast.I.spelling.equals("main")) {

            frame = new Frame(true);

            // Assume that main has one String parameter and reserve 0 for it
            frame.getNewIndex();

            emit(JVM.METHOD_START, "public static main([Ljava/lang/String;)V");
            // Assume implicitly that
            //      classname vc$;
            // appears before all local variable declarations.
            // (1) Reserve 1 for this object reference.

            frame.getNewIndex();

        } else {

            frame = new Frame(false);

            // all other programmer-defined functions are treated as if
            // they were instance methods
            frame.getNewIndex(); // reserve 0 for "this"

            String retType = VCtoJavaType(ast.T);

            // The types of the parameters of the called function are not
            // directly available in the FuncDecl node but can be gathered
            // by traversing its field PL.

            StringBuffer argsTypes = new StringBuffer("");
            List fpl = ast.PL;
            while (! fpl.isEmpty()) {
                // split normal var and arrayVar
                if (!(((ParaList) fpl).P.T instanceof ArrayType)) { // add at 23/April/10:45am
                    if (((ParaList) fpl).P.T.equals(StdEnvironment.booleanType))
                        argsTypes.append("Z");
                    else if (((ParaList) fpl).P.T.equals(StdEnvironment.intType))
                        argsTypes.append("I");
                    else
                        argsTypes.append("F");
                }
                else {
                    argsTypes.append(((ParaList) fpl).P.T.toString());
                }
                fpl = ((ParaList) fpl).PL;
            }

            emit(JVM.METHOD_START, ast.I.spelling + "(" + argsTypes + ")" + retType);
        }

        ast.S.visit(this, frame);

        // JVM requires an explicit return in every method.
        // In VC, a function returning void may not contain a return, and
        // a function returning int or float is not guaranteed to contain
        // a return. Therefore, we add one at the end just to be sure.

        if (ast.T.equals(StdEnvironment.voidType)) {
            emit("");
            emit("; return may not be present in a VC function returning void");
            emit("; The following return inserted by the VC compiler");
            emit(JVM.RETURN);
        } else if (ast.I.spelling.equals("main")) {
            // In case VC's main does not have a return itself
            emit(JVM.RETURN);
        } else
            emit(JVM.NOP);

        emit("");
        emit("; set limits used by this method");
        emit(JVM.LIMIT, "locals", frame.getNewIndex());

        emit(JVM.LIMIT, "stack", frame.getMaximumStackSize());
        emit(".end method");

//        System.out.println("------------currentStack: " + frame.getCurStackSize() +
//                ", maxStackSize: " + frame.getMaximumStackSize());
        return null;
    }

    public Object visitGlobalVarDecl(GlobalVarDecl ast, Object o) {
        // nothing to be done
        return null;
    }

    public Object visitLocalVarDecl(LocalVarDecl ast, Object o) {
        Frame frame = (Frame) o;
        ast.index = frame.getNewIndex();
        String T;
        // split normal var and arrayVar 23/April/11:00am
        if (ast.T instanceof ArrayType){
            T = ast.T.toString();
        }
        else {
            T = VCtoJavaType(ast.T);
        }

        emit(JVM.VAR + " " + ast.index + " is " + ast.I.spelling + " " + T + " from " + (String) frame.scopeStart.peek() + " to " +  (String) frame.scopeEnd.peek());

        if (!ast.E.isEmptyExpr()) {
            if (ast.T instanceof ArrayType){ // if array type emit newarray, 23/April/11:30am
                IntExpr range = (IntExpr) ((ArrayType) ast.T).E;
                range.visit(this, o); // emit iconst and push into stack
                frame.pop();// pop the range
                String arrayType = VCtoArrayType(ast.T.toString());
                emit("newarray", arrayType);
                frame.push(); // ***push this newarray into the stack

                processArray(ast.E, o); // process the initializer part
                frame.pop(); // ***after insertion pop this newarray into the stack
                emitASTORE(ast.index);
            }
            else { // 23/April/12:12pm
                ast.E.visit(this, o);

                if (ast.T.equals(StdEnvironment.floatType)) {
                    // cannot call emitFSTORE(ast.I) since this I is not an
                    // applied occurrence
                    if (ast.index >= 0 && ast.index <= 3)
                        emit(JVM.FSTORE + "_" + ast.index);
                    else
                        emit(JVM.FSTORE, ast.index);
                    frame.pop();
                } else { // intType and booleanType
                    // cannot call emitISTORE(ast.I) since this I is not an
                    // applied occurrence
                    if (ast.index >= 0 && ast.index <= 3)
                        emit(JVM.ISTORE + "_" + ast.index);
                    else
                        emit(JVM.ISTORE, ast.index);
                    frame.pop();
                }
            }
        }
        else{//there may have an array, but without initilized, e.g. int a[10];  23/April/12:50pm
            if (ast.T instanceof ArrayType){
                localAndParaArrayDecl((ArrayType) ast.T, o);//, ast.index);
                frame.pop(); // ***after insertion pop this newarray into the stack
                emitASTORE(ast.index);
            }
            // normal variable
            else {
                // if code doesn't initialize the local variable, then auto initialize 0 or 0.0
                if (ast.T.equals(StdEnvironment.floatType)) {
                    emit(JVM.FCONST_0);
                    emit(JVM.FSTORE, ast.index);
                }
                else {
                    emit(JVM.ICONST_0);
                    emit(JVM.ISTORE, ast.index);
                }
            }
        }

        return null;
    }

    // process the initializer part
    public void processArray(Expr expr, Object o){
        Frame frame = (Frame) o;
        // 24/April/11:00am
        // vist initExpr by hand(while)
        InitExpr initExpr = (InitExpr) expr;
        List il = initExpr.IL;
        int counter = 0;
        while (!il.isEmpty()){
            ExprList exprList = (ExprList) il;
            emit(JVM.DUP);
            frame.push(); // push the arrayref
            emitICONST(counter);
            frame.push(); // push the counter
            exprList.E.visit(this, o); // in this retrival the element is pushed into the stack
            frame.pop(); // pop the element
            frame.pop(); // pop the counter
            frame.pop(); // pop the arrayref
            putEleIntoArray(exprList.E, (Frame) o);

            il = exprList.EL;
            counter++;
        }

    }


    public String VCtoArrayType(String T){
        switch (T){
        case "[I":
            return "int";
        case "[F":
            return "float";
        default:
            return "boolean";
        }
    }

    // ===========================================Statements========================================

    public Object visitCompoundStmt(CompoundStmt ast, Object o) {
        Frame frame = (Frame) o;

        String scopeStart = frame.getNewLabel();
        String scopeEnd = frame.getNewLabel();
        frame.scopeStart.push(scopeStart);
        frame.scopeEnd.push(scopeEnd);
   
        emit(scopeStart + ":");
        if (ast.parent instanceof FuncDecl) {
            if (((FuncDecl) ast.parent).I.spelling.equals("main")) {
                emit(JVM.VAR, "0 is argv [Ljava/lang/String; from " + (String) frame.scopeStart.peek() + " to " +  (String) frame.scopeEnd.peek());
                emit(JVM.VAR, "1 is vc$ L" + classname + "; from " + (String) frame.scopeStart.peek() + " to " +  (String) frame.scopeEnd.peek());
                // Generate code for the initialiser vc$ = new classname();
                emit(JVM.NEW, classname);
                emit(JVM.DUP);
                frame.push(2);
                emit("invokenonvirtual", classname + "/<init>()V");
                frame.pop();
                emit(JVM.ASTORE_1);
                frame.pop();
            } else {
                emit(JVM.VAR, "0 is this L" + classname + "; from " + (String) frame.scopeStart.peek() + " to " +  (String) frame.scopeEnd.peek());
                ((FuncDecl) ast.parent).PL.visit(this, o);
            }
        }
        ast.DL.visit(this, o);
        ast.SL.visit(this, o);
        emit(scopeEnd + ":");

        frame.scopeStart.pop();
        frame.scopeEnd.pop();

        return null;
    }

    public Object visitStmtList(StmtList ast, Object o) {
        ast.S.visit(this, o);
        ast.SL.visit(this, o);
        return null;
    }
    @Override
    public Object visitIfStmt(IfStmt ast, Object o) {
        Frame frame = (Frame) o;

        if (!(ast.S2 instanceof EmptyStmt)) {
            String elseLable = frame.getNewLabel(); // L2
            String nextLable = frame.getNewLabel(); // L3

            ast.E.visit(this, o);
            frame.pop(); // pop boolean value in the operand stack
            String opcode = JVM.IFEQ;
            emit(opcode, elseLable);
            ast.S1.visit(this, o);
            emit("goto", nextLable);
            emit(elseLable + ":");
            ast.S2.visit(this, o);
            emit(nextLable + ":");
        }
        else{
            String nextLable = frame.getNewLabel(); // L2
            ast.E.visit(this, o);
            frame.pop(); // pop boolean value in the operand stack
            String opcode = JVM.IFEQ;
            emit(opcode, nextLable);
            ast.S1.visit(this, o);
            emit(nextLable + ":");
        }
        return null;
    }

    @Override
    public Object visitWhileStmt(WhileStmt ast, Object o) {
        //-------------------------------
        Frame frame = (Frame) o;
        String whileLable = frame.getNewLabel(); // L2
        String nextLable = frame.getNewLabel();   // L3
        loopStart.push(whileLable);
        loopEnd.push(nextLable);

        emit(whileLable + ":");
        //---------------------------------------

        ast.E.visit(this, o);
        frame.pop();
        String opcode = JVM.IFEQ;

        emit(opcode, nextLable);
        ast.S.visit(this, o);
//        frame.pop();
        emit("goto", whileLable);

        //---------------------------------------------------
        emit(nextLable + ":");
        loopStart.pop();
        loopEnd.pop();
        //---------------------------------------------------
        return null;
    }

    @Override
    public Object visitForStmt(ForStmt ast, Object o) {
        Frame frame = (Frame) o;

        String forLable = frame.getNewLabel();  // L2
        String nextLable = frame.getNewLabel(); // L3
        loopStart.push(forLable);
        loopEnd.push(nextLable);

        int before = frame.getCurStackSize();
        ast.E1.visit(this, o);
        int after = frame.getCurStackSize();
        if (before < after){
            int gap = after - before;
            for (int i = 0; i < gap; i++) {
                frame.pop();
                emit(JVM.POP);
            }
        }

        emit(forLable + ":");
        ast.E2.visit(this, o);
        frame.pop(); // pop boolean value to cmp with 0
        String opcode = JVM.IFEQ;
        emit(opcode, nextLable);
        ast.S.visit(this, o);
        ast.E3.visit(this, o); // attention, if  ++i or i++ the would get one more ele in the stack, 24/April
        emit("goto", forLable);
        emit(nextLable + ":");

        loopStart.pop();
        loopEnd.pop();
        return null;
    }

    @Override
    public Object visitBreakStmt(BreakStmt ast, Object o) {
        String nextLable = loopEnd.peek();
        emit("goto", nextLable);
        return null;
    }

    @Override
    public Object visitContinueStmt(ContinueStmt ast, Object o) {
        String loopStartLable = loopStart.peek();
        emit("goto", loopStartLable);
        return null;
    }

    @Override
    public Object visitExprStmt(ExprStmt ast, Object o) {
        Frame frame = (Frame) o;
        // star compoundStmt we record the currentStackSize,
        // in order to keep the current Stack Size same
        // at the end of the compoundStmt
        int startcurrStackSize = frame.getCurStackSize();

        ast.E.visit(this, o);

        int endcurrStackSize = frame.getCurStackSize();
        if (endcurrStackSize > startcurrStackSize){
            int gap = endcurrStackSize - startcurrStackSize;
            frame.pop(gap);
            // pop the residual element in the operand stack
            for (int i = 0; i < gap; i++){
                emit(JVM.POP);
            }
        }
        return null;
    }

    public Object visitReturnStmt(ReturnStmt ast, Object o) {
        Frame frame = (Frame)o;

        /*
        * int main() { return 0; } must be interpretted as
        * public static void main(String[] args) { return ; }
        * Therefore, "return expr", if present in the main of a VC program
        * must be translated into a RETURN rather than IRETURN instruction.
        */

        if (frame.isMain())  {
            emit(JVM.RETURN);
            return null;
        }

        // Your other code goes here
        ast.E.visit(this, o);
        if (ast.E.type.isIntType() || ast.E.type.isBooleanType()){
            emit(JVM.IRETURN);
        }
        else {
            emit(JVM.FRETURN);
        }
        ((Frame)o).pop();
        return null;
    }

    public Object visitEmptyStmtList(EmptyStmtList ast, Object o) {
        return null;
    }

    @Override
    public Object visitEmptyExprList(EmptyExprList ast, Object o) {
        return null;
    }

    public Object visitEmptyCompStmt(EmptyCompStmt ast, Object o) {
        return null;
    }

    public Object visitEmptyStmt(EmptyStmt ast, Object o) {
        return null;
    }

    // =========================================Expressions=========================================

    public Object visitCallExpr(CallExpr ast, Object o) {
        int countArray = 0;

        Frame frame = (Frame) o;
        String fname = ast.I.spelling;

        if (fname.equals("getInt")) {
            ast.AL.visit(this, o); // push args (if any) into the op stack
            emit("invokestatic VC/lang/System.getInt()I");
            frame.push();
        } else if (fname.equals("putInt")) {
            ast.AL.visit(this, o); // push args (if any) into the op stack
            emit("invokestatic VC/lang/System.putInt(I)V");
            frame.pop();
        } else if (fname.equals("putIntLn")) {
            ast.AL.visit(this, o); // push args (if any) into the op stack
            emit("invokestatic VC/lang/System/putIntLn(I)V");
            frame.pop();
        } else if (fname.equals("getFloat")) {
            ast.AL.visit(this, o); // push args (if any) into the op stack
            emit("invokestatic VC/lang/System/getFloat()F");
            frame.push();
        } else if (fname.equals("putFloat")) {
            ast.AL.visit(this, o); // push args (if any) into the op stack
            emit("invokestatic VC/lang/System/putFloat(F)V");
            frame.pop();
        } else if (fname.equals("putFloatLn")) {
            ast.AL.visit(this, o); // push args (if any) into the op stack
            emit("invokestatic VC/lang/System/putFloatLn(F)V");
            frame.pop();
        } else if (fname.equals("putBool")) {
            ast.AL.visit(this, o); // push args (if any) into the op stack
            emit("invokestatic VC/lang/System/putBool(Z)V");
            frame.pop();
        } else if (fname.equals("putBoolLn")) {
            ast.AL.visit(this, o); // push args (if any) into the op stack
            emit("invokestatic VC/lang/System/putBoolLn(Z)V");
            frame.pop();
        } else if (fname.equals("putString")) {
            ast.AL.visit(this, o);
            emit(JVM.INVOKESTATIC, "VC/lang/System/putString(Ljava/lang/String;)V");
            frame.pop();
        } else if (fname.equals("putStringLn")) {
            ast.AL.visit(this, o);
            emit(JVM.INVOKESTATIC, "VC/lang/System/putStringLn(Ljava/lang/String;)V");
            frame.pop();
        } else if (fname.equals("putLn")) {
            ast.AL.visit(this, o); // push args (if any) into the op stack
            emit("invokestatic VC/lang/System/putLn()V");
        } else { // programmer-defined functions

            FuncDecl fAST = (FuncDecl) ast.I.decl;

            // all functions except main are assumed to be instance methods
            if (frame.isMain())
                emit("aload_1"); // vc.funcname(...)
            else
                emit("aload_0"); // this.funcname(...)
            frame.push();

            ast.AL.visit(this, o);
    
            String retType = VCtoJavaType(fAST.T);
      
            // The types of the parameters of the called function are not
            // directly available in the FuncDecl node but can be gathered
            // by traversing its field PL.

            StringBuffer argsTypes = new StringBuffer("");
            List fpl = fAST.PL;
            while (! fpl.isEmpty()) {
                // split normal var and arrayVar
                if (!(((ParaList) fpl).P.T instanceof ArrayType)) { // add at 23/April/10:45am
                    if (((ParaList) fpl).P.T.equals(StdEnvironment.booleanType))
                        argsTypes.append("Z");
                    else if (((ParaList) fpl).P.T.equals(StdEnvironment.intType))
                        argsTypes.append("I");
                    else
                        argsTypes.append("F");
                }
                else {
                    countArray++;
                    argsTypes.append(((ParaList) fpl).P.T.toString());
                }
                fpl = ((ParaList) fpl).PL;
            }
      
            emit("invokevirtual", classname + "/" + fname + "(" + argsTypes + ")" + retType);
//            frame.pop(argsTypes.length() + 1);

            // cause arrayType length is 2, so we avoid minus 2 instead minus 1
            frame.pop(argsTypes.length() + 1 - countArray);

            if (! retType.equals("V"))
                frame.push();
        }
        return null;
    }

    @Override
    public Object visitAssignExpr(AssignExpr ast, Object o) {
        ast.E2.visit(this, o); // visit right expr first get the value then assign to left var
        if (!(ast.parent instanceof ExprStmt || ast.parent instanceof ForStmt
                || ast.parent instanceof IfStmt || ast.parent instanceof WhileStmt
                || ast.parent instanceof ReturnStmt)) {
            emit(JVM.DUP);
            ((Frame) o).push();
        }
        // cause assignment left element must be variable, so we do this
        Var var = ((VarExpr)ast.E1).V;
        SimpleVar simpleVar = (SimpleVar) var;
        Ident ident = simpleVar.I;
        assignIdent(ident, o); // equal to "ast.E1.visit(this, o);"
        return null;
    }

    //---------------------Assignment Identifier define------------------------
    public Object assignIdent(Ident ast, Object o) {
        ((Frame) o).pop(); // pop right value of the equal sign from the operand stack
        // In the assignment, we don't really need to care about left variable's value,
        // so no need to trival.
        Type type = ((Decl)ast.decl).T;
        if (ast.decl instanceof GlobalVarDecl){
            String T = VCtoJavaType(type);
            String I = ((GlobalVarDecl) ast.decl).I.spelling;
            emitPUTSTATIC(T, I);
        }
        else {
            if (type.isIntType()) {
                emitISTORE(ast);
            }
            else if (type.isFloatType()) {
                emitFSTORE(ast);
            }
            else if (type.isBooleanType()){
                emitISTORE(ast);
            }
            // assignment can not assign for the array
//            else if (type.isArrayType()){
//
//            }
        }
        return null;
    }

    //-------------------------------------------------------------------------


    public Object visitEmptyExpr(EmptyExpr ast, Object o) {
        return null;
    }

    public Object visitIntExpr(IntExpr ast, Object o) {
        ast.IL.visit(this, o);
        return null;
    }

    public Object visitFloatExpr(FloatExpr ast, Object o) {
        ast.FL.visit(this, o);
        return null;
    }

    public Object visitBooleanExpr(BooleanExpr ast, Object o) {
        ast.BL.visit(this, o);
        return null;
    }

    public Object visitStringExpr(StringExpr ast, Object o) {
        ast.SL.visit(this, o);
        return null;
    }

    // unary need to consider the ++i;
    @Override
    public Object visitUnaryExpr(UnaryExpr ast, Object o) {
        Operator operator = ast.O;
        ast.E.visit(this, o);
        signOfUnary(operator.spelling, ast.E.type, (Frame) o);
        return null;
    }

    @Override
    public Object visitBinaryExpr(BinaryExpr ast, Object o) {
        Operator operator = ast.O;
        Frame frame = (Frame) o;
        String sign = operator.spelling.substring(0, 1);
        String op = operator.spelling.substring(1, operator.spelling.length());

//        ast.E1.visit(this, o);
//        ast.E2.visit(this, o);

        // ==, !=, >, >=, <, <=
        if (relationalChecker(op)) {
            ast.E1.visit(this, o);
            ast.E2.visit(this, o);
            // int
            if (sign.equals("i")) {
                emitIF_ICMPCOND(operator.spelling, (Frame) o);
            }
            // float
            else {
                emitFCMP(operator.spelling, (Frame) o);
            }
        }
        // +, -, *, /
        else if (binaryChecker(op)){
            ast.E1.visit(this, o);
            ast.E2.visit(this, o);
            binaryEmit(o, sign, op);
        }
        // &&, ||
        else {
            if (operator.spelling.equals("i&&")){
                andAnd((Frame)o, ast.E1, ast.E2);
            }
            else{ // (operator.spelling.equals("i||"))
                orOr((Frame)o, ast.E1, ast.E2);
            }
        }
        return null;
    }

    @Override
    public Object visitInitExpr(InitExpr ast, Object o) {
        return null;
    }

    // this function only be used in the array initialization
    @Override
    public Object visitExprList(ExprList ast, Object o) {
        return null;
    }

    // when we invok array in the expression, we use this
    @Override
    public Object visitArrayExpr(ArrayExpr ast, Object o) {
        ast.V.visit(this, o);
        ast.E.visit(this, o);
        ((Frame) o).pop();
        return null;
    }

    @Override
    public Object visitVarExpr(VarExpr ast, Object o) {
        ast.V.visit(this, o);
        return null;
    }

    // ==========================================Parameters=========================================

    public Object visitParaList(ParaList ast, Object o) {
        ast.P.visit(this, o);
        ast.PL.visit(this, o);
        return null;
    }

    public Object visitParaDecl(ParaDecl ast, Object o) {
        Frame frame = (Frame) o;
        ast.index = frame.getNewIndex();
        //String T = VCtoJavaType(ast.T);
        String T;
        // split normal var and arrayVar 23/April/11:20am
        if (ast.T instanceof ArrayType){
            T = ast.T.toString();
        }
        else {
            T = VCtoJavaType(ast.T);
        }

        emit(JVM.VAR + " " + ast.index + " is " + ast.I.spelling + " " + T + " from " + (String) frame.scopeStart.peek() + " to " +  (String) frame.scopeEnd.peek());

        // 24/April/15:55pm   paraDecl array define;
        if (ast.T instanceof ArrayType){
            localAndParaArrayDecl((ArrayType) ast.T, o);//, ast.index);
            frame.pop(); // ***after insertion pop this newarray into the stack
            emitASTORE(ast.index);
        }
        // normal variable
        else {
            // if code doesn't initialize the parameter variable, then auto initialize 0 or 0.0
            if (ast.T.equals(StdEnvironment.floatType)) {
                emit(JVM.FCONST_0);
                emit(JVM.FSTORE, ast.index);
            }
            else {
                emit(JVM.ICONST_0);
                emit(JVM.ISTORE, ast.index);
            }
        }

        return null;
    }

    public Object visitEmptyParaList(EmptyParaList ast, Object o) {
        return null;
    }

    // Arguments

    public Object visitArgList(ArgList ast, Object o) {
        ast.A.visit(this, o);
        ast.AL.visit(this, o);
        return null;
    }

    public Object visitArg(Arg ast, Object o) {
        ast.E.visit(this, o);
        return null;
    }

    public Object visitEmptyArgList(EmptyArgList ast, Object o) {
        return null;
    }

    // =============================================Types===========================================

    public Object visitIntType(IntType ast, Object o) {
        return null;
    }

    public Object visitFloatType(FloatType ast, Object o) {
        return null;
    }

    @Override
    public Object visitStringType(StringType ast, Object o) {
        return null;
    }

    @Override
    public Object visitArrayType(ArrayType ast, Object o) {
        ast.T.visit(this, o);
        ast.E.visit(this, o);
        return null;
    }

    public Object visitBooleanType(BooleanType ast, Object o) {
        return null;
    }

    public Object visitVoidType(VoidType ast, Object o) {
        return null;
    }

    public Object visitErrorType(ErrorType ast, Object o) {
        return null;
    }

    // ======================================Literals, Identifiers and Operators======================
    // variable only have int, float, boolean, array those kinds of type.
    // This function is only used to get the variable which is defined already.
    public Object visitIdent(Ident ast, Object o) {
        Type type = ((Decl)ast.decl).T;
        int index = ((Decl)ast.decl).index;
        if (ast.decl instanceof GlobalVarDecl){
            String T;
            if (type.isArrayType()){
                T = type.toString();
            }
            else {
                T = VCtoJavaType(type);
            }
            String I = ((GlobalVarDecl) ast.decl).I.spelling;
            emitGETSTATIC(T, I);
        }
        else {
            if (type.isIntType()) {
                emitILOAD(index);
            }
            else if (type.isFloatType()) {
                emitFLOAD(index);
            }
            else if (type.isBooleanType()) {
                emitILOAD(index);
            }
            else if (type.isArrayType()) {
                ArrayType arrayType = (ArrayType) type;
                Type at = arrayType.T;

                if (at.isIntType()) {// each time load arrayref from the local variable;
                    emitIALOAD(index);
                }
                else if (at.isFloatType()){
                    emitFALOAD(index);
                }
                else {
                    emitBALOAD(index);
                }
            }
        }
        ((Frame) o).push();
        return null;
    }


    public Object visitIntLiteral(IntLiteral ast, Object o) {
        Frame frame = (Frame) o;
        emitICONST(Integer.parseInt(ast.spelling));
        frame.push();
        return null;
    }

    public Object visitFloatLiteral(FloatLiteral ast, Object o) {
        Frame frame = (Frame) o;
        emitFCONST(Float.parseFloat(ast.spelling));
        frame.push();
        return null;
    }

    public Object visitBooleanLiteral(BooleanLiteral ast, Object o) {
        Frame frame = (Frame) o;
        emitBCONST(ast.spelling.equals("true"));
        frame.push();
        return null;
    }

    public Object visitStringLiteral(StringLiteral ast, Object o) {
        Frame frame = (Frame) o;
        emit(JVM.LDC, "\"" + ast.spelling + "\"");
        frame.push();
        return null;
    }

    public Object visitOperator(Operator ast, Object o) {
        return null;
    }

    // ===========================================Variables=========================================

    public Object visitSimpleVar(SimpleVar ast, Object o) {
        ast.I.visit(this, o);
        return null;
    }

    // ========================Auxiliary methods for byte code generation===========================

    // The following method appends an instruction directly into the JVM
    // Code Store. It is called by all other overloaded emit methods.

    private void emit(String s) {
        JVM.append(new Instruction(s));
    }

    private void emit(String s1, String s2) {
        emit(s1 + " " + s2);
    }

    private void emit(String s1, int i) {
        emit(s1 + " " + i);
    }

    private void emit(String s1, float f) {
        emit(s1 + " " + f);
    }

    private void emit(String s1, String s2, int i) {
        emit(s1 + " " + s2 + " " + i);
    }

    private void emit(String s1, String s2, String s3) {
        emit(s1 + " " + s2 + " " + s3);
    }

    private void emitIF_ICMPCOND(String op, Frame frame) {
        String opcode;

        if (op.equals("i!="))
            opcode = JVM.IF_ICMPNE;
        else if (op.equals("i=="))
            opcode = JVM.IF_ICMPEQ;
        else if (op.equals("i<"))
            opcode = JVM.IF_ICMPLT;
        else if (op.equals("i<="))
            opcode = JVM.IF_ICMPLE;
        else if (op.equals("i>"))
            opcode = JVM.IF_ICMPGT;
        else // if (op.equals("i>="))
            opcode = JVM.IF_ICMPGE;

        String falseLabel = frame.getNewLabel();
        String nextLabel = frame.getNewLabel();

        emit(opcode, falseLabel);
        frame.pop(2);
        emit("iconst_0");
        emit("goto", nextLabel);
        emit(falseLabel + ":");
        emit(JVM.ICONST_1);
        frame.push();
        emit(nextLabel + ":");
    }

    private void emitFCMP(String op, Frame frame) {
        String opcode;

        if (op.equals("f!="))
            opcode = JVM.IFNE;
        else if (op.equals("f=="))
            opcode = JVM.IFEQ;
        else if (op.equals("f<"))
            opcode = JVM.IFLT;
        else if (op.equals("f<="))
            opcode = JVM.IFLE;
        else if (op.equals("f>"))
            opcode = JVM.IFGT;
        else // if (op.equals("f>="))
            opcode = JVM.IFGE;

        String falseLabel = frame.getNewLabel();
        String nextLabel = frame.getNewLabel();

        emit(JVM.FCMPG);
        frame.pop(2);
        emit(opcode, falseLabel);
        emit(JVM.ICONST_0);
        emit("goto", nextLabel);
        emit(falseLabel + ":");
        emit(JVM.ICONST_1);
        frame.push();
        emit(nextLabel + ":");

    }

    private void emitILOAD(int index) {
        if (index >= 0 && index <= 3)
            emit(JVM.ILOAD + "_" + index);
        else
            emit(JVM.ILOAD, index);
    }

    private void emitFLOAD(int index) {
        if (index >= 0 && index <= 3)
            emit(JVM.FLOAD + "_"  + index);
        else
            emit(JVM.FLOAD, index);
    }

    private void emitGETSTATIC(String T, String I) {
        emit(JVM.GETSTATIC, classname + "/" + I, T);
    }

    private void emitISTORE(Ident ast) {
        int index;
        if (ast.decl instanceof ParaDecl)
            index = ((ParaDecl) ast.decl).index;
        else
            index = ((LocalVarDecl) ast.decl).index;

        if (index >= 0 && index <= 3)
            emit(JVM.ISTORE + "_" + index);
        else
            emit(JVM.ISTORE, index);
    }

    private void emitFSTORE(Ident ast) {
        int index;
        if (ast.decl instanceof ParaDecl)
            index = ((ParaDecl) ast.decl).index;
        else
            index = ((LocalVarDecl) ast.decl).index;
        if (index >= 0 && index <= 3)
            emit(JVM.FSTORE + "_" + index);
        else
            emit(JVM.FSTORE, index);
    }

    private void emitPUTSTATIC(String T, String I) {
        emit(JVM.PUTSTATIC, classname + "/" + I, T);
    }

    private void emitICONST(int value) {
        if (value == -1)
            emit(JVM.ICONST_M1);
        else if (value >= 0 && value <= 5)
            emit(JVM.ICONST + "_" + value);
        else if (value >= -128 && value <= 127)
            emit(JVM.BIPUSH, value);
        else if (value >= -32768 && value <= 32767)
            emit(JVM.SIPUSH, value);
        else
            emit(JVM.LDC, value);
    }

    private void emitFCONST(float value) {
        if(value == 0.0)
            emit(JVM.FCONST_0);
        else if(value == 1.0)
            emit(JVM.FCONST_1);
        else if(value == 2.0)
            emit(JVM.FCONST_2);
        else
            emit(JVM.LDC, value);
    }

    private void emitBCONST(boolean value) {
        if (value)
            emit(JVM.ICONST_1);
        else
            emit(JVM.ICONST_0);
    }

    private String VCtoJavaType(Type t) {
        if (t.equals(StdEnvironment.booleanType))
            return "Z";
        else if (t.equals(StdEnvironment.intType))
            return "I";
        else if (t.equals(StdEnvironment.floatType))
            return "F";
        else // if (t.equals(StdEnvironment.voidType))
            return "V";
    }

    // =========================================aux===============================================

    public boolean relationalChecker(String operator){
        switch (operator){
        case "!=":
            return true;
        case "==":
            return true;
        case "<":
            return true;
        case "<=":
            return true;
        case ">":
            return true;
        case ">=":
            return true;
        default:
            return false;
        }
    }

    public boolean binaryChecker(String operator){
        switch (operator){
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

    public void binaryEmit(Object o, String sign, String op){
        String opcode;
        // intger binary operation
        if (sign.equals("i")){
            if (op.equals("+")){
                opcode = JVM.IADD;
            }
            else if (op.equals("-")){
                opcode = JVM.ISUB;
            }
            else if (op.equals("*")){
                opcode = JVM.IMUL;
            }
            else { // if (op.equals("/"))
                opcode = JVM.IDIV;
            }
        }
        // float binary operation
        else{
            if (op.equals("+")){
                opcode = JVM.FADD;
            }
            else if (op.equals("-")){
                opcode = JVM.FSUB;
            }
            else if (op.equals("*")){
                opcode = JVM.FMUL;
            }
            else { // if (op.equals("/"))
                opcode = JVM.FDIV;
            }
        }

        ((Frame)o).pop(2);
        emit(opcode);
        ((Frame)o).push();
    }

    public void andAnd(Frame frame, Expr expr1, Expr expr2){
        String falseLabel = frame.getNewLabel();
        String nextLabel = frame.getNewLabel();

        expr1.visit(this, frame);
        frame.pop();

        emit(JVM.IFEQ, falseLabel);
        expr2.visit(this, frame);
        frame.pop();

        emit(JVM.IFEQ, falseLabel);
        emit(JVM.ICONST_1);
        emit(JVM.GOTO, nextLabel);
        emit(falseLabel + ":");
        emit(JVM.ICONST_0);// push iconst_0 or iconst_1
        frame.push();

        emit(nextLabel + ":");
    }

    public void orOr(Frame frame, Expr expr1, Expr expr2){
        String trueLable = frame.getNewLabel();
        String nextLable = frame.getNewLabel();

        expr1.visit(this, frame);
        frame.pop();

        emit(JVM.IFNE, trueLable); //emit(JVM.IFGE, trueLable);
        expr2.visit(this, frame);
        frame.pop();

        emit(JVM.IFNE, trueLable); //emit(JVM.IFGE, trueLable);
        emit(JVM.ICONST_0);
        emit(JVM.GOTO, nextLable);
        emit(trueLable + ":");
        emit(JVM.ICONST_1);
        frame.push();// push iconst_0 or iconst_1

        emit(nextLable + ":");
    }

    public void signOfUnary(String operator, Type type, Frame frame){
        frame.pop();
        if (operator.equals("i2f"))
            emit(JVM.I2F);
        else {
            if (type.isIntType()) {
                if (operator.equals("i-"))
                    emit(JVM.INEG);
            }
            else if (type.isFloatType()){
                if (operator.equals("f-"))
                    emit(JVM.FNEG);
            }
            else{ // if (type.isBooleanType())
                if (operator.equals("i!")) {
                    String falseLabel = frame.getNewLabel();
                    String nextLabel = frame.getNewLabel();

                    emit(JVM.IFNE, falseLabel);
                    emit(JVM.ICONST_1);
                    emit("goto", nextLabel);
                    emit(falseLabel + ":");
                    emit(JVM.ICONST_0);
                    emit(nextLabel + ":");
                }
            }
        }
        frame.push();
    }

    public void emitASTORE(int index){
        if (index >= 0 && index <= 3)
            emit(JVM.ASTORE + "_" + index);
        else
            emit(JVM.ASTORE, index);
    }

    public void emitIALOAD(int index){
        if (index >= 0 && index <= 3)
            emit("iaload" + "_" + index);
        else
            emit("iaload", index);
    }

    public void emitFALOAD(int index){
        if (index >= 0 && index <= 3)
            emit("faload" + "_" + index);
        else
            emit("faload", index);
    }

    public void emitBALOAD(int index){
        if (index >= 0 && index <= 3)
            emit("baload" + "_" + index);
        else
            emit("baload", index);
    }

    public void putEleIntoArray(Expr expr, Frame frame){
        if (expr instanceof IntExpr)
            emit("iastore");
        else if (expr instanceof FloatExpr)
            emit("fastore");
        else if (expr instanceof BooleanExpr)
            emit("bastore");
        else{ // must be unaryExpr
            UnaryExpr unaryExpr = (UnaryExpr) expr;
            Operator operator = unaryExpr.O;
            Type type = unaryExpr.E.type;
            if (operator.spelling.equals("i2f"))
                emit("fastore");
            else{
                if (type.isIntType())
                    emit("iastore");
                else if (type.isFloatType())
                    emit("fastore");
                else //if (type.isBooleanType())
                    emit("bastore");
            }
        }
    }
    // this function would be used by 3 times, local para, global, and do the initialize
    public void localAndParaArrayDecl(ArrayType type, Object o){//, int index){
        Frame frame = (Frame) o;
        IntExpr range = (IntExpr) type.E;
        range.visit(this, o); // emit iconst and push into stack
        frame.pop();// pop the range
        String arrayType = VCtoArrayType(type.toString());
        emit("newarray", arrayType);
        frame.push(); // ***push this newarray into the stack
        // no initializer, so we need initialize a default initializer
        processUninitArray(type, o);

//        frame.pop(); // ***after insertion pop this newarray into the stack
//        emitASTORE(index);
    }

    // process the uninitialized array
    public void processUninitArray(ArrayType ast, Object o){ // here expr is the initializer part expr
        Type at = ast.T;
        int range = Integer.parseInt(((IntExpr) ast.E).IL.spelling);
        Frame frame = (Frame) o;

        for (int i = 0; i < range; i++){
            emit(JVM.DUP);
            frame.push(); // push the arrayref
            emitICONST(i); // emit array counter
            frame.push(); // push the counter
            emitUninitArrayEle(at); // initialize the  element as 0(or0.0),
            frame.push(); // push array element into the stack

            frame.pop(); // pop the element
            frame.pop(); // pop the counter
            frame.pop(); // pop the arrayref
            emitEleIntoUnititArray(at); // here at is normal type not the arrayType
        }
    }

    // get the initial value
    public void emitUninitArrayEle(Type at){
        if (at.isIntType() || at.isBooleanType()){
            emitICONST(0);
        }
        else {
            emitFCONST(0);
        }
    }

    // store into the newarray varible
    public void emitEleIntoUnititArray(Type at){
        if (at.isIntType())
            emit("iastore");
        else if (at.isFloatType())
            emit("fastore");
        else if (at.isBooleanType())
            emit("bastore");
        else
            errorReporter.reportError("Error, array can only be int, float or boolean type", "", at.position);
    }
}