#include "codegeneration.hpp"

// CodeGenerator Visitor Functions: These are the functions
// you will complete to generate the x86 assembly code. Not
// all functions must have code, many may be left empty.

void CodeGenerator::visitProgramNode(ProgramNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitClassNode(ClassNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitMethodNode(MethodNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitMethodBodyNode(MethodBodyNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitParameterNode(ParameterNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitDeclarationNode(DeclarationNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitReturnStatementNode(ReturnStatementNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitAssignmentNode(AssignmentNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitCallNode(CallNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitIfElseNode(IfElseNode* node) {
    
    std::cout << "  #### If/Else" << std::endl;
    node->visit_children(this);
    std::cout << "  #pop the else statement list" << std::endl;
    std::cout << "  pop %ecx" << std::endl;
    std::cout << "  #pop the if statement list" << std::endl;
    std::cout << "  pop %ebx" << std::endl;
    std::cout << "  #pop the expression" << std::endl;
    std::cout << "  pop %eax" << std::endl;

    std::cout << "  #check if the statement is not false (0)" << std::endl;
    std::cout << "  and %eax, 1" << std::endl;
    std::cout << "  cmp %eax, 1" << std::endl;
    std::cout << "  je ifTrue" << std::endl;
    std::cout << "  #if the statment is false" << std::endl;
    std::cout << "  #run every statment in %ecx" << std::endl;

    std::cout << "  #if the statment is true" << std::endl;
    std::cout << "  ifTrue:" << std::endl;
    std::cout << "  #run every statment in %ebx" << std::endl;
}

void CodeGenerator::visitWhileNode(WhileNode* node) {

    std::cout << "  #### While" << std::endl;
    node->visit_children(this);
    std::cout << "  #pop the statement list" << std::endl;
    std::cout << "  pop %ebx" << std::endl;
    std::cout << "  #pop the expression" << std::endl;
    std::cout << "  pop %eax" << std::endl;

    std::cout << "  startLoop:" << std::endl;
    std::cout << "  #check if the statement is true" << std::endl;
    std::cout << "  and %eax, 1" << std::endl;
    std::cout << "  cmp %eax, 1" << std::endl;
    std::cout << "  jne endLoop" << std::endl;

    std::cout << "  #run entire statement list" << std::endl;
    std::cout << "  jmp startLoop"  << std::endl;

    std::cout << "  endLoop:" << std::endl;
}

void CodeGenerator::visitPrintNode(PrintNode* node) {
    
    std::cout << "  #### Do-While" << std::endl;
    node->visit_children(this);
    std::cout << "  #pop the statement list" << std::endl;
    std::cout << "  pop %ebx" << std::endl;
    std::cout << "  #pop the expression" << std::endl;
    std::cout << "  pop %eax" << std::endl;

    std::cout << "  startLoop:" << std::endl;
    std::cout << "  #run entire statement list" << std::endl;

    std::cout << "  #check if the statement is true" << std::endl;
    std::cout << "  and %eax, 1" << std::endl;
    std::cout << "  cmp %eax, 1" << std::endl;
    std::cout << "  jne endLoop" << std::endl;

    std::cout << "  jmp startLoop"  << std::endl;

    std::cout << "  endLoop:" << std::endl;
}

void CodeGenerator::visitDoWhileNode(DoWhileNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitPlusNode(PlusNode* node) {

    std::cout << "  #### Plus" << std::endl;

    node->visit_children(this);

    std::cout << "  pop %ebx" << std::endl;
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  add %ebx, %eax" << std::endl;
    std::cout << "  push %eax" << std::endl;
    /*
    pop %ebx
    pop %eax
    add %ebx, %eax
    push %eax
    */
}

void CodeGenerator::visitMinusNode(MinusNode* node) {

    std::cout << "  #### Minus" << std::endl;

    node->visit_children(this);

    std::cout << "  pop %ebx" << std::endl;
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  sub %ebx, %eax" << std::endl;
    std::cout << "  push %eax" << std::endl;
}

void CodeGenerator::visitTimesNode(TimesNode* node) {

    std::cout << "  #### Times" << std::endl;
    node->visit_children(this);
    std::cout << "  pop %ebx" << std::endl;
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  imul %ebx, %eax" << std::endl;
    std::cout << "  push %eax" << std::endl;
}

void CodeGenerator::visitDivideNode(DivideNode* node) {

    //ask the professor
    std::cout << "  #### Divide" << std::endl;
    node->visit_children(this);
    std::cout << "  pop %edx" << std::endl;
    std::cout << "  pop %eax" << std::endl;
    //std::cout << "  cdq" << std::endl;
    std::cout << "  idiv %ebx" << std::endl;
    std::cout << "  push %eax" << std::endl;
}

void CodeGenerator::visitGreaterNode(GreaterNode* node) {

    std::cout << "  #### Greater" << std::endl;
    node->visit_children(this);
    std::cout << "  pop %ebx" << std::endl;
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  comp %ebx, %eax" << std::endl;
    std::cout << "  jge setGreater" << std::endl;
    std::cout << "  push $0" << std::endl;
    //how do we end this part without also pushing 1?

    std::cout << "  setGreater:" << std::endl;
    std::cout << "  push $1" << std::endl;
}

void CodeGenerator::visitGreaterEqualNode(GreaterEqualNode* node) {

    std::cout << "  #### GreaterEqual" << std::endl;
    node->visit_children(this);
    std::cout << "  pop %ebx" << std::endl;
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  comp %ebx, %eax" << std::endl;
    std::cout << "  jge setGreaterEqual" << std::endl;
    std::cout << "  push $0" << std::endl;
    //how do we end this part without also pushing 1?

    std::cout << "  setGreaterEqual:" << std::endl;
    std::cout << "  push $1" << std::endl;
}

void CodeGenerator::visitEqualNode(EqualNode* node) {

    std::cout << "  #### Equal" << std::endl;
    node->visit_children(this);
    std::cout << "  pop %ebx" << std::endl;
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  comp %ebx, %eax" << std::endl;
    std::cout << "  je setEqual" << std::endl;
    std::cout << "  push $0" << std::endl;
    //how do we end this part without also pushing 1?

    std::cout << "  setEqual:" << std::endl;
    std::cout << "  push $1" << std::endl;
}

void CodeGenerator::visitAndNode(AndNode* node) {

    std::cout << "  #### And" << std::endl;
    node->visit_children(this);
    std::cout << "  pop %ebx" << std::endl;
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  and %ebx, %eax" << std::endl;
    std::cout << "  push %eax" << std::endl;
}

void CodeGenerator::visitOrNode(OrNode* node) {

    std::cout << "  #### Or" << std::endl;
    node->visit_children(this);
    std::cout << "  pop %ebx" << std::endl;
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  or %ebx, %eax" << std::endl;
    std::cout << "  push %eax" << std::endl;
}

void CodeGenerator::visitNotNode(NotNode* node) {

    std::cout << "  #### Not" << std::endl;
    node->visit_children(this);
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  not %eax" << std::endl;
    std::cout << "  push %eax" << std::endl;
}

void CodeGenerator::visitNegationNode(NegationNode* node) {
    
    std::cout << "  #### Negation" << std::endl;
    node->visit_children(this);
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  neg %eax" << std::endl;
    std::cout << "  push %eax" << std::endl;
}

void CodeGenerator::visitMethodCallNode(MethodCallNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitMemberAccessNode(MemberAccessNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitVariableNode(VariableNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitIntegerLiteralNode(IntegerLiteralNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitBooleanLiteralNode(BooleanLiteralNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitNewNode(NewNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitIntegerTypeNode(IntegerTypeNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitBooleanTypeNode(BooleanTypeNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitObjectTypeNode(ObjectTypeNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitNoneNode(NoneNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitIdentifierNode(IdentifierNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitIntegerNode(IntegerNode* node) {
    // WRITEME: Replace with code if necessary
}
