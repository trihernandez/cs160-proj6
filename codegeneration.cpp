#include "codegeneration.hpp"

// CodeGenerator Visitor Functions: These are the functions
// you will complete to generate the x86 assembly code. Not
// all functions must have code, many may be left empty.

//Clarifications on @269 and @270

void CodeGenerator::visitProgramNode(ProgramNode* node) {
    
    std::cout << "  #### Program" << std::endl;
    std::cout << "  .data" << std::endl;
    std::cout << "  printstr: .asciz \"%d\n\"" << std::endl;
    std::cout << "  .text" << std::endl;
    node->visit_children(this);
    std::cout << "  ret" << std::endl;
}

void CodeGenerator::visitClassNode(ClassNode* node) {
    
    std::cout << "  #### Class" << std::endl;
    std::cout << "  .globl" << std::endl;
    node->visit_children(this);
}

void CodeGenerator::visitMethodNode(MethodNode* node) {

    std::cout << "  #### Method" << std::endl;
    std::cout << currentClassName << "_" << currentMethodName << ":" << std::endl;
    std::cout << "  push %ebp" << std::endl;
    std::cout << "  mov %esp, %ebp" << std::endl;
    std::cout << "  push %ebx" << std::endl;
    node->visit_children(this);
    std::cout << "  pop %ebx" << std::endl;
    std::cout << "  mov %ebp, %esp" << std::endl;
    std::cout << "  pop %ebp" << std::endl;
}

void CodeGenerator::visitMethodBodyNode(MethodBodyNode* node) {

    std::cout << "  #### MethodBody" << std::endl;
    node->visit_children(this);
}

void CodeGenerator::visitParameterNode(ParameterNode* node) {

    std::cout << "  #### Parameter" << std::endl;
    node->visit_children(this);
}

void CodeGenerator::visitDeclarationNode(DeclarationNode* node) {

    std::cout << "  #### Declaration" << std::endl;
    node->visit_children(this);
}

void CodeGenerator::visitReturnStatementNode(ReturnStatementNode* node) {

    std::cout << "  #### ReturnStatement" << std::endl;
    node->visit_children(this);
}

void CodeGenerator::visitAssignmentNode(AssignmentNode* node) {

    std::cout << "  #### Assignment" << std::endl;
    node->visit_children(this);
}

void CodeGenerator::visitCallNode(CallNode* node) {

    std::cout << "  #### Call" << std::endl;
    node->visit_children(this);
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
    
    std::cout << "  #### Print" << std::endl;
    node->visit_children(this);
    std::cout << "  #print the expression" << std::endl;
    std::cout << "  push $printstr" << std::endl;
    std::cout << "  call printf" << std::endl;
    std::cout << "  add $8, %esp" << std::endl;

    //std::cout << "  #pop the expression" << std::endl;
    //std::cout << "  pop %ebx" << std::endl;
}

void CodeGenerator::visitDoWhileNode(DoWhileNode* node) {

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

void CodeGenerator::visitPlusNode(PlusNode* node) {

    std::cout << "  #### Plus" << std::endl;

    node->visit_children(this);

    std::cout << "  pop %ebx" << std::endl;
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  add %ebx, %eax" << std::endl;
    std::cout << "  push %eax" << std::endl;
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

    //VariableTable* variableTable = new VariableTable();
    //VariableInfo variable;
    //MethodInfo method;
    std::string methodName = node->identifier_1->name;
    if( node->identifier_2 )
        methodName = node->identifier_2->name;

    std::cout << "  #### MethodCall" << std::endl;
    //update current method info?
    node->visit_children(this);
    if( currentClassInfo.methods->find(methodName) != currentClassInfo.methods->end() )
    {        
        currentClassInfo.methods->at(methodName).offset;
        std::cout << "  mov %ebp, %eax" << std::endl;
        std::cout << "  add $offset, %eax" << std::endl;
        std::cout << "  push %eax" << std::endl;
    }
    else
    {
        if( classTable->find(currentClassInfo.superClassName) != classTable->end() )
        { 
            ClassInfo superClass = classTable->at(currentClassInfo.superClassName);
            if( superClass.methods->find(methodName) != superClass.methods->end()  )        
            {
                superClass.methods->at(methodName).offset;
                std::cout << "  mov %ebp, %eax" << std::endl;
                std::cout << "  add $offset, %eax" << std::endl;
                std::cout << "  push %eax" << std::endl;
            }
        }
    }
}

void CodeGenerator::visitMemberAccessNode(MemberAccessNode* node) {

    //VariableInfo variable;
    std::string classObjectName = node->identifier_1->name;
    std::string classObjectMember = node->identifier_2->name;

    std::cout << "  #### Member access" << std::endl;
    std::cout << "  # Find class object (identifier 1) in declared objects" << std::endl;
    node->visit_children(this);
    if( currentClassInfo.members->find(classObjectMember) != currentClassInfo.members->end() )
    {        
        currentClassInfo.members->at(classObjectMember).offset;
        std::cout << "  mov %ebp, %eax" << std::endl;
        std::cout << "  mov " << offset << "(%eax), %eax" << std::endl;
        std::cout << "  push %eax" << std::endl;
    }
    else
    {
        if( classTable->find(currentClassInfo.superClassName) != classTable->end() )
        { 
            ClassInfo superClass = classTable->at(currentClassInfo.superClassName);
            if( superClass.members->find(classObjectMember) != superClass.members->end() )       
            {
                superClass.members->at(classObjectMember).offset;
                std::cout << "  mov %ebp, %eax" << std::endl;
                std::cout << "  mov " << offset << "(%eax), %eax" << std::endl;
                std::cout << "  push %eax" << std::endl;
            }
        }
    }
}

void CodeGenerator::visitVariableNode(VariableNode* node) {
    
    //VariableInfo variable;
    std::string variable = node->identifier->name;

    std::cout << "  #### Variable" << std::endl;
    node->visit_children(this);
    if( currentMethodInfo.variables->find(variable) != currentMethodInfo.variables->end() )
    {        
        int offset = currentMethodInfo.variables->at(variable).offset;
        std::cout << "  mov %ebp, %eax" << std::endl;
        std::cout << "  mov " << offset << "(%eax), %eax" << std::endl;
        std::cout << "  push %eax" << std::endl;
    }
}

void CodeGenerator::visitIntegerLiteralNode(IntegerLiteralNode* node) {

    std::cout << "  #### IntLiteral" << std::endl;
    node->visit_children(this);
    std::cout << "  # get the integer value stored at this node" << std::endl;
    std::cout << "  push $" << node->integer->value << std::endl;
}

void CodeGenerator::visitBooleanLiteralNode(BooleanLiteralNode* node) {
    std::cout << "  #### BoolLiteral" << std::endl;
    node->visit_children(this);
    std::cout << "  # get 1 if true and 0 if false" << std::endl;
    std::cout << "  push $" << node->integer->value << std::endl;
}

void CodeGenerator::visitNewNode(NewNode* node) {
    // WRITEME: Replace with code if necessary
}

//starting here I don't think we write anything else
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
