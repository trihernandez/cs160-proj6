#include "codegeneration.hpp"

// CodeGenerator Visitor Functions: These are the functions
// you will complete to generate the x86 assembly code. Not
// all functions must have code, many may be left empty.

//Clarifications on @269 and @270

void CodeGenerator::visitProgramNode(ProgramNode* node) {
        
    //std::cout << "  ########### Program" << std::endl;
    std::cout << ".data" << std::endl;
    std::cout << "printstr: .asciz \"%d\\n\"" << std::endl;
    std::cout << "" << std::endl;
    std::cout << ".text" << std::endl;
    std::cout << ".globl Main_main" << std::endl;

    std::cout << "" << std::endl;


    node->visit_children(this);
    std::cout << "  ########### End Program" << std::endl;
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

    std::cout << "  # Class " << currentClassName << std::endl;

    node->visit_children(this);
    std::cout << "  # End Class " << currentClassName << std::endl;
}

void CodeGenerator::visitMethodNode(MethodNode* node) {

    currentMethodName = node->identifier->name;
    currentMethodInfo = currentClassInfo.methods->find(currentMethodName)->second;

    std::cout << "  # Method " << currentMethodName << std::endl;

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
    std::cout << "  # End Method " << currentMethodName << std::endl;

}

void CodeGenerator::visitMethodBodyNode(MethodBodyNode* node) {

    //std::cout << "  # MethodBody" << std::endl;
    node->visit_children(this);
    //std::cout << "  # End MethodBody" << std::endl;

}

void CodeGenerator::visitParameterNode(ParameterNode* node) {

    //std::cout << "  # Parameter" << std::endl;
    node->visit_children(this);
    //std::cout << "  # End Parameter" << std::endl;

}

void CodeGenerator::visitDeclarationNode(DeclarationNode* node) {

    int allocSize = 0; // size of the object that we want to allocate.  I think it's 4 + (4*numVariables)

    // std::cout << "  # Declaration" << std::endl;

    node->visit_children(this);
    // //taken from Disc. 10 slide 15
    // std::cout << "  push $" << allocSize << "       # size to allocate" << std::endl;
    // std::cout << "  call malloc   # call malloc with one arg" << std::endl;
    // std::cout << "  add  $4, %esp # remove args from stack" << std::endl;
    // std::cout << "  push %eax" << std::endl;

    // std::cout << "  # End Declaration" << std::endl;
}

void CodeGenerator::visitReturnStatementNode(ReturnStatementNode* node) {

    std::cout << "  # ReturnStatement" << std::endl;
    node->visit_children(this);
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  # End ReturnStatement" << std::endl;
}


void CodeGenerator::visitAssignmentNode(AssignmentNode* node) {

    std::string iden1 = node->identifier_1->name;
    std::string iden2;

    BaseType iden1type = bt_none;
    std::string iden1className = "";

    std::string leftClassName = "";

    VariableTable* currentVariableTable = currentMethodInfo.variables;

    int offset1 = 0;
    int offset2 = 0;

    int iden1IsVariable = 0;
    int iden1IsFound = 0;

    int iden2IsVariable = 0;
    int iden2IsFound = 0;

    std::map<std::string, ClassInfo>::iterator currentClass;
    
    // std::cout << " # 137" << std::endl;

    std::string thisVariableString;
    VariableInfo thisVariableInfo;

        // std::cout << " # 142" << std::endl;
        //first, check if iden1 is in the currentVariableTable

        auto thisVariable = currentVariableTable->find(iden1);

        if( thisVariable != currentVariableTable->end() )
        {
            thisVariableString = thisVariable->first;
            thisVariableInfo = thisVariable->second;
            iden1className = thisVariableInfo.type.objectClassName;
            offset1 = thisVariableInfo.offset;
            iden1IsVariable = 1;
        }

        // std::cout << " # 156" << std::endl;
        //else check if iden1 is still a member of the current class
        currentClass = classTable->find(currentClassName);
        if( iden1IsVariable == 0 && currentClass != classTable->end() && currentClass->second.members != NULL )
        {
            thisVariable = currentClass->second.members->find(iden1);
            if( thisVariable != currentClass->second.members->end() )
            {
                thisVariableString = thisVariable->first;
                thisVariableInfo = thisVariable->second;
                iden1className = thisVariableInfo.type.objectClassName;
                offset1 = thisVariableInfo.offset;
                iden1IsFound = 1;
            }
        }

        // std::cout << " # 176" << std::endl;
        //else check if iden1 is still a member of the superclass of the current class

        if( iden1IsVariable == 0 && currentClass != classTable->end() )
        {
            std::string currentSuperClassName = currentClass->second.superClassName;
            currentClass = classTable->find(currentSuperClassName);

            if( currentClass != classTable->end() && currentClass->second.members != NULL )
            {
                thisVariable = currentClass->second.members->find(iden1);
                if( iden1IsFound == 0 && thisVariable != currentClass->second.members->end() )
                {
                    thisVariableString = thisVariable->first;
                    thisVariableInfo = thisVariable->second;
                    iden1className = thisVariableInfo.type.objectClassName;
                    offset1 = thisVariableInfo.offset;
                    iden1IsFound = 1;
                }
                else
                {
                    offset1 += 4 * ( currentClass->second.members->size() );
                }
            }
        }

    if( node->identifier_2 )
    {
        iden2 = node->identifier_2->name;

        // std::cout << " # iden1className = " << iden1className << std::endl;
        // std::cout << " # currentClass = " << currentClass->first << std::endl;
        // std::cout << " # 205; offset2 = " << offset2 << std::endl;

        currentClass = classTable->find(iden1className);
        thisVariable = currentClass->second.members->find(iden2);

        //else check if iden1 is still a member of the current class
        if( currentClass != classTable->end() && currentClass->second.members != NULL )
        {
            // std::cout << " # 213; offset2 = " << offset2 << std::endl;
            thisVariable = currentClass->second.members->find(iden2);
            if( thisVariable != currentClass->second.members->end() )
            {
                // std::cout << " # 217; offset2 = " << offset2 << std::endl;
                thisVariableString = thisVariable->first;
                thisVariableInfo = thisVariable->second;
                offset2 = thisVariableInfo.offset;
                iden2IsFound = 1;
            }
        }

        // std::cout << " # 230; offset2 = " << offset2 << std::endl;
        //else check if iden1 is still a member of the superclass of the current class
        if( currentClass != classTable->end() )
        {
            // std::cout << " # 234; offset2 = " << offset2 << std::endl;
            std::string currentSuperClassName = currentClass->second.superClassName;
            currentClass = classTable->find(currentSuperClassName);

            if( currentClass != classTable->end() && currentClass->second.members != NULL )
            {
                // std::cout << " # 240; offset2 = " << offset2 << std::endl;
                thisVariable = currentClass->second.members->find(iden2);
                if( iden2IsFound == 0 && thisVariable != currentClass->second.members->end() )
                {
                    // std::cout << " # 244; offset2 = " << offset2 << std::endl;
                    thisVariableString = thisVariable->first;
                    thisVariableInfo = thisVariable->second;
                    offset2 = thisVariableInfo.offset;
                    iden2IsFound = 1;
                }
                else
                {
                    // std::cout << " # 252; offset2 = " << offset2 << std::endl;
                    offset2 += 4 * ( currentClass->second.members->size() );
                }
            }
        }
        // std::cout << " # 257; offset2 = " << offset2 << std::endl;
    }

    //work on an offset calculator

    std::cout << "  # Assignment" << std::endl;
    node->visit_children(this);

    if( node->identifier_2 != NULL )
    {
        std::cout << "  mov " << offset1 << "(%ebp), %ebx" << std::endl;
        std::cout << "  pop " << offset2 << "(%ebx)" << std::endl;
    }
    else
    {
        if(iden1IsVariable == 0)
        {
            std::cout << "  mov 8(%ebp), %ebx" << std::endl;
            std::cout << "  pop " << offset1 << "(%ebx)" << std::endl;
        }
        else
        {
            std::cout << "  pop " << offset1 << "(%ebp)" << std::endl;
        }
    }

    std::cout << "  # End Assignment" << std::endl;
}


void CodeGenerator::visitCallNode(CallNode* node) {

    std::cout << "  # Call" << std::endl;
    node->visit_children(this);
    std::cout << "   add $4, %esp" << std::endl;
    std::cout << "  # End Call" << std::endl;

}

void CodeGenerator::visitIfElseNode(IfElseNode* node) {
    
    std::cout << "  # If-Else" << std::endl;

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

    std::cout << "  # End If-Else" << std::endl;
}

void CodeGenerator::visitWhileNode(WhileNode* node) {

    std::cout << "  # While" << std::endl;

    
    int thisLabel = nextLabel();
    std::cout << "  while" << thisLabel << ":" << std::endl;

    node->expression->accept(this);

    std::cout << "  pop %eax" << std::endl;
    std::cout << "  cmp $0, %eax" << std::endl;
    std::cout << "  je endwhile" << thisLabel << std::endl;

    if (node->statement_list != NULL)
    {
        for(std::list<StatementNode*>::iterator iter = node->statement_list->begin();
            iter != node->statement_list->end(); iter++) 
        {
            (*iter)->accept(this);
        }
    }

    std::cout << "  jmp while" << thisLabel << std::endl;
    std::cout << "  endwhile" << thisLabel << ":" << std::endl;
    

    std::cout << "  # End While" << std::endl;

}


void CodeGenerator::visitPrintNode(PrintNode* node) {
    
    std::cout << "  # Print" << std::endl;

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

    std::cout << "  # End Print" << std::endl;
}


void CodeGenerator::visitDoWhileNode(DoWhileNode* node) {

    int thisLabel = nextLabel();

    std::cout << "  # Do-While" << std::endl;

    std::cout << "  dowhile" << thisLabel << ":" << std::endl;
    
    if (node->statement_list)
    {
        for(std::list<StatementNode*>::iterator iter = node->statement_list->begin();
            iter != node->statement_list->end(); iter++) 
        {
            (*iter)->accept(this);
        }
    }

    node->expression->accept(this);

    std::cout << "  pop %eax" << std::endl;
    std::cout << "  cmp $0, %eax" << std::endl;
    std::cout << "  jne dowhile" << thisLabel << std::endl;

    std::cout << "  # End Do-While" << std::endl;

}

void CodeGenerator::visitPlusNode(PlusNode* node) {

    std::cout << "  # Plus" << std::endl;

    node->visit_children(this);

    std::cout << "  pop %ebx" << std::endl;
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  add %ebx, %eax" << std::endl;
    std::cout << "  push %eax" << std::endl;

    std::cout << "  # End Plus" << std::endl;
}

void CodeGenerator::visitMinusNode(MinusNode* node) {

    std::cout << "  # Minus" << std::endl;

    node->visit_children(this);

    std::cout << "  pop %ebx" << std::endl;
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  sub %ebx, %eax" << std::endl;
    std::cout << "  push %eax" << std::endl;

    std::cout << "  # End Minus" << std::endl;
}

void CodeGenerator::visitTimesNode(TimesNode* node) {

    std::cout << "  # Times" << std::endl;
    node->visit_children(this);
    std::cout << "  pop %ebx" << std::endl;
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  imul %ebx, %eax" << std::endl;
    std::cout << "  push %eax" << std::endl;

    std::cout << "  # End Times" << std::endl;

}

void CodeGenerator::visitDivideNode(DivideNode* node) {

    //ask the professor
    std::cout << "  # Divide" << std::endl;
    node->visit_children(this);
    std::cout << "  pop %ebx" << std::endl;
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  cdq" << std::endl;
    std::cout << "  idiv %ebx" << std::endl;
    std::cout << "  push %eax" << std::endl;

    std::cout << "  # End Divide" << std::endl;

}

void CodeGenerator::visitGreaterNode(GreaterNode* node) {

    std::cout << "  # Greater" << std::endl;
    node->visit_children(this);
    
    std::cout << "  pop %ebx" << std::endl;
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  mov $0, %edx" << std::endl;
    std::cout << "  cmp %ebx, %eax" << std::endl;
    std::cout << "  setg %dl" << std::endl;
    std::cout << "  push %edx" << std::endl;

    std::cout << "  # End Greater" << std::endl;

}

void CodeGenerator::visitGreaterEqualNode(GreaterEqualNode* node) {

    std::cout << "  # GreaterEqual" << std::endl;
    node->visit_children(this);

    std::cout << "  pop %ebx" << std::endl;
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  mov $0, %edx" << std::endl;
    std::cout << "  cmp %ebx, %eax" << std::endl;
    std::cout << "  setge %dl" << std::endl;
    std::cout << "  push %edx" << std::endl;

    std::cout << "  # End GreaterEqual" << std::endl;
}

void CodeGenerator::visitEqualNode(EqualNode* node) {

    std::cout << "  # Equal" << std::endl;
    node->visit_children(this);

    std::cout << "  pop %ebx" << std::endl;
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  mov $0, %edx" << std::endl;
    std::cout << "  cmp %ebx, %eax" << std::endl;
    std::cout << "  sete %dl" << std::endl;
    std::cout << "  push %edx" << std::endl;

    std::cout << "  # End Equal" << std::endl;
}

void CodeGenerator::visitAndNode(AndNode* node) {

    std::cout << "  # And" << std::endl;
    node->visit_children(this);
    std::cout << "  pop %ebx" << std::endl;
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  and %ebx, %eax" << std::endl;
    std::cout << "  push %eax" << std::endl;
    std::cout << "  # End And" << std::endl;
}

void CodeGenerator::visitOrNode(OrNode* node) {

    std::cout << "  # Or" << std::endl;
    node->visit_children(this);
    std::cout << "  pop %ebx" << std::endl;
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  or %ebx, %eax" << std::endl;
    std::cout << "  push %eax" << std::endl;
    std::cout << "  # End Or" << std::endl;
}

void CodeGenerator::visitNotNode(NotNode* node) {

    std::cout << "  # Not" << std::endl;
    node->visit_children(this);
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  not %eax" << std::endl;
    std::cout << "  push %eax" << std::endl;
    std::cout << "  # End Not" << std::endl;
}

void CodeGenerator::visitNegationNode(NegationNode* node) {
    
    std::cout << "  # Negation" << std::endl;
    node->visit_children(this);
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  neg %eax" << std::endl;
    std::cout << "  push %eax" << std::endl;

    std::cout << "  # End Negation" << std::endl;
}




void CodeGenerator::visitMethodCallNode(MethodCallNode* node) {

/*
    //memberOffsetIs
    std::string iden1 = node->identifier_1->name;
    std::string iden2;

    std::string iden1className = "";

    int offset = 0;
    //find the lowest offset of a parameter
    int nextLowestOffest = 12;
    int printedOffset = -4;

    auto currentMethodTable = currentClassInfo.methods;
    auto currentVariableTable = currentMethodInfo.variables;

    MethodInfo thisMethod;

    if(node->identifier_2 == NULL)
    {
        iden1className = currentClassName;

        while(true)
        {
            //first, check if iden1 is in the currentVariableTable
            auto MethodTableSearch = currentMethodTable->find(iden1);
            if( MethodTableSearch != currentMethodTable->end() )
            {
                thisMethod = MethodTableSearch->second;
                break;
            }

            std::map<std::string, ClassInfo>::iterator currentClass = classTable->find(currentClassName);

            //else check if iden1 is still a method of the superclass of the current class
            if( currentClass != classTable->end() )
            {
                std::string currentSuperClassName = currentClass->second.superClassName;
                std::map<std::string, ClassInfo>::iterator currentSuperClass = classTable->find(currentSuperClassName);

                if( currentSuperClass != classTable->end() && currentSuperClass->second.methods != NULL )
                {
                    std::map<std::string, MethodInfo>::iterator superMethodTableSearch = currentSuperClass->second.methods->find(iden1);
                    if( superMethodTableSearch != currentSuperClass->second.methods->end() )
                    {
                        iden1className = currentSuperClass->first;
                        thisMethod = superMethodTableSearch->second;
                        break;
                    }
                }
            }
        }
    }
    else
    {
        //  if identifier_2 does exist, check if identifier_1 is in the currentVariableTable as a defined class
        //  and then check if identifier_2 is in the class' method table
        iden2 = node->identifier_2->name;

        while(true)
        {
            //first, check if iden1 is in the currentVariableTable
            std::map<std::string, VariableInfo>::iterator variableTableSearch = currentVariableTable->find(iden1);
            if( variableTableSearch != currentVariableTable->end() )
            {
                iden1className = variableTableSearch->second.type.objectClassName;
                break;
            }

            //else check if iden1 is still a member of the current class
            std::map<std::string, ClassInfo>::iterator currentClass = classTable->find(currentClassName);
            if( currentClass != classTable->end() && currentClass->second.members != NULL )
            {
                std::map<std::string, VariableInfo>::iterator memberTableSearch = currentClass->second.members->find(iden1);
                if( memberTableSearch != currentClass->second.members->end() )
                {
                    //iden1className = memberTableSearch->second.type.objectClassName;
                    //iden1className = currentSuperClass->first;
                    iden1className = currentClass->first;
                    break;
                }
            }
         
            //check if the superclass of the current class exists
            if( currentClass != classTable->end() )
            {
                std::string currentSuperClassName = currentClass->second.superClassName;
                std::map<std::string, ClassInfo>::iterator currentSuperClass = classTable->find(currentSuperClassName);
                if( currentSuperClass != classTable->end() )
                {
                    std::map<std::string, VariableInfo>::iterator superMemberTableSearch = currentSuperClass->second.members->find(iden1);
                    if( superMemberTableSearch != currentSuperClass->second.members->end() )
                    {
                        iden1className = currentSuperClass->first;
                        break;
                    }
                }
            }
        }

        //check if the class type of identifier_1 is in the classTable
        std::map<std::string, ClassInfo>::iterator iden1Class = classTable->find(iden1className);

        //make sure the iden2 is a method in the class of iden1, else return error
        std::map<std::string, MethodInfo>::iterator iden1MethodTableSearch = iden1Class->second.methods->find(iden2);
        if( iden1MethodTableSearch != iden1Class->second.methods->end() )
        {
            thisMethod = iden1MethodTableSearch->second;
        }
        else
        {
            //check if iden2 is a method in the superclass of iden1
            std::string iden1superClassName = iden1Class->second.superClassName;

            //iden1className = iden1superClassName;

            std::map<std::string, ClassInfo>::iterator iden1superClass = classTable->find(iden1superClassName);
            if( iden1superClass != classTable->end() )
            {
                std::map<std::string, MethodInfo>::iterator iden1SuperMethodTableSearch = iden1superClass->second.methods->find(iden2);
                if( iden1SuperMethodTableSearch != iden1superClass->second.methods->end() )
                {
                    thisMethod = iden1SuperMethodTableSearch->second;
                }
                else
                {
                }
            }
            else
            {
            }
        }
    }
*/
    std::string iden1 = node->identifier_1->name;
    std::string iden2;

    BaseType iden1type = bt_none;
    std::string iden1className = "";
    std::string iden2className = "";

    VariableTable* currentVariableTable = currentMethodInfo.variables;

    int offset1 = 0;

    int iden1IsVariable = 0;
    int iden1IsFound = 0;

    int iden2IsVariable = 0;
    int iden2IsFound = 0;

    std::map<std::string, ClassInfo>::iterator currentClass;
    
    // std::cout << " # 137" << std::endl;

    std::string thisVariableString;
    VariableInfo thisVariableInfo;

    std::string thisMethodString;
    MethodInfo thisMethodInfo;

        // std::cout << " # 142" << std::endl;
        //first, check if iden1 is in the currentVariableTable

    if( node->identifier_2 != NULL )
    {
        iden2 = node->identifier_2->name;

        auto thisVariable = currentVariableTable->find(iden1);

        if( thisVariable != currentVariableTable->end() )
        {
            thisVariableString = thisVariable->first;
            thisVariableInfo = thisVariable->second;
            iden1className = thisVariableInfo.type.objectClassName;
            offset1 = thisVariableInfo.offset;
            iden1IsVariable = 1;
        }

        // std::cout << " # 156" << std::endl;
        //else check if iden1 is still a member of the current class
        currentClass = classTable->find(currentClassName);
        if( iden1IsVariable == 0 && currentClass != classTable->end() && currentClass->second.members != NULL )
        {
            thisVariable = currentClass->second.members->find(iden1);
            if( thisVariable != currentClass->second.members->end() )
            {
                thisVariableString = thisVariable->first;
                thisVariableInfo = thisVariable->second;
                iden1className = thisVariableInfo.type.objectClassName;
                offset1 = thisVariableInfo.offset;
                iden1IsFound = 1;
            }
        }

        // std::cout << " # 176" << std::endl;
        //else check if iden1 is still a member of the superclass of the current class

        if( iden1IsVariable == 0 && currentClass != classTable->end() )
        {
            std::string currentSuperClassName = currentClass->second.superClassName;
            currentClass = classTable->find(currentSuperClassName);

            if( currentClass != classTable->end() && currentClass->second.members != NULL )
            {
                thisVariable = currentClass->second.members->find(iden1);
                if( iden1IsFound == 0 && thisVariable != currentClass->second.members->end() )
                {
                    thisVariableString = thisVariable->first;
                    thisVariableInfo = thisVariable->second;
                    iden1className = thisVariableInfo.type.objectClassName;
                    offset1 = thisVariableInfo.offset;
                    iden1IsFound = 1;
                }
                else
                {
                    offset1 += 4 * ( currentClass->second.members->size() );
                }
            }
        }

        // std::cout << " # iden1className = " << iden1className << std::endl;
        // std::cout << " # currentClass = " << currentClass->first << std::endl;
        // std::cout << " # 205; offset2 = " << offset2 << std::endl;

        currentClass = classTable->find(iden1className);
        auto thisMethod = currentClass->second.methods->find(iden2);

        //else check if iden1 is still a member of the current class
        if( currentClass != classTable->end() && currentClass->second.methods != NULL )
        {
            // std::cout << " # 213; offset2 = " << offset2 << std::endl;
            thisMethod = currentClass->second.methods->find(iden2);
            if( thisMethod != currentClass->second.methods->end() )
            {
                // std::cout << " # 217; offset2 = " << offset2 << std::endl;
                iden2className = currentClass->first;
                thisMethodString = thisMethod->first;
                thisMethodInfo = thisMethod->second;
                iden2IsFound = 1;
            }
        }

        // std::cout << " # 230; offset2 = " << offset2 << std::endl;
        //else check if iden1 is still a member of the superclass of the current class
        if( currentClass != classTable->end() )
        {
            // std::cout << " # 234; offset2 = " << offset2 << std::endl;
            std::string currentSuperClassName = currentClass->second.superClassName;
            currentClass = classTable->find(currentSuperClassName);

            if( currentClass != classTable->end() && currentClass->second.methods != NULL )
            {
                // std::cout << " # 240; offset2 = " << offset2 << std::endl;
                thisMethod = currentClass->second.methods->find(iden2);
                if( iden2IsFound == 0 && thisMethod != currentClass->second.methods->end() )
                {
                    // std::cout << " # 244; offset2 = " << offset2 << std::endl;
                    iden2className = currentClass->first;
                    thisMethodString = thisMethod->first;
                    thisMethodInfo = thisMethod->second;
                    iden2IsFound = 1;
                }
            }
        }
        // std::cout << " # 257; offset2 = " << offset2 << std::endl;
    }
    else
    {
        offset1 = 0;

        currentClass = classTable->find(currentClassName);
        auto thisMethod = currentClass->second.methods->find(iden2);

        //std::cout << "      # 839; currentClass = " << currentClass->first << std::endl;
        //std::cout << "      # 840; currentClassName = " << currentClassName << std::endl;
        //std::cout << "      # 841; iden2className = " << iden2className << std::endl;

        //else check if iden1 is still a member of the current class
        if( currentClass != classTable->end() && currentClass->second.methods != NULL )
        {
            // std::cout << " # 213; offset2 = " << offset2 << std::endl;
            thisMethod = currentClass->second.methods->find(iden1);
            if( thisMethod != currentClass->second.methods->end() )
            {
                        //std::cout << "      # 839; currentClass = " << currentClass->first << std::endl;
                        //std::cout << "      # 851; currentClassName = " << currentClassName << std::endl;
                        //std::cout << "      # 852; iden2className = " << iden2className << std::endl;
                iden2className = currentClass->first;
                thisMethodString = thisMethod->first;
                thisMethodInfo = thisMethod->second;
                iden2IsFound = 1;
            }
        }

        // std::cout << " # 230; offset2 = " << offset2 << std::endl;
        //else check if iden1 is still a member of the superclass of the current class
        if( currentClass != classTable->end() )
        {
            // std::cout << " # 234; offset2 = " << offset2 << std::endl;
            std::string currentSuperClassName = currentClass->second.superClassName;
            currentClass = classTable->find(currentSuperClassName);

            if( currentClass != classTable->end() && currentClass->second.methods != NULL )
            {
                // std::cout << " # 240; offset2 = " << offset2 << std::endl;
                thisMethod = currentClass->second.methods->find(iden1);
                if( iden2IsFound == 0 && thisMethod != currentClass->second.methods->end() )
                {
                    // std::cout << " # 244; offset2 = " << offset2 << std::endl;
                    iden2className = currentClass->first;
                    thisMethodString = thisMethod->first;
                    thisMethodInfo = thisMethod->second;
                    iden2IsFound = 1;
                }
            }
        }
    }

    //obtain method information
    
    //auto thisMethod = methods->find(iden1);
    //if(node->identifier_2 != NULL)
    //        thisMethod = methods->find(iden2);

    /*
    for( auto it = thisMethod.variables->begin();
            it != thisMethod.variables->end(); it++ )
    {
        if( (it)->second.offset == nextLowestOffest )
        {
            offset = nextLowestOffest;
            nextLowestOffest += 4;
            // if (node->expression_list)
            // {
            //     for(std::list<ExpressionNode*>::iterator it2 = node->expression_list->begin();
            //         it2 != node->expression_list->end(); it2++) 
            //     {
            //         if( iden1isVariable == 23 ) //always false
            //         {
            //             (*it2)->accept(this);
            //         }
            //     }
            // }
        }
    }
    */

    //printedOffset = offset-4;

    std::cout << "  # MethodCall" << std::endl;

    std::cout << "  push %eax" << std::endl;
    std::cout << "  push %ecx" << std::endl;
    std::cout << "  push %edx" << std::endl;

    int addOffset = 0;

    if (node->expression_list)
    {
        for(std::list<ExpressionNode*>::reverse_iterator iter = node->expression_list->rbegin();
            iter != node->expression_list->rend(); iter++) 
        {
            addOffset += 4;
            (*iter)->accept(this);
        }
    }

    //std::cout << "  push " << offset << "(%ebp)" << std::endl;
    std::cout << "  push " << offset1 << "(%ebp)" << std::endl;

    if( node->identifier_2 == NULL )
        std::cout << "  call " << iden2className << "_" << iden1 << std::endl;
    else
        std::cout << "  call " << iden2className << "_" << iden2 << std::endl;
    //std::cout << "  add $"<< -offset <<", %esp" << std::endl;

    std::cout << "  add $"<< addOffset <<", %esp" << std::endl;
    std::cout << "  pop %edx" << std::endl;
    std::cout << "  pop %ecx" << std::endl;
    std::cout << "  xchg (%esp), %eax" << std::endl;

    std::cout << "  # End MethodCall" << std::endl;

}

void CodeGenerator::visitMemberAccessNode(MemberAccessNode* node) {


    std::string iden1 = node->identifier_1->name;
    std::string iden2 = node->identifier_2->name;

    //BaseType iden1type = bt_none;
    std::string iden1className = "";

    VariableTable* currentVariableTable = currentMethodInfo.variables;

    int offset1 = 0;
    int offset2 = 0;

    int iden1IsVariable = 0;
    int iden1IsFound = 0;

    int iden2IsVariable = 0;
    int iden2IsFound = 0;

    std::map<std::string, ClassInfo>::iterator currentClass;
    
    // std::cout << " # 137" << std::endl;

    std::string thisVariableString;
    VariableInfo thisVariableInfo;

        // std::cout << " # 142" << std::endl;
        //first, check if iden1 is in the currentVariableTable

        auto thisVariable = currentVariableTable->find(iden1);

        if( thisVariable != currentVariableTable->end() )
        {
            thisVariableString = thisVariable->first;
            thisVariableInfo = thisVariable->second;
            iden1className = thisVariableInfo.type.objectClassName;
            offset1 = thisVariableInfo.offset;
            iden1IsVariable = 1;
        }

        // std::cout << " # 156" << std::endl;
        //else check if iden1 is still a member of the current class
        currentClass = classTable->find(currentClassName);
        if( iden1IsVariable == 0 && currentClass != classTable->end() && currentClass->second.members != NULL )
        {
            thisVariable = currentClass->second.members->find(iden1);
            if( thisVariable != currentClass->second.members->end() )
            {
                thisVariableString = thisVariable->first;
                thisVariableInfo = thisVariable->second;
                iden1className = thisVariableInfo.type.objectClassName;
                offset1 = thisVariableInfo.offset;
                iden1IsFound = 1;
            }
        }

        // std::cout << " # 176" << std::endl;
        //else check if iden1 is still a member of the superclass of the current class

        if( iden1IsVariable == 0 && currentClass != classTable->end() )
        {
            std::string currentSuperClassName = currentClass->second.superClassName;
            currentClass = classTable->find(currentSuperClassName);

            if( currentClass != classTable->end() && currentClass->second.members != NULL )
            {
                thisVariable = currentClass->second.members->find(iden1);
                if( iden1IsFound == 0 && thisVariable != currentClass->second.members->end() )
                {
                    thisVariableString = thisVariable->first;
                    thisVariableInfo = thisVariable->second;
                    iden1className = thisVariableInfo.type.objectClassName;
                    offset1 = thisVariableInfo.offset;
                    iden1IsFound = 1;
                }
                else
                {
                    offset1 += 4 * ( currentClass->second.members->size() );
                }
            }
        }

        //iden2 = node->identifier_2->name;

        // std::cout << " # iden1className = " << iden1className << std::endl;
        // std::cout << " # currentClass = " << currentClass->first << std::endl;
        // std::cout << " # 205; offset2 = " << offset2 << std::endl;

        currentClass = classTable->find(iden1className);
        thisVariable = currentClass->second.members->find(iden2);

        //else check if iden1 is still a member of the current class
        if( currentClass != classTable->end() && currentClass->second.members != NULL )
        {
            // std::cout << " # 213; offset2 = " << offset2 << std::endl;
            thisVariable = currentClass->second.members->find(iden2);
            if( thisVariable != currentClass->second.members->end() )
            {
                // std::cout << " # 217; offset2 = " << offset2 << std::endl;
                thisVariableString = thisVariable->first;
                thisVariableInfo = thisVariable->second;
                offset2 = thisVariableInfo.offset;
                iden2IsFound = 1;
            }
        }

        // std::cout << " # 230; offset2 = " << offset2 << std::endl;
        //else check if iden1 is still a member of the superclass of the current class
        if( currentClass != classTable->end() )
        {
            // std::cout << " # 234; offset2 = " << offset2 << std::endl;
            std::string currentSuperClassName = currentClass->second.superClassName;
            currentClass = classTable->find(currentSuperClassName);

            if( currentClass != classTable->end() && currentClass->second.members != NULL )
            {
                // std::cout << " # 240; offset2 = " << offset2 << std::endl;
                thisVariable = currentClass->second.members->find(iden2);
                if( iden2IsFound == 0 && thisVariable != currentClass->second.members->end() )
                {
                    // std::cout << " # 244; offset2 = " << offset2 << std::endl;
                    thisVariableString = thisVariable->first;
                    thisVariableInfo = thisVariable->second;
                    offset2 = thisVariableInfo.offset;
                    iden2IsFound = 1;
                }
                else
                {
                    // std::cout << " # 252; offset2 = " << offset2 << std::endl;
                    offset2 += 4 * ( currentClass->second.members->size() );
                }
            }
        }
        // std::cout << " # 257; offset2 = " << offset2 << std::endl;

    std::cout << "  # Member Access " << iden1 << "." << iden2 << std::endl;

    std::cout << "  mov " << offset1 << "(%ebp), %ebx" << std::endl;
    std::cout << "  push " << offset2 << "(%ebx)" << std::endl;

    std::cout << "  # End Member Access " << iden1 << "." << iden2 << std::endl;

}




void CodeGenerator::visitVariableNode(VariableNode* node) {
    
    //VariableInfo variable;
    std::string variable = node->identifier->name;

    int offset = 0;

    std::cout << "  # Variable " << variable << std::endl;
    node->visit_children(this);
    if( currentMethodInfo.variables->find(variable) != currentMethodInfo.variables->end() )
    {        
        offset = currentMethodInfo.variables->at(variable).offset;
        std::cout << "  push " << offset << "(%ebp)" << std::endl;
    }
    else
    {
        std::cout << "  mov 8(%ebp), %ebx" << std::endl;

        auto currentClass = classTable->find(currentClassName);
        while( currentClass != classTable->end() )
        {
            auto thisClassInfo = currentClass->second;

            if( thisClassInfo.members->find(variable) != thisClassInfo.members->end() )
            {
                offset = thisClassInfo.members->at(variable).offset;
                std::cout << "  push " << offset << "(%ebx)" << std::endl;
            }
            std::string currentSuperClassName = thisClassInfo.superClassName;
            currentClass = classTable->find(currentSuperClassName);
        }
    }
    std::cout << "  # End Variable " << variable << std::endl;
}


void CodeGenerator::visitIntegerLiteralNode(IntegerLiteralNode* node) {

    std::cout << "  # Integer Literal" << std::endl;
    node->visit_children(this);
    std::cout << "  push $" << node->integer->value << std::endl;
    std::cout << "  # End Integer Literal" << std::endl;
}

void CodeGenerator::visitBooleanLiteralNode(BooleanLiteralNode* node) {
    std::cout << "  # BoolLiteral" << std::endl;
    node->visit_children(this);
    std::cout << "  push $" << node->integer->value << std::endl;
    std::cout << "  # End BoolLiteral" << std::endl;
}

void CodeGenerator::visitNewNode(NewNode* node) {

    std::string newClass = node->identifier->name;

    ClassInfo newClassInfo = classTable->find(newClass)->second;
    int numMembers = newClassInfo.members->size();
    int numParameters = 1;

    auto currentClass = classTable->find(newClass);

    //else check if iden1 is still a member of the superclass of the current class
    while (currentClass != classTable->end())
    {
        std::string currentSuperClassName = currentClass->second.superClassName;
        currentClass = classTable->find(currentSuperClassName);

        if( currentClass != classTable->end() && currentClass->second.members != NULL )
        {
            numMembers = numMembers + currentClass->second.members->size();
        }
    }

    std::cout << " # NEW" << std::endl;

    std::cout << " push $" << (4 * numMembers) << std::endl;
    std::cout << " call malloc" << std::endl;
    std::cout << " add $4, %esp" << std::endl;

    //look for constructor, and do associated things if it exists
    auto constructor = newClassInfo.methods->find(newClass);
    if( constructor != newClassInfo.methods->end() )
    {
        std::cout << " mov %eax, %edi" << std::endl;
        std::cout << " push %eax" << std::endl;
        std::cout << " push %ecx" << std::endl;
        std::cout << " push %edx" << std::endl;

        if (node->expression_list != NULL)
        {
            for(std::list<ExpressionNode*>::reverse_iterator iter = node->expression_list->rbegin();
                iter != node->expression_list->rend(); iter++) 
            {
                numParameters++;
                (*iter)->accept(this);
            }
        }

        std::cout << " push %edi" << std::endl;
        std::cout << " call " << newClass << "_" << newClass << std::endl;
        std::cout << " add $" << (4 * numParameters) << ", %esp" << std::endl;

        std::cout << " pop %edx" << std::endl;
        std::cout << " pop %ecx" << std::endl;
        std::cout << " pop %eax" << std::endl;
        std::cout << " push %edi" << std::endl;


    }
    //otherwise, the new lass does not have a constructor
    else
    {
        std::cout << " push %eax" << std::endl;
    }
    //std::cout << "  # End New" << std::endl;
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
