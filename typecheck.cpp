#include "typecheck.hpp"

// Defines the function used to throw type errors. The possible
// type errors are defined as an enumeration in the header file.
void typeError(TypeErrorCode code) {
    switch (code) {
        case undefined_variable:
            std::cerr << "Undefined variable." << std::endl;
            break;
        case undefined_method:
            std::cerr << "Method does not exist." << std::endl;
            break;
        case undefined_class:
            std::cerr << "Class does not exist." << std::endl;
            break;
        case undefined_member:
            std::cerr << "Class member does not exist." << std::endl;
            break;
        case not_object:
            std::cerr << "Variable is not an object." << std::endl;
            break;
        case expression_type_mismatch:
            std::cerr << "Expression types do not match." << std::endl;
            break;
        case argument_number_mismatch:
            std::cerr << "Method called with incorrect number of arguments." << std::endl;
            break;
        case argument_type_mismatch:
            std::cerr << "Method called with argument of incorrect type." << std::endl;
            break;
        case while_predicate_type_mismatch:
            std::cerr << "Predicate of while loop is not boolean." << std::endl;
            break;
        case do_while_predicate_type_mismatch:
            std::cerr << "Predicate of do while loop is not boolean." << std::endl;
            break;
        case if_predicate_type_mismatch:
            std::cerr << "Predicate of if statement is not boolean." << std::endl;
            break;
        case assignment_type_mismatch:
            std::cerr << "Left and right hand sides of assignment types mismatch." << std::endl;
            break;
        case return_type_mismatch:
            std::cerr << "Return statement type does not match declared return type." << std::endl;
            break;
        case constructor_returns_type:
            std::cerr << "Class constructor returns a value." << std::endl;
            break;
        case no_main_class:
            std::cerr << "The \"Main\" class was not found." << std::endl;
            break;
        case main_class_members_present:
            std::cerr << "The \"Main\" class has members." << std::endl;
            break;
        case no_main_method:
            std::cerr << "The \"Main\" class does not have a \"main\" method." << std::endl;
            break;
        case main_method_incorrect_signature:
            std::cerr << "The \"main\" method of the \"Main\" class has an incorrect signature." << std::endl;
            break;
    }
    exit(1);
}

// TypeCheck Visitor Functions: These are the functions you will
// complete to build the symbol table and type check the program.
// Not all functions must have code, many may be left empty.

void TypeCheck::visitProgramNode(ProgramNode* node) {

    classTable = new std::map<std::string, ClassInfo>();

    node->visit_children(this);

    std::map<std::string, ClassInfo>::iterator MainClass = classTable->find("Main");
    if(MainClass==classTable->end()){
        typeError(no_main_class);
    }
    if(MainClass->second.methods->find("main") == MainClass->second.methods->end()){
        typeError(no_main_method);
    }
}

void TypeCheck::visitClassNode(ClassNode* node) {

    ClassInfo cInfo;

    if( node->identifier_2 == NULL)
        cInfo.superClassName = "";
    else
    {
        cInfo.superClassName = node->identifier_2->name;
        if( classTable->find( cInfo.superClassName ) == classTable->end() )
            typeError(undefined_class);
    }

    currentClassName = node->identifier_1->name;
    //printf("------new class ------\n");

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
    if(currentClassName == "Main" && currentMemberOffset != 0){
        typeError(main_class_members_present);
    }

    //insert a class into the table with only members and a superclass name
    classTable->insert( std::pair<std::string, ClassInfo>(currentClassName,cInfo) );

    currentMethodTable = new std::map<std::string, MethodInfo>();
    cInfo.methods = currentMethodTable;
    std::list<MethodNode*>* MethodNodeList = node->method_list;
    for (auto it = MethodNodeList->begin(); it!=MethodNodeList->end(); it++)
    {
        visitMethodNode( *it );
    }

    //take out the old class and re-insert it with all method data.
    classTable->erase( currentClassName );
    classTable->insert( std::pair<std::string, ClassInfo>(currentClassName,cInfo) );
}


void TypeCheck::visitMethodNode(MethodNode* node) {

    currentVariableTable = new std::map<std::string, VariableInfo>();
    //since it's not a local variable, disable the currentMemberOffset for this declaration
    currentMemberOffset = 0;
    currentLocalOffset = 0;
    currentParameterOffset = 12;

    node->visit_children(this);

    if(node->methodbody->returnstatement != NULL)
    {
        //test if both the return type and return statement are object
        if(node->type->basetype == bt_object && node->methodbody->returnstatement->expression->basetype == bt_object)
        {
            //check if they're both of the same object class type
            if(node->type->objectClassName != node->methodbody->returnstatement->expression->objectClassName){
                std::cout << node->type->objectClassName << std::endl;
                std::cout << node->methodbody->returnstatement->expression->objectClassName << std::endl;
                printf("149 return_type_mismatch");
                typeError(return_type_mismatch);
            }
        }
        //else at least one statement is not an object
        else
        {
            //check if they're both of the same type
            if(node->type->basetype != node->methodbody->returnstatement->expression->basetype){
                printf("158 return_type_mismatch");
                typeError(return_type_mismatch);
            }
        }
    }
    else
    {
        if(node->type->basetype != bt_none){
            printf("166 return_type_mismatch");
            typeError(return_type_mismatch);
        }
    }
    
    MethodInfo mInfo;

    std::string methodName = node->identifier->name;

    mInfo.returnType.baseType = node->type->basetype;
    mInfo.returnType.objectClassName = node->type->objectClassName;
    
    //visitMethodBodyNode(node->methodbody);
    mInfo.variables = currentVariableTable;
    mInfo.localsSize = -currentLocalOffset;

    mInfo.parameters = new std::list<CompoundType>();
    std::list<ParameterNode*>* ParamNodeList = node->parameter_list;
    for (auto it = ParamNodeList->begin(); it!=ParamNodeList->end(); it++)
    {
        CompoundType cType;
        cType.baseType = (*it)->type->basetype;
        if( cType.baseType == bt_object )
            cType.objectClassName = (*it)->type->objectClassName;

        mInfo.parameters->push_back(cType);
    }

    currentMethodTable->insert( std::pair<std::string, MethodInfo>(methodName,mInfo) );
}


void TypeCheck::visitMethodBodyNode(MethodBodyNode* node) {

    node->visit_children(this);
}


void TypeCheck::visitParameterNode(ParameterNode* node) {

    node->visit_children(this);

    BaseType declarType = node->type->basetype;
    std::string declarClassName = "";
    if(declarType == bt_object)
        declarClassName = node->type->objectClassName;

    VariableInfo vInfo;

    std::string variableName = node->identifier->name;

    vInfo.type.baseType = declarType;
    vInfo.type.objectClassName = declarClassName;

    vInfo.size = 4;
    vInfo.offset = currentParameterOffset;
    currentParameterOffset = currentParameterOffset + vInfo.size;

    currentVariableTable->insert( std::pair<std::string, VariableInfo>(variableName,vInfo) );
}

void TypeCheck::visitDeclarationNode(DeclarationNode* node) {

    node->visit_children(this);

    BaseType declarType = node->type->basetype;
    std::string declarClassName = "";
    if(declarType == bt_object)
        declarClassName = node->type->objectClassName;

    if( declarType == bt_object && classTable->find(declarClassName) == classTable->end() )
        typeError(undefined_class);

    std::list<IdentifierNode*>* IdenNodeList = node->identifier_list;
    for (auto it = IdenNodeList->begin(); it!=IdenNodeList->end(); it++)
    {
        VariableInfo vInfo;
        IdentifierNode* iNode = *it;

        std::string variableName = iNode->name;

        vInfo.type.baseType = declarType;
        vInfo.type.objectClassName = declarClassName;

        vInfo.size = 4;
        currentLocalOffset = currentLocalOffset - vInfo.size;
        vInfo.offset = currentLocalOffset + currentMemberOffset;

        currentVariableTable->insert( std::pair<std::string, VariableInfo>(variableName,vInfo) );
    }
}

void TypeCheck::visitReturnStatementNode(ReturnStatementNode* node) {
    
    node->visit_children(this);

    node->basetype = node->expression->basetype;
    if(node->basetype==bt_object){
        node->objectClassName = node->expression->objectClassName;
    }
}


void TypeCheck::visitAssignmentNode(AssignmentNode* node) {

    node->visit_children(this);


    std::string iden1 = node->identifier_1->name;
    std::string iden2;

    BaseType iden1type = bt_none;
    std::string iden1className = "";

    BaseType leftType = bt_none;
    std::string leftClassName = "";

    if(node->identifier_2 == NULL)
    {
        while(true)
        {
            //first, check if iden1 is in the currentVariableTable
            std::map<std::string, VariableInfo>::iterator variableTableSearch = currentVariableTable->find(iden1);
            if( variableTableSearch != currentVariableTable->end() )
            {
                leftType = variableTableSearch->second.type.baseType;
                if ( leftType == bt_object )
                    leftClassName = variableTableSearch->second.type.objectClassName;
                break;
            }

            //else check if iden1 is still a member of the current class
            std::map<std::string, ClassInfo>::iterator currentClass = classTable->find(currentClassName);
            if( currentClass != classTable->end() && currentClass->second.members != NULL )
            {
                std::map<std::string, VariableInfo>::iterator memberTableSearch = currentClass->second.members->find(iden1);
                if( memberTableSearch != currentClass->second.members->end() )
                {
                    leftType = memberTableSearch->second.type.baseType;
                    if ( leftType == bt_object )
                        leftClassName = memberTableSearch->second.type.objectClassName;
                    break;
                }
            }
         
            //else check if iden1 is still a member of the superclass of the current class
            if( currentClass != classTable->end() )
            {
                std::string currentSuperClassName = currentClass->second.superClassName;
                std::map<std::string, ClassInfo>::iterator currentSuperClass = classTable->find(currentSuperClassName);

                if( currentSuperClass != classTable->end() && currentSuperClass->second.members != NULL )
                {
                    std::map<std::string, VariableInfo>::iterator superMemberTableSearch = currentSuperClass->second.members->find(iden1);
                    if( superMemberTableSearch != currentSuperClass->second.members->end() )
                    {
                        leftType = superMemberTableSearch->second.type.baseType;
                        if ( leftType == bt_object )
                            leftClassName = superMemberTableSearch->second.type.objectClassName;
                        break;
                    }
                }
            }
            printf("344: undefined_variable");
            typeError(undefined_variable);
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
                iden1type = variableTableSearch->second.type.baseType;
                if ( iden1type == bt_object )
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
                    iden1type = memberTableSearch->second.type.baseType;
                    if ( iden1type == bt_object )
                        iden1className = memberTableSearch->second.type.objectClassName;
                    break;
                }
            }
         
            //else check if iden1 is still a member of the superclass of the current class
            if( currentClass != classTable->end() )
            {
                std::string currentSuperClassName = currentClass->second.superClassName;
                std::map<std::string, ClassInfo>::iterator currentSuperClass = classTable->find(currentSuperClassName);

                if( currentSuperClass != classTable->end() && currentSuperClass->second.members != NULL )
                {
                    std::map<std::string, VariableInfo>::iterator superMemberTableSearch = currentSuperClass->second.members->find(iden1);
                    if( superMemberTableSearch != currentSuperClass->second.members->end() )
                    {
                        iden1type = superMemberTableSearch->second.type.baseType;
                        if ( iden1type == bt_object )
                            iden1className = superMemberTableSearch->second.type.objectClassName;
                        break;
                    }
                }
            }
            printf("344: undefined_variable");
            typeError(undefined_variable);
        }

        //check if the class type of identifier_1 is in the classTable
        std::map<std::string, ClassInfo>::iterator iden1Class = classTable->find(iden1className);
        if( (iden1type != bt_object) || (iden1Class == classTable->end()) )
            typeError(not_object);

        //make sure the iden2 is a member in the class of iden1, else return error
        std::map<std::string, VariableInfo>::iterator iden1MemberTableSearch = iden1Class->second.members->find(iden2);
        if( iden1MemberTableSearch != iden1Class->second.members->end() )
        {
            leftType = iden1MemberTableSearch->second.type.baseType;
            if ( leftType == bt_object )
                leftClassName = iden1MemberTableSearch->second.type.objectClassName;
        }
        else
        {
            //check if the superclass of iden1 exists
            std::string iden1superClassName = iden1Class->second.superClassName;
            std::map<std::string, ClassInfo>::iterator iden1superClass = classTable->find(iden1superClassName);
            if( iden1superClass != classTable->end() ) {
                //check if iden2 is a member in the superclass of iden1
                std::map<std::string, VariableInfo>::iterator iden1SuperMemberTableSearch = iden1superClass->second.members->find(iden2);
                if( iden1SuperMemberTableSearch != iden1superClass->second.members->end() )
                {
                    leftType = iden1SuperMemberTableSearch->second.type.baseType;
                    if ( leftType == bt_object )
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
    
    //now compare both sides of the assignment
    if( leftType != node->expression->basetype )
        typeError(assignment_type_mismatch);
    if( leftType == bt_object )
        if( leftClassName != node->expression->objectClassName )
            typeError(assignment_type_mismatch);
}


void TypeCheck::visitCallNode(CallNode* node) {
  
  node->visit_children(this);

}

void TypeCheck::visitIfElseNode(IfElseNode* node) {

    //Make sure to visit children
    node->visit_children(this);

    //Check if the expression is a Boolean
    if (node->expression->basetype != bt_boolean)
        typeError(if_predicate_type_mismatch);
}

void TypeCheck::visitWhileNode(WhileNode* node) {

    //Make sure to visit children
    node->visit_children(this);

    //check if the expression is a Boolean
    if (node->expression->basetype != bt_boolean)
       typeError(while_predicate_type_mismatch);
}

void TypeCheck::visitDoWhileNode(DoWhileNode* node) {

    //Make sure to visit children
    node->visit_children(this);

    //check if the expression is a Boolean
    if (node->expression->basetype != bt_boolean)
        typeError(do_while_predicate_type_mismatch);
}

void TypeCheck::visitPrintNode(PrintNode* node) {
    
    node->visit_children(this);
}

void TypeCheck::visitPlusNode(PlusNode* node) {

    //printf("PLUS NODE\n");
    //Make sure to visit children
    node->visit_children(this);

    if( node->expression_1->basetype != bt_integer )
        typeError(expression_type_mismatch);
    if( node->expression_2->basetype != bt_integer )
        typeError(expression_type_mismatch);
    node->basetype = bt_integer;
}

void TypeCheck::visitMinusNode(MinusNode* node) {

    //Make sure to visit children
    //printf("MINUS NODE\n");
    node->visit_children(this);

    if( node->expression_1->basetype != bt_integer )
        typeError(expression_type_mismatch);
    if( node->expression_2->basetype != bt_integer )
        typeError(expression_type_mismatch);
    node->basetype = bt_integer;
}

void TypeCheck::visitTimesNode(TimesNode* node) {

    //Make sure to visit children
    //printf("TIMES NODE\n");
    node->visit_children(this);

    if( node->expression_1->basetype != bt_integer )
        typeError(expression_type_mismatch);
    if( node->expression_2->basetype != bt_integer )
        typeError(expression_type_mismatch);
    node->basetype = bt_integer;
}

void TypeCheck::visitDivideNode(DivideNode* node) {

    //Make sure to visit children
    //printf("DIVIDE NODE\n");
    node->visit_children(this);

    if( node->expression_1->basetype != bt_integer )
        typeError(expression_type_mismatch);
    if( node->expression_2->basetype != bt_integer )
        typeError(expression_type_mismatch);
    node->basetype = bt_integer;
}

void TypeCheck::visitGreaterNode(GreaterNode* node) {

    //Make sure to visit children
    //printf("GREATER NODE\n");
    node->visit_children(this);

    if( node->expression_1->basetype != bt_integer )
        typeError(expression_type_mismatch);
    if( node->expression_2->basetype != bt_integer )
        typeError(expression_type_mismatch);
    node->basetype = bt_boolean;
}

void TypeCheck::visitGreaterEqualNode(GreaterEqualNode* node) {

    //Make sure to visit children
    //printf("GREATEREQ NODE\n");
    node->visit_children(this);

    if( node->expression_1->basetype != bt_integer )
        typeError(expression_type_mismatch);
    if( node->expression_2->basetype != bt_integer )
        typeError(expression_type_mismatch);
    node->basetype = bt_boolean;
}

void TypeCheck::visitEqualNode(EqualNode* node) {

    //Make sure to visit children
    //printf("EQUAL NODE\n");
    node->visit_children(this);

    if( !(node->expression_1->basetype == bt_integer || node->expression_1->basetype == bt_boolean) )
        typeError(expression_type_mismatch);
    if( !(node->expression_2->basetype == bt_integer || node->expression_2->basetype == bt_boolean) )
        typeError(expression_type_mismatch);
    if( node->expression_1->basetype != node->expression_1->basetype )
        typeError(expression_type_mismatch);
    node->basetype = bt_boolean;
}

void TypeCheck::visitAndNode(AndNode* node) {

    //Make sure to visit children
    //printf("AND NODE\n");
    node->visit_children(this);

    if( node->expression_1->basetype != bt_boolean )
        typeError(expression_type_mismatch);
    if( node->expression_2->basetype != bt_boolean )
        typeError(expression_type_mismatch);
    node->basetype = bt_boolean;
}

void TypeCheck::visitOrNode(OrNode* node) {

    //Make sure to visit children
    //printf("OR NODE\n");
    node->visit_children(this);

    if( node->expression_1->basetype != bt_boolean )
        typeError(expression_type_mismatch);
    if( node->expression_2->basetype != bt_boolean )
        typeError(expression_type_mismatch);
    node->basetype = bt_boolean;
}

void TypeCheck::visitNotNode(NotNode* node) {
    
    //Make sure to visit children
    //printf("NOT NODE\n");
    node->visit_children(this);

    if( node->expression->basetype != bt_boolean )
        typeError(expression_type_mismatch);
    node->basetype = bt_boolean;
}

void TypeCheck::visitNegationNode(NegationNode* node) {

    //Make sure to visit children
    //printf("NEGATION NODE\n");
    node->visit_children(this);

    if( node->expression->basetype != bt_integer )
        typeError(expression_type_mismatch);
    node->basetype = bt_integer;
}

void TypeCheck::visitMethodCallNode(MethodCallNode* node) {

    //Make sure to visit children
    //printf("METHOD CALL NODE\n");
    node->visit_children(this);

    std::string iden1 = node->identifier_1->name;
    std::string iden2;

    BaseType iden1type = bt_none;
    std::string iden1className = "";

    MethodInfo cMethod;

    //the top part:
    //if there's only one identifier, check if this identifier is in the current methodtable
    //else
    //  check if the first identifier if in the currentVariableTable of the method
    //      then check if it's in the variableTable of the currentClass
    //      then check if it's in the variableTable of the superclass of the currentClass
    //      else return error
    //  if identifier_1 is not initialized as a class in the corresponding variable table return error
    //  if identifier_1 lacks methods return error
    if(node->identifier_2 == NULL)
    {
        while(true)
        {
            //first, check if iden1 is in the currentVariableTable
            auto MethodTableSearch = currentMethodTable->find(iden1);
            if( MethodTableSearch != currentMethodTable->end() )
            {
                cMethod = MethodTableSearch->second;
                node->basetype = MethodTableSearch->second.returnType.baseType;
                node->objectClassName = "";
                if ( node->basetype == bt_object )
                    node->objectClassName = MethodTableSearch->second.returnType.objectClassName;
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
                        cMethod = superMethodTableSearch->second;
                        node->basetype = superMethodTableSearch->second.returnType.baseType;
                        node->objectClassName = "";
                        if ( node->basetype == bt_object )
                            node->objectClassName = superMethodTableSearch->second.returnType.objectClassName;
                        break;
                    }
                }
            }
            printf("656: undefined_method");
            typeError(undefined_method);
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
                iden1type = variableTableSearch->second.type.baseType;
                if ( iden1type == bt_object )
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
                    iden1type = memberTableSearch->second.type.baseType;
                    if ( iden1type == bt_object )
                        iden1className = memberTableSearch->second.type.objectClassName;
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
                        iden1type = superMemberTableSearch->second.type.baseType;
                        if ( iden1type == bt_object )
                            iden1className = superMemberTableSearch->second.type.objectClassName;
                        break;
                    }
                }
            }
            printf("663: undefined_variable");
            typeError(undefined_variable);
        }


        //check if the class type of identifier_1 is in the classTable
        std::map<std::string, ClassInfo>::iterator iden1Class = classTable->find(iden1className);
        if( (iden1type != bt_object) || (iden1Class == classTable->end()) )
            typeError(not_object);

        //make sure the iden1Class has methods, else return error
        //if( iden1Class->second.methods == NULL )
        //    typeError(undefined_method);

        //make sure the iden2 is a method in the class of iden1, else return error
        std::map<std::string, MethodInfo>::iterator iden1MethodTableSearch = iden1Class->second.methods->find(iden2);
        if( iden1MethodTableSearch != iden1Class->second.methods->end() )
        {
            cMethod = iden1MethodTableSearch->second;
            node->basetype = iden1MethodTableSearch->second.returnType.baseType;
            node->objectClassName = "";
            if ( node->basetype == bt_object )
                node->objectClassName = iden1MethodTableSearch->second.returnType.objectClassName;
        }
        else
        {
            //check if iden2 is a method in the superclass of iden1
            std::string iden1superClassName = iden1Class->second.superClassName;
            std::map<std::string, ClassInfo>::iterator iden1superClass = classTable->find(iden1superClassName);
            if( iden1superClass != classTable->end() )
            {
                std::map<std::string, MethodInfo>::iterator iden1SuperMethodTableSearch = iden1superClass->second.methods->find(iden2);
                if( iden1SuperMethodTableSearch != iden1superClass->second.methods->end() )
                {
                    cMethod = iden1SuperMethodTableSearch->second;
                    node->basetype = iden1SuperMethodTableSearch->second.returnType.baseType;
                    node->objectClassName = "";
                    if ( node->basetype == bt_object )
                        node->objectClassName = iden1SuperMethodTableSearch->second.returnType.objectClassName;
                }
                else
                {
                    printf("759: undefined_method");
                    typeError(undefined_method);
                }
            }
            else
            {
                printf("765: undefined_method");
                typeError(undefined_method);
            }
        }
    }

    //get the # of arguments for the given method
    int argCount = 0;

    if(node->expression_list!= NULL){
        std::list<ExpressionNode*>* ExpressionNodeList = node->expression_list;
        for (auto it = ExpressionNodeList->begin(); it!=ExpressionNodeList->end(); it++)
        {
            argCount++;
        }
    }
    if(cMethod.parameters != NULL){
        std::list<CompoundType>* ParamList= cMethod.parameters;
        for (auto it = ParamList->begin(); it!=ParamList->end(); it++)
        {
            argCount--;
        }
    }
    
    if (argCount != 0){
        typeError(argument_number_mismatch);
    }

    if(cMethod.parameters != NULL && node->expression_list!= NULL){

        std::list<ExpressionNode*>* ExpressionNodeList = node->expression_list;
        std::list<CompoundType>* ParamList= cMethod.parameters;

        auto ExpressionNodeIt = ExpressionNodeList->begin();
        auto ParamIt = ParamList->begin();

        while( ExpressionNodeIt != ExpressionNodeList->end() && ParamIt != ParamList->end() )
        {
            if( (*ExpressionNodeIt)->basetype != ParamIt->baseType )
                typeError(argument_type_mismatch);
            if( (*ExpressionNodeIt)->basetype == bt_object )
                if( (*ExpressionNodeIt)->objectClassName != ParamIt->objectClassName )
                typeError(argument_type_mismatch);
            
            ExpressionNodeIt++;
            ParamIt++;
        }
    }
}


void TypeCheck::visitMemberAccessNode(MemberAccessNode* node) {

    //printf("MEM ACCESS NODE\n");
    node->visit_children(this);

    //get the names of the identifier
    std::string iden1 = node->identifier_1->name;
    std::string iden2 = node->identifier_2->name;

    BaseType iden1type = bt_none;
    std::string iden1className = "";

    //check if iden1 is in the currentVariableTable
    while(true)
    {
        std::map<std::string, VariableInfo>::iterator variableTableSearch = currentVariableTable->find(iden1);
        if( variableTableSearch != currentVariableTable->end() )
        {
            iden1type = variableTableSearch->second.type.baseType;
            if ( iden1type == bt_object )
                iden1className = variableTableSearch->second.type.objectClassName;
            break;
        }
        //else check if iden1 is still a member or method of the current class
        std::map<std::string, ClassInfo>::iterator currentClass = classTable->find(currentClassName);
        if( currentClass != classTable->end() && currentClass->second.members != NULL )
        {
            std::map<std::string, VariableInfo>::iterator memberTableSearch = currentClass->second.members->find(iden1);
            if( memberTableSearch != currentClass->second.members->end() )
            {
                iden1type = memberTableSearch->second.type.baseType;
                if ( iden1type == bt_object )
                    iden1className = memberTableSearch->second.type.objectClassName;
                break;
            }
        }
            
        //else check if iden1 is still a member of the superclass of the current class
        if(currentClass != classTable->end())
        {
            std::string currentSuperClassName = currentClass->second.superClassName;
            std::map<std::string, ClassInfo>::iterator currentSuperClass = classTable->find(currentSuperClassName);

            if( currentSuperClass != classTable->end() && currentSuperClass->second.members != NULL )
            {
                std::map<std::string, VariableInfo>::iterator superMemberTableSearch = currentSuperClass->second.members->find(iden1);
                if( superMemberTableSearch != currentSuperClass->second.members->end() )
                {
                    iden1type = superMemberTableSearch->second.type.baseType;
                    if ( iden1type == bt_object )
                        iden1className = superMemberTableSearch->second.type.objectClassName;
                    break;
                }
            }
        }
        printf("815: undefined_variable");
        typeError(undefined_variable);
    }
    
    //check if the class type of identifier_1 is in the classTable
    std::map<std::string, ClassInfo>::iterator iden1Class = classTable->find(iden1className);
    if( (iden1type != bt_object) || (iden1Class == classTable->end()) )
        typeError(not_object);

    //make sure the iden2 is a member in the class of iden1, else return error
    std::map<std::string, VariableInfo>::iterator iden1MemberTableSearch = iden1Class->second.members->find(iden2);
    if( iden1MemberTableSearch != iden1Class->second.members->end() )
    {
        node->basetype = iden1MemberTableSearch->second.type.baseType;
        node->objectClassName = "";
        if ( node->basetype == bt_object )
            node->objectClassName = iden1MemberTableSearch->second.type.objectClassName;
    }
    else
    {
        //check if the superclass of iden1 exists
        std::string iden1superClassName = iden1Class->second.superClassName;
        std::map<std::string, ClassInfo>::iterator iden1superClass = classTable->find(iden1superClassName);
        if( iden1superClass != classTable->end() ) {
            //check if iden2 is a member in the superclass of iden1
            std::map<std::string, VariableInfo>::iterator iden1SuperMemberTableSearch = iden1superClass->second.members->find(iden2);
            if( iden1SuperMemberTableSearch != iden1superClass->second.members->end() )
            {
                node->basetype = iden1SuperMemberTableSearch->second.type.baseType;
                node->objectClassName = "";
                if ( node->basetype == bt_object )
                    node->objectClassName = iden1SuperMemberTableSearch->second.type.objectClassName;
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


void TypeCheck::visitVariableNode(VariableNode* node) {

    //printf("VARIABLE NODE\n");
    node->visit_children(this);

    std::string iden1 = node->identifier->name;

    //check if iden1 is in the currentVariableTable
    std::map<std::string, VariableInfo>::iterator variableTableSearch = currentVariableTable->find(iden1);
    if( variableTableSearch != currentVariableTable->end() )
    {
        node->basetype = variableTableSearch->second.type.baseType;
        node->objectClassName = "";
        if(node->basetype  == bt_object)
            node->objectClassName = variableTableSearch->second.type.objectClassName;
        return;
    }

    //else check if iden1 is still a member or method of the current class
    std::map<std::string, ClassInfo>::iterator currentClass = classTable->find(currentClassName);
    if( currentClass != classTable->end() && currentClass->second.members != NULL )
    {
        std::map<std::string, VariableInfo>::iterator memberTableSearch = currentClass->second.members->find(iden1);
        if( currentClass->second.members->find(iden1) != currentClass->second.members->end() )
        {
            node->basetype = memberTableSearch->second.type.baseType;
            node->objectClassName = "";
            if(node->basetype  == bt_object)
                node->objectClassName = memberTableSearch->second.type.objectClassName;
            return;
        }
    }
        
    //else check if iden1 is still a member of the superclass of the current class
    while (currentClass != classTable->end())
    {
        std::string currentSuperClassName = currentClass->second.superClassName;
        currentClass = classTable->find(currentSuperClassName);

        if( currentClass != classTable->end() && currentClass->second.members != NULL )
        {
            std::map<std::string, VariableInfo>::iterator superMemberTableSearch = currentClass->second.members->find(iden1);
            if( currentClass->second.members->find(iden1) != currentClass->second.members->end() )
            {
                node->basetype = superMemberTableSearch->second.type.baseType;
                node->objectClassName = "";
                if(node->basetype  == bt_object)
                    node->objectClassName = superMemberTableSearch->second.type.objectClassName;
                return;
            }
        }
    }
    printf("913: undefined_variable");
    typeError(undefined_variable);
}


void TypeCheck::visitIntegerLiteralNode(IntegerLiteralNode* node) {
    node->basetype = bt_integer;
}

void TypeCheck::visitBooleanLiteralNode(BooleanLiteralNode* node) {
    node->basetype = bt_boolean;
}

void TypeCheck::visitNewNode(NewNode* node) {

    /*
    std::string className = node->identifier->name;
    std::map<std::string, ClassInfo>::iterator newClass = classTable->find(className);
    if(newClass == classTable->end() )
        typeError(undefined_class);
    node->basetype = bt_object;
    node->objectClassName = className;
    */

    node->visit_children(this);

    std::string className = node->identifier->name;

    std::map<std::string, ClassInfo>::iterator newClass = classTable->find(className);
    if(newClass == classTable->end() )
        typeError(undefined_class);

    node->basetype = bt_object;
    node->objectClassName = className;

    std::map<std::string, MethodInfo>::iterator Constructor = newClass->second.methods->find(className);
    if(Constructor != newClass->second.methods->end() )
    {
        MethodInfo cMethod = Constructor->second;

        //get the # of arguments for the given method
        int argCount = 0;

        if(node->expression_list!= NULL){
            std::list<ExpressionNode*>* ExpressionNodeList = node->expression_list;
            for (auto it = ExpressionNodeList->begin(); it!=ExpressionNodeList->end(); it++)
            {
                argCount++;
            }
        }
        if(cMethod.parameters != NULL){
            std::list<CompoundType>* ParamList= cMethod.parameters;
            for (auto it = ParamList->begin(); it!=ParamList->end(); it++)
            {
                argCount--;
            }
        }
        
        if (argCount != 0){
            typeError(argument_number_mismatch);
        }

        if(cMethod.parameters != NULL && node->expression_list!= NULL){

            std::list<ExpressionNode*>* ExpressionNodeList = node->expression_list;
            std::list<CompoundType>* ParamList= cMethod.parameters;

            auto ExpressionNodeIt = ExpressionNodeList->begin();
            auto ParamIt = ParamList->begin();

            while( ExpressionNodeIt != ExpressionNodeList->end() && ParamIt != ParamList->end() )
            {
                if( (*ExpressionNodeIt)->basetype != ParamIt->baseType )
                    typeError(argument_type_mismatch);
                if( (*ExpressionNodeIt)->basetype == bt_object )
                    if( (*ExpressionNodeIt)->objectClassName != ParamIt->objectClassName )
                    typeError(argument_type_mismatch);
                
                ExpressionNodeIt++;
                ParamIt++;
            }
        }
    }
    else
    {
        if(node->expression_list != NULL)
        {    
            typeError(argument_number_mismatch);
        }
    }
}

void TypeCheck::visitIntegerTypeNode(IntegerTypeNode* node) {
    node->basetype = bt_integer;
}

void TypeCheck::visitBooleanTypeNode(BooleanTypeNode* node) {
    node->basetype = bt_boolean;
}

void TypeCheck::visitObjectTypeNode(ObjectTypeNode* node) {
    node->basetype = bt_object;
    node->objectClassName = node->identifier->name;
}

void TypeCheck::visitNoneNode(NoneNode* node) {
    node->basetype = bt_none;
}

void TypeCheck::visitIdentifierNode(IdentifierNode* node) {}

void TypeCheck::visitIntegerNode(IntegerNode* node) {}


/////////////////////////////////////////////////////////////////
//                                                             //
// The following functions are used to print the Symbol Table. //
// They do not need to be modified at all.                     //
//                                                             //
/////////////////////////////////////////////////////////////////

std::string genIndent(int indent) {
    std::string string = std::string("");
    for (int i = 0; i < indent; i++)
        string += std::string(" ");
    return string;
}

std::string string(CompoundType type) {
    switch (type.baseType) {
        case bt_integer:
            return std::string("Integer");
        case bt_boolean:
            return std::string("Boolean");
        case bt_none:
            return std::string("None");
        case bt_object:
            return std::string("Object(") + type.objectClassName + std::string(")");
        default:
            return std::string("");
    }
}


void print(VariableTable variableTable, int indent) {
    std::cout << genIndent(indent) << "VariableTable {";
    if (variableTable.size() == 0) {
        std::cout << "}";
        return;
    }
    std::cout << std::endl;
    for (VariableTable::iterator it = variableTable.begin(); it != variableTable.end(); it++) {
        std::cout << genIndent(indent + 2) << it->first << " -> {" << string(it->second.type);
        std::cout << ", " << it->second.offset << ", " << it->second.size << "}";
        if (it != --variableTable.end())
            std::cout << ",";
        std::cout << std::endl;
    }
    std::cout << genIndent(indent) << "}";
}

void print(MethodTable methodTable, int indent) {
    std::cout << genIndent(indent) << "MethodTable {";
    if (methodTable.size() == 0) {
        std::cout << "}";
        return;
    }
    std::cout << std::endl;
    for (MethodTable::iterator it = methodTable.begin(); it != methodTable.end(); it++) {
        std::cout << genIndent(indent + 2) << it->first << " -> {" << std::endl;
        std::cout << genIndent(indent + 4) << string(it->second.returnType) << "," << std::endl;
        std::cout << genIndent(indent + 4) << it->second.localsSize << "," << std::endl;
        print(*it->second.variables, indent + 4);
        std::cout <<std::endl;
        std::cout << genIndent(indent + 2) << "}";
        if (it != --methodTable.end())
            std::cout << ",";
        std::cout << std::endl;
    }
    std::cout << genIndent(indent) << "}";
}

void print(ClassTable classTable, int indent) {
    std::cout << genIndent(indent) << "ClassTable {" << std::endl;
    for (ClassTable::iterator it = classTable.begin(); it != classTable.end(); it++) {
        std::cout << genIndent(indent + 2) << it->first << " -> {" << std::endl;
        if (it->second.superClassName != "")
            std::cout << genIndent(indent + 4) << it->second.superClassName << "," << std::endl;
        print(*it->second.members, indent + 4);
        std::cout << "," << std::endl;
        print(*it->second.methods, indent + 4);
        std::cout <<std::endl;
        std::cout << genIndent(indent + 2) << "}";
        if (it != --classTable.end())
            std::cout << ",";
        std::cout << std::endl;
    }
    std::cout << genIndent(indent) << "}" << std::endl;
}

void print(ClassTable classTable) {
    print(classTable, 0);
}
