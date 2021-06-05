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
    std::cout << "  sub $" << currentMethodInfo.localsSize << ", %esp" << std::endl; //I think this should not be 0 and instead the total offset of local variables
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
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  ## End ReturnStatement" << std::endl;
}

void CodeGenerator::visitAssignmentNode(AssignmentNode* node) {

    //memberOffsetIs
    std::string iden1 = node->identifier_1->name;
    std::string iden2;

    std::string iden1className = "";
    std::string leftClassName = "";

    //for each member ahead, it would be 4
    //std::cout << "  # Start" << std::endl;
    auto members = this->classTable->find(currentClassName)->second.members;
    auto variables = this->currentMethodInfo.variables;

    int offset = 0;
    int maxSuperOffset = 0;
    //std::cout << "  # Before if" << std::endl;

    //Check if node->identifier 2 is null.
    //  If so, check if node->identifier 1 is a member of the current class
    //  Or in the current method's variableTable
    if(node->identifier_2 == NULL)
    {
        //std::cout << "  # iden1 = " << iden1 << std::endl;
        //if it is in the variable table get its offset
        if( variables->find(iden1) != variables->end())
        {
            //std::cout << "  # iden1 in variable table " << std::endl;
            for( auto m = variables->begin(); m != variables->end(); m++)
            {
                if(m->first == iden1)
                {
                    offset = m->second.offset;
                }
                //std::cout << "  # offset = " << offset << std::endl;
            }
        }

        //if it is in current class check all the members
        if( members->find(iden1) != members->end())
        {
            //std::cout << "  # iden1 in member table " << std::endl;
            for( auto m = members->begin(); m != members->end(); m++)
            {
                if(m->first == iden1)
                {
                    offset = m->second.offset;
                }
                maxSuperOffset = maxSuperOffset + 4;
                //std::cout << "  # offset = " << offset << std::endl;
            }
        }
    }
    //Else, check if identifier_1 is in the currentVariableTable as a defined class
    //  and then check if identifier_2 is in the class' method table
    else
    {
        std::cout << "  # In else " << std::endl;

        iden2 = node->identifier_2->name;

        while(true)
        {
            //First, check if iden1 is in auto *members
            auto memberSearch = members->find( iden1 );
            if( memberSearch != members->end() )
            {
                //iden1 is in the member table as a class
                if ( node->basetype == bt_object )
                    leftClassName = memberSearch->second.type.objectClassName;
                break;
            }

            //Else, check if iden1 is a local var. of the current method
            auto variableSearch = variables->find( iden1 );
            if( variableSearch != variables->end() )
            {
                //iden1 is in the variable table as a class
                if ( node->basetype == bt_object )
                    leftClassName = variableSearch->second.type.objectClassName;
                break;
            }
         
            //Else, check if iden1 is a member of a superclass
            std::string currentSuperClassName = currentClassInfo.superClassName;
            auto currentSuperClass = classTable->find(currentSuperClassName);

            //Else, Check if the superclass exists and has members
            if( currentSuperClass != classTable->end() && currentSuperClass->second.members != NULL )
            {
                //check if iden1 is a member of the superclass
                auto superMemberTableSearch = currentSuperClass->second.members->find(iden1);
                if( superMemberTableSearch != currentSuperClass->second.members->end() )
                {
                    //iden1 is in the superclass' member table as a class
                    if ( node->basetype == bt_object )
                        leftClassName = superMemberTableSearch->second.type.objectClassName;
                    break;
                }
            }
        }

        printf("  # iden1Class");
        auto iden1Class = classTable->find(iden1className);

        //Check if iden2 is a member of iden1
        std::map<std::string, VariableInfo>::iterator iden1MemberTableSearch = iden1Class->second.members->find(iden2);
        if( iden1MemberTableSearch != iden1Class->second.members->end() )
        {
            //iden2 is a member of iden1
        }
        else
        {
            //check if the superclass of iden1 exists
            std::string iden1superClassName = iden1Class->second.superClassName;
            std::map<std::string, ClassInfo>::iterator iden1superClass = classTable->find(iden1superClassName);
            
            //check if iden2 is a member in the superclass of iden1
            if( iden1superClass != classTable->end() ) {
                std::map<std::string, VariableInfo>::iterator iden1SuperMemberTableSearch = iden1superClass->second.members->find(iden2);
                if( iden1SuperMemberTableSearch != iden1superClass->second.members->end() )
                {
                    //iden2 is a member of the superclass of iden1
                    if ( node->basetype == bt_object )
                        leftClassName = iden1SuperMemberTableSearch->second.type.objectClassName;
                }
                else
                {
                    typeError(undefined_member);
                }
            }
            else
            {
                typeError(undefined_member);
            }
        }
    }

    /*
    if(members->find(iden1) != members->end()){//if it is in current class check all the members
        std::cout << "  # In if " << std::endl;
        for( auto m = members->begin(); m != members->end(); m++)
        {
            if(m->first == iden1)
            {
                offset = m->second.offset;
            }
            iteration = iteration + 4;
        }
    }
    else{//if it is in the superclass
        std::cout << "  # In else" << std::endl;
        std::string superClassName = this->classTable->find(currentClassName)->second.superClassName;
        auto SuperMembers = this->classTable->find(superClassName)->second.members;

        std::cout << "  # Start" << std::endl;
        for( auto m = members->begin(); m != members->end(); m++)
        {
            std::cout << "  # For" << std::endl;
            if(m->first == iden1)
            {
                offset = m->second.offset;
            }
            iteration = iteration + 4;
        }
    }
    */


    std::string variable = node->identifier_1->name;

    //work on an offset calculator

    std::cout << "  ## Assignment" << std::endl;
    node->visit_children(this);
    if( this->currentMethodInfo.variables->find(variable) != this->currentMethodInfo.variables->end() )
    {        
        //offset = this->currentMethodInfo.variables->at(variable).offset;
        std::cout << "  pop " << offset << "(%ebp)" << std::endl;
    }
    std::cout << "  ## End Assignment" << std::endl;

}

void CodeGenerator::visitCallNode(CallNode* node) {

    std::cout << "  ## Call" << std::endl;
    node->visit_children(this);
    std::cout << "   add $4, %esp" << std::endl;
    std::cout << "  ## End Call" << std::endl;

}

void CodeGenerator::visitIfElseNode(IfElseNode* node) {
    
    std::cout << "  ## If/Else" << std::endl;

    int thisLabel = nextLabel();

    //node->visit_children(this);
    
    node->expression->accept(this);

    std::cout << "  pop %eax" << std::endl;
    std::cout << "  cmp $0, %eax" << std::endl;
    std::cout << "  je else" << thisLabel << std::endl;
    
    //node->statement_list_1->accept(this);

    if (node->statement_list_1) 
    {
        for(std::list<StatementNode*>::iterator iter = node->statement_list_1->begin();
            iter != node->statement_list_1->end(); iter++) 
        {
            (*iter)->accept(this);
        }
    }

    std::cout << "  jmp endif" << thisLabel << "" << std::endl;
    std::cout << "  else" << thisLabel << ":" << std::endl;

    if (node->statement_list_2)
    {
        for(std::list<StatementNode*>::iterator iter = node->statement_list_2->begin();
            iter != node->statement_list_2->end(); iter++) 
        {
            (*iter)->accept(this);
        }
    }

    std::cout << "  endif" << thisLabel << ":" << std::endl;

    std::cout << "  ## End If/Else" << std::endl;
}

void CodeGenerator::visitWhileNode(WhileNode* node) {

    std::cout << "  ## While" << std::endl;

    int thisLabel = nextLabel();
    std::cout << "  while" << thisLabel << ":" << std::endl;

    node->expression->accept(this);

    std::cout << "  pop %eax" << std::endl;
    std::cout << "  cmp $0, %eax" << std::endl;
    std::cout << "  je endwhile" << thisLabel << std::endl;

    if (node->statement_list)
    {
        for(std::list<StatementNode*>::iterator iter = node->statement_list->begin();
            iter != node->statement_list->end(); iter++) 
        {
            (*iter)->accept(this);
        }
    }

    std::cout << "  jmp while" << thisLabel << std::endl;
    std::cout << "  endwhile" << thisLabel << ":" << std::endl;

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
    std::cout << "  mov $0, %edx" << std::endl;
    std::cout << "  cmp %ebx, %eax" << std::endl;
    std::cout << "  setg %dl" << std::endl;
    std::cout << "  push %edx" << std::endl;

    std::cout << "  ## End Greater" << std::endl;

}

void CodeGenerator::visitGreaterEqualNode(GreaterEqualNode* node) {

    std::cout << "  ## GreaterEqual" << std::endl;
    node->visit_children(this);

    std::cout << "  pop %ebx" << std::endl;
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  mov $0, %edx" << std::endl;
    std::cout << "  cmp %ebx, %eax" << std::endl;
    std::cout << "  setge %dl" << std::endl;
    std::cout << "  push %edx" << std::endl;

    std::cout << "  ## End GreaterEqual" << std::endl;
}

void CodeGenerator::visitEqualNode(EqualNode* node) {

    std::cout << "  ## Equal" << std::endl;
    node->visit_children(this);

    std::cout << "  pop %ebx" << std::endl;
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  mov $0, %edx" << std::endl;
    std::cout << "  cmp %ebx, %eax" << std::endl;
    std::cout << "  sete %dl" << std::endl;
    std::cout << "  push %edx" << std::endl;

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

    //memberOffsetIs
    std::string iden1 = node->identifier_1->name;
    std::string iden2;

    std::string callingClassName = "";
    std::string currentClassID = currentClassName;

    //for each member ahead, it would be 4
    //std::cout << "  # Start" << std::endl;
    auto members = this->classTable->find(currentClassName)->second.members;
    auto variables = this->currentMethodInfo.variables;

    //If the first part of a two-part method call is in the variable table
    int iden1isVariable = 0;

    int offset = 0;
    int maxSuperOffset = 0;

    //identify the class of the caller
    if(node->identifier_2 == NULL)
    {
        callingClassName = currentClassName;

        //find the first class with this method's name
        while( this->classTable->find(callingClassName) != this->classTable->end() )
        {
            std::cout << "  # " << callingClassName << std::endl;
            auto callingClass = this->classTable->find(callingClassName);

            auto methods = callingClass->second.methods;

            if( methods->find(iden1) == methods->end() )
            {
                callingClassName = callingClass->second.superClassName;
            }
            else
            {
                break;
            }
        }
    }
    else
    {
        iden2 = node->identifier_2->name;

        currentClassID = currentClassName;

        //look for the iden1 in the current variable table
        if( variables->find(iden1) != variables->end() )
        {
            iden1isVariable = 1;
            callingClassName = variables->find(iden1)->second.type.objectClassName;
        }

        //look for the iden1 in the current member table, if not found in var. table
        if( iden1isVariable == 0 )
        {
            while( members->find(iden1) != members->end() )
            {
                //std::cout << "  # " << currentClassID << std::endl;
                auto currentClass = this->classTable->find(currentClassID);

                members = currentClass->second.members;

                if( members->find(iden1) == members->end() )
                {
                    currentClassID = currentClass->second.superClassName;
                }
                else
                {
                    break;
                }
            }
            callingClassName = members->find(iden1)->second.type.objectClassName;
        }

        //find the first class with this method's name
        while( this->classTable->find(callingClassName) != this->classTable->end() )
        {
            //std::cout << "  # " << callingClassName << std::endl;
            auto callingClass = this->classTable->find(callingClassName);

            auto methods = callingClass->second.methods;

            if( methods->find(iden2) == methods->end() )
            {
                callingClassName = callingClass->second.superClassName;
            }
            else
            {
                break;
            }
        }
    }

    std::cout << "  ## MethodCall" << std::endl;

    std::cout << "  push %eax" << std::endl;
    std::cout << "  push %ecx" << std::endl;
    std::cout << "  push %edx" << std::endl;

    //print parameters before running method
    //node->expression_list->accept(this);

    if (node->expression_list)
    {
        for(std::list<ExpressionNode*>::iterator iter = node->expression_list->begin();
            iter != node->expression_list->end(); iter++) 
        {
            (*iter)->accept(this);
        }
    }

    //std::cout << "  push " << offset << "(%ebp)" << std::endl;
    std::cout << "  push -4(%ebp)" << std::endl;
    if( node->identifier_2 == NULL )
        std::cout << "  call " << callingClassName << "_" << iden1 << std::endl;
    else
        std::cout << "  call " << callingClassName << "_" << iden2 << std::endl;
    //std::cout << "  add $"<< -offset <<", %esp" << std::endl;
    std::cout << "  add $4, %esp" << std::endl;
    std::cout << "  push %edx" << std::endl;
    std::cout << "  push %ecx" << std::endl;
    std::cout << "  xchg (%esp), %eax" << std::endl;

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
        std::cout << "  push " << offset << "(%ebp)" << std::endl;
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
