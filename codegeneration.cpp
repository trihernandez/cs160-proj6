#include "codegeneration.hpp"

// CodeGenerator Visitor Functions: These are the functions
// you will complete to generate the x86 assembly code. Not
// all functions must have code, many may be left empty.

//Clarifications on @269 and @270

void CodeGenerator::visitProgramNode(ProgramNode* node) {
    
    std::cout << "  ############ Program" << std::endl;
    std::cout << "  .data" << std::endl;
    std::cout << "  printstr: .asciz \"%d\\n\"" << std::endl;
    std::cout << "  .text" << std::endl;
    std::cout << "  .globl Main_main" << std::endl;

    node->visit_children(this);
    std::cout << "  ############ End Program" << std::endl;
}

void CodeGenerator::visitClassNode(ClassNode* node) {
    
    //initialize current class info
    ClassInfo cInfo;

    if( node->identifier_2 == NULL)
        cInfo.superClassName = "";
    else
        cInfo.superClassName = node->identifier_2->name;
        // if( classTable->find( cInfo.superClassName ) == classTable->end() )
        //     typeError(undefined_class);

    currentClassName = node->identifier_1->name;
    currentClassInfo = this->classTable->find(currentClassName)->second;

    std::cout << "  ######## Class" << currentClassName << std::endl;

    /*
    currentMemberOffset = 0;
    currentVariableTable = new std::map<std::string, VariableInfo>();
    cInfo.members = currentVariableTable;
    std::list<DeclarationNode*>* MemberNodeList = node->declaration_list;
    for (auto it = MemberNodeList->begin(); it!=MemberNodeList->end(); it++)
    {
        //since it's not a local variable, disable the currentLocalOffset for this declaration
        currentLocalOffset = 4;
        visitDeclarationNode( *it );
        currentMemberOffset = currentMemberOffset + 4;
    }
    */

    node->visit_children(this);
    std::cout << "  ######## End Class" << currentClassName << std::endl;
}

void CodeGenerator::visitMethodNode(MethodNode* node) {

    currentMethodName = node->identifier->name;
    currentMethodInfo = currentClassInfo.methods->find(currentMethodName)->second;

    std::cout << "  #### Method " << currentMethodName << std::endl;

    std::cout << currentClassName << "_" << currentMethodName << ":" << std::endl; //name not printing correctly
    std::cout << "  push %ebp" << std::endl;
    std::cout << "  mov %esp, %ebp" << std::endl;
    std::cout << "  sub $0, %esp" << std::endl; //I think this should not be 0 and instead the total offset of local variables
    std::cout << "  push %ebx" << std::endl;

    std::cout << "  push %edi" << std::endl;
    std::cout << "  push %esi" << std::endl;

    node->visit_children(this);

    std::cout << "  pop %esi" << std::endl;
    std::cout << "  pop %edi" << std::endl;

    std::cout << "  pop %ebx" << std::endl;
    std::cout << "  mov %ebp, %esp" << std::endl;
    std::cout << "  pop %ebp" << std::endl;

    std::cout << "  ret" << std::endl;
    std::cout << "  #### End Method " << currentMethodName << std::endl;

}

void CodeGenerator::visitMethodBodyNode(MethodBodyNode* node) {

    std::cout << "  ## MethodBody" << std::endl;
    node->visit_children(this);
    std::cout << "  ## End MethodBody" << std::endl;

}

void CodeGenerator::visitParameterNode(ParameterNode* node) {

    std::cout << "  ## Parameter" << std::endl;
    node->visit_children(this);
    std::cout << "  ## End Parameter" << std::endl;

}

void CodeGenerator::visitDeclarationNode(DeclarationNode* node) {

    int allocSize = 0; // size of the object that we want to allocate.  I think it's 4 + (4*numVariables)

    // std::cout << "  ## Declaration" << std::endl;

    node->visit_children(this);
    // //taken from Disc. 10 slide 15
    // std::cout << "  push $" << allocSize << "       # size to allocate" << std::endl;
    // std::cout << "  call malloc   # call malloc with one arg" << std::endl;
    // std::cout << "  add  $4, %esp # remove args from stack" << std::endl;
    // std::cout << "  push %eax" << std::endl;

    // std::cout << "  ## End Declaration" << std::endl;
}

void CodeGenerator::visitReturnStatementNode(ReturnStatementNode* node) {

    std::cout << "  ## ReturnStatement" << std::endl;
    node->visit_children(this);
    std::cout << "  ## End ReturnStatement" << std::endl;
}

void CodeGenerator::visitAssignmentNode(AssignmentNode* node) {

    //memberOffsetIs
    //node->identifier_1->name
    //node->identifier_2->name

    //for each member ahead, it would be 4
    std::cout << "  Start" << std::endl;
    auto *members = this->classTable->find(currentClassName)->second.members;

    int offset = 0;
    int iteration = 0;
    std::cout << "  Before if" << std::endl;

    if(members->find(node->identifier_1->name) != members->end()){//if it is in current class check all the members
        std::cout << "  In if " << std::endl;
        for( auto m = members->begin(); m != members->end(); m++){
            if(m->first == node->identifier_1->name){
                offset = iteration;
            }
            iteration = iteration + 4;
        }
    }
    else{//if it is in the superclass
    std::cout << "  In else" << std::endl;
        members = this->classTable->find(this->classTable->find(currentClassName)->second.superClassName)->second.members;
        std::cout << "  Start" << std::endl;
        for( auto m = members->begin(); m != members->end(); m++){
            std::cout << "  For" << std::endl;
            if(m->first == node->identifier_1->name){
                offset = iteration;
            }
            iteration = iteration + 4;
        }

    }

    std::string variable = node->identifier_1->name;

    //work on an offset calculator

    std::cout << "  ## Assignment" << std::endl;
    node->visit_children(this);
    if( this->currentMethodInfo.variables->find(variable) != this->currentMethodInfo.variables->end() )
    {        
        //offset = this->currentMethodInfo.variables->at(variable).offset;
        std::cout << "  pop " << offset << "(%ebx)" << std::endl;
    }
    std::cout << "  ## End Assignment" << std::endl;

}

void CodeGenerator::visitCallNode(CallNode* node) {

    std::cout << "  ## Call" << std::endl;
    node->visit_children(this);
    std::cout << "  ## End Call" << std::endl;

}

void CodeGenerator::visitIfElseNode(IfElseNode* node) {
    
    std::cout << "  ## If/Else" << std::endl;
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
    std::cout << "  ## End If/Else" << std::endl;

}

void CodeGenerator::visitWhileNode(WhileNode* node) {

    std::cout << "  ## While" << std::endl;
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
    std::cout << "  ## End While" << std::endl;

}

void CodeGenerator::visitPrintNode(PrintNode* node) {
    
    std::cout << "  ## Print" << std::endl;

    std::cout << "  push %eax" << std::endl;
    std::cout << "  push %ecx" << std::endl;
    std::cout << "  push %edx" << std::endl;

    node->visit_children(this);

    std::cout << "  pushl $printstr" << std::endl;
    std::cout << "  call printf" << std::endl;
    std::cout << "  add $8, %esp" << std::endl;

    std::cout << "  pop %edx" << std::endl;
    std::cout << "  pop %ecx" << std::endl;
    std::cout << "  pop %eax" << std::endl;

    std::cout << "  ## End Print" << std::endl;
}

void CodeGenerator::visitDoWhileNode(DoWhileNode* node) {

    std::cout << "  ## Do-While" << std::endl;
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
    std::cout << "  ## End Do-While" << std::endl;

}

void CodeGenerator::visitPlusNode(PlusNode* node) {

    std::cout << "  ## Plus" << std::endl;

    node->visit_children(this);

    std::cout << "  pop %ebx" << std::endl;
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  add %ebx, %eax" << std::endl;
    std::cout << "  push %eax" << std::endl;

    std::cout << "  ## End Plus" << std::endl;
}

void CodeGenerator::visitMinusNode(MinusNode* node) {

    std::cout << "  ## Minus" << std::endl;

    node->visit_children(this);

    std::cout << "  pop %ebx" << std::endl;
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  sub %ebx, %eax" << std::endl;
    std::cout << "  push %eax" << std::endl;

    std::cout << "  ## End Minus" << std::endl;
}

void CodeGenerator::visitTimesNode(TimesNode* node) {

    std::cout << "  ## Times" << std::endl;
    node->visit_children(this);
    std::cout << "  pop %ebx" << std::endl;
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  imul %ebx, %eax" << std::endl;
    std::cout << "  push %eax" << std::endl;

    std::cout << "  ## End Times" << std::endl;

}

void CodeGenerator::visitDivideNode(DivideNode* node) {

    //ask the professor
    std::cout << "  ## Divide" << std::endl;
    node->visit_children(this);
    std::cout << "  pop %ebx" << std::endl;
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  cdq" << std::endl;
    std::cout << "  idiv %ebx" << std::endl;
    std::cout << "  push %eax" << std::endl;

    std::cout << "  ## End Divide" << std::endl;

}

void CodeGenerator::visitGreaterNode(GreaterNode* node) {

    std::cout << "  ## Greater" << std::endl;
    node->visit_children(this);
    std::cout << "  pop %ebx" << std::endl;
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  comp %ebx, %eax" << std::endl;
    std::cout << "  jge setGreater" << std::endl;
    std::cout << "  push $0" << std::endl;
    //how do we end this part without also pushing 1?

    std::cout << "  setGreater:" << std::endl;
    std::cout << "  push $1" << std::endl;

    std::cout << "  ## End Greater" << std::endl;

}

void CodeGenerator::visitGreaterEqualNode(GreaterEqualNode* node) {

    std::cout << "  ## GreaterEqual" << std::endl;
    node->visit_children(this);
    std::cout << "  pop %ebx" << std::endl;
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  comp %ebx, %eax" << std::endl;
    std::cout << "  jge setGreaterEqual" << std::endl;
    std::cout << "  push $0" << std::endl;
    //how do we end this part without also pushing 1?

    std::cout << "  setGreaterEqual:" << std::endl;
    std::cout << "  push $1" << std::endl;

    std::cout << "  ## End GreaterEqual" << std::endl;
}

void CodeGenerator::visitEqualNode(EqualNode* node) {

    std::cout << "  ## Equal" << std::endl;
    node->visit_children(this);
    std::cout << "  pop %ebx" << std::endl;
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  comp %ebx, %eax" << std::endl;
    std::cout << "  je setEqual" << std::endl;
    std::cout << "  push $0" << std::endl;
    //how do we end this part without also pushing 1?

    std::cout << "  setEqual:" << std::endl;
    std::cout << "  push $1" << std::endl;

    std::cout << "  ## End Equal" << std::endl;
}

void CodeGenerator::visitAndNode(AndNode* node) {

    std::cout << "  ## And" << std::endl;
    node->visit_children(this);
    std::cout << "  pop %ebx" << std::endl;
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  and %ebx, %eax" << std::endl;
    std::cout << "  push %eax" << std::endl;
    std::cout << "  ## End And" << std::endl;
}

void CodeGenerator::visitOrNode(OrNode* node) {

    std::cout << "  ## Or" << std::endl;
    node->visit_children(this);
    std::cout << "  pop %ebx" << std::endl;
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  or %ebx, %eax" << std::endl;
    std::cout << "  push %eax" << std::endl;
    std::cout << "  ## End Or" << std::endl;
}

void CodeGenerator::visitNotNode(NotNode* node) {

    std::cout << "  ## Not" << std::endl;
    node->visit_children(this);
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  not %eax" << std::endl;
    std::cout << "  push %eax" << std::endl;
    std::cout << "  ## End Not" << std::endl;
}

void CodeGenerator::visitNegationNode(NegationNode* node) {
    
    std::cout << "  ## Negation" << std::endl;
    node->visit_children(this);
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  neg %eax" << std::endl;
    std::cout << "  push %eax" << std::endl;

    std::cout << "  ## End Negation" << std::endl;
}

void CodeGenerator::visitMethodCallNode(MethodCallNode* node) {

    //VariableTable* variableTable = new VariableTable();
    //VariableInfo variable;
    //MethodInfo method;
    std::string methodName = node->identifier_1->name;
    if( node->identifier_2 )
        methodName = node->identifier_2->name;

    std::cout << "  ## MethodCall" << std::endl;
    //update current method info?
//     node->visit_children(this);
//     if( currentClassInfo.methods->find(methodName) != currentClassInfo.methods->end() )
//     {        
//         currentClassInfo.methods->at(methodName).offset;
//         std::cout << "  mov %ebp, %eax" << std::endl;
//         std::cout << "  add $offset, %eax" << std::endl;
//         std::cout << "  push %eax" << std::endl;
//     }
//     else
//     {
//         if( classTable->find(currentClassInfo.superClassName) != classTable->end() )
//         { 
//             ClassInfo superClass = classTable->at(currentClassInfo.superClassName);
//             if( superClass.methods->find(methodName) != superClass.methods->end()  )        
//             {
//                 superClass.methods->at(methodName).offset;
//                 std::cout << "  mov %ebp, %eax" << std::endl;
//                 std::cout << "  add $offset, %eax" << std::endl;
//                 std::cout << "  push %eax" << std::endl;
//             }
//         }
//     }
    std::cout << "  ## End MethodCall" << std::endl;

}

void CodeGenerator::visitMemberAccessNode(MemberAccessNode* node) {

    //VariableInfo variable;
    std::string classObjectName = node->identifier_1->name;
    std::string classObjectMember = node->identifier_2->name;
    int offset = 0;
    int iteration = 0;
    std::cout << "  ## MemberAccess" << std::endl;
    std::cout << "  # Find class object (identifier 1) in declared objects" << std::endl;
    node->visit_children(this);
    if( currentClassInfo.members->find(classObjectMember) != currentClassInfo.members->end() )
    {       
        // auto* mems =  currentClassInfo.members;
        // for( auto m = mems->begin(); m != mems->end(); m++){
        //     if(m->first == classObjectMember){
        //         offset = iteration;
        //     }
        //     iteration = iteration + 4;
        // }


        offset = currentClassInfo.members->at(classObjectMember).offset;
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
                // auto* mems =  superClass.members;
                // for( auto m = mems->begin(); m != mems->end(); m++){
                //     if(m->first == classObjectMember){
                //         offset = iteration;//may need to add offset of main class
                //     }
                //     iteration = iteration + 4;
                // }
                offset = superClass.members->at(classObjectMember).offset;
                std::cout << "  mov %ebp, %eax" << std::endl;
                std::cout << "  mov " << offset << "(%eax), %eax" << std::endl;
                std::cout << "  push %eax" << std::endl;
            }
        }
    }
    std::cout << "  ## End MemberAccess" << std::endl;

}

void CodeGenerator::visitVariableNode(VariableNode* node) {
    
    //VariableInfo variable;
    std::string variable = node->identifier->name;

    std::cout << "  ## Variable" << std::endl;
    node->visit_children(this);
    if( currentMethodInfo.variables->find(variable) != currentMethodInfo.variables->end() )
    {        
        int offset = currentMethodInfo.variables->at(variable).offset;
        std::cout << "  push " << offset << "%(ebp)" << std::endl;
    }
    std::cout << "  ## End Variable" << std::endl;
}

void CodeGenerator::visitIntegerLiteralNode(IntegerLiteralNode* node) {

    std::cout << "  ## IntLiteral" << std::endl;
    node->visit_children(this);
    std::cout << "  push $" << node->integer->value << std::endl;
    std::cout << "  ## End IntLiteral" << std::endl;
}

void CodeGenerator::visitBooleanLiteralNode(BooleanLiteralNode* node) {
    std::cout << "  ## BoolLiteral" << std::endl;
    node->visit_children(this);
    std::cout << "  push $" << node->integer->value << std::endl;
    std::cout << "  ## End BoolLiteral" << std::endl;
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
