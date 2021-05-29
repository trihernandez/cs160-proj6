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
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitWhileNode(WhileNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitPrintNode(PrintNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitDoWhileNode(DoWhileNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitPlusNode(PlusNode* node) {
    node->visit_children(this);
    std::cout << "  # Plus" << std::endl;
    std::cout << "  pop %edx" << std::endl;
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  add %edx, %eax" << std::endl;
    std::cout << "  push %eax" << std::endl;
    pop %edx
    pop %eax
    add %edx, %eax
    push %eax
}

void CodeGenerator::visitMinusNode(MinusNode* node) {
    node->visit_children(this);
    std::cout << "  # Minus" << std::endl;
    std::cout << "  pop %edx" << std::endl;
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  sub %edx, %eax" << std::endl;
    std::cout << "  push %eax" << std::endl;
    pop %edx
    pop %eax
    sub %edx, %eax
    push %eax
}

void CodeGenerator::visitTimesNode(TimesNode* node) {
    node->visit_children(this);
    std::cout << "  # Times" << std::endl;
    std::cout << "  pop %edx" << std::endl;
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  imul %edx, %eax" << std::endl;
    std::cout << "  push %eax" << std::endl;
    pop %edx
    pop %eax
    imul %edx, %eax
    push %eax
}

void CodeGenerator::visitDivideNode(DivideNode* node) {
    node->visit_children(this);
    std::cout << "  # Divide" << std::endl;
    std::cout << "  pop %edx" << std::endl;
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  idiv %edx, %eax" << std::endl;
    std::cout << "  push %eax" << std::endl;
    pop %edx
    pop %eax
    idiv %edx, %eax
    push %eax
}

void CodeGenerator::visitGreaterNode(GreaterNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitGreaterEqualNode(GreaterEqualNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitEqualNode(EqualNode* node) {
    node->visit_children(this);
    std::cout << "  # Equal" << std::endl;
    std::cout << "  pop %edx" << std::endl;
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  beq %edx, %eax" << std::endl;
    std::cout << "  push %eax" << std::endl;
    pop %edx
    pop %eax
    comp %edx, %eax //both sides of the expression are equal
    push %eax
}

void CodeGenerator::visitAndNode(AndNode* node) {
    node->visit_children(this);
    std::cout << "  # And" << std::endl;
    std::cout << "  pop %edx" << std::endl;
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  and %edx, %eax" << std::endl;
    std::cout << "  push %eax" << std::endl;
    pop %edx
    pop %eax
    and %edx, %eax
    push %eax
}

void CodeGenerator::visitOrNode(OrNode* node) {
    node->visit_children(this);
    std::cout << "  # Or" << std::endl;
    std::cout << "  pop %edx" << std::endl;
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  or %edx, %eax" << std::endl;
    std::cout << "  push %eax" << std::endl;
    pop %edx
    pop %eax
    or %edx, %eax
    push %eax
}

void CodeGenerator::visitNotNode(NotNode* node) {
    pop %eax
    not %eax
    push %eax
}

void CodeGenerator::visitNegationNode(NegationNode* node) {
    pop %eax
    neg %eax
    push %eax
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
