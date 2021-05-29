%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <iostream>

    #include "ast.hpp"
    
    #define YYDEBUG 1
    #define YYINITDEPTH 10000
    int yylex(void);
    void yyerror(const char *);
    
    extern ASTNode* astRoot;
%}

%error-verbose

/* WRITEME: List all your tokens here.*/

%token T_NUMBER       300
%token T_PLUS         301
%token T_MINUS        302
%token T_MULTIPLY     303
%token T_DIVIDE       304
%token T_GREATER      305
%token T_GREATEREQ    306
%token T_EQUALS       307
%token T_EQUALSIGN    308
%token T_AND          309
%token T_OR           310
%token T_NOT          311
%token U_MINUS        312
%token T_LPARENTH     313
%token T_RPARENTH     314
%token T_PERIOD       315
%token T_COMMA        316
%token T_INTEGER      317
%token T_BOOLEAN      318
%token T_TRUE         319
%token T_FALSE        320
%token T_NEW          321
%token T_DO           322
%token T_IF           323
%token T_ELSE         324
%token T_WHILE        325
%token T_PRINT        326
%token T_LBRACKET     327
%token T_RBRACKET     328
%token T_SEMICOLON    329
%token T_RETURN       330
%token T_NONE         331
%token T_EXTENDS      332
%token T_IDENTIFIER   333
%token T_ARROW        334

/* WRITEME: Specify precedence here */

%left T_OR
%left T_AND
%left T_GREATER T_GREATEREQ T_EQUALS
%left T_MINUS T_PLUS
%left T_MULTIPLY T_DIVIDE
%right T_NOT
%right U_MINUS

/* WRITEME: Specify types for all nonterminals and necessary terminals here */

%type <program_ptr> Start
%type <class_list_ptr> ClasslistA ClasslistB
%type <class_ptr> Class Superclass
%type <identifier_ptr> Iden
%type <declaration_list_ptr> Declarlist Memberlist
%type <method_list_ptr> MethodList
%type <method_ptr> Method
%type <parameter_list_ptr> ParamlistA ParamlistB
%type <type_ptr> Type Returntype
%type <methodbody_ptr> Body
%type <statement_list_ptr> Statement Block
%type <returnstatement_ptr> Return
%type <parameter_ptr> Param
%type <declaration_ptr> DeclarA Member
%type <identifier_list_ptr> DeclarB
%type <expression_ptr> Expression Parenthexp
%type <assignment_ptr> Assignment
%type <call_ptr> Call
%type <methodcall_ptr> Methodcall
%type <ifelse_ptr> Ifstatement If Ifelse
%type <while_ptr> While
%type <dowhile_ptr> Dowhile
%type <print_ptr> Print

%type <plus_ptr> Plus
%type <minus_ptr> Minus
%type <times_ptr> Multiply
%type <divide_ptr> Divide
%type <greater_ptr> Greater
%type <greaterequal_ptr> Greatereq
%type <equal_ptr> Equals
%type <and_ptr> And
%type <or_ptr> Or
%type <not_ptr> Not
%type <negation_ptr> Unaryminus
%type <expression_list_ptr> Arguments ArgumentsB
%type <memberaccess_ptr> MemAccess
%type <variable_ptr> Variable
%type <integerliteral_ptr> IntLiteral
%type <integer_ptr> IntNode
%type <booleanliteral_ptr> BoolLiteral
%type <new_ptr> New

%type <integertype_ptr> T_INTEGER
%type <booleantype_ptr> T_BOOLEAN
%type <objecttype_ptr> Object
%type <none_ptr> T_NONE
%type <statement_ptr> StatementB
%type <base_char_ptr> T_IDENTIFIER
%type <base_int> T_NUMBER

%%

Start       : ClasslistA              { $$ = new ProgramNode($1); astRoot = $$; }
            ;
ClasslistA  : ClasslistB Class        { $$ = $1; $$->push_back($2); }
            | ClasslistB Superclass   { $$ = $1; $$->push_back($2); }
            ;
ClasslistB  : ClasslistB Class        { $$ = $1; $$->push_back($2); }
            | ClasslistB Superclass   { $$ = $1; $$->push_back($2); }
            | %empty                  { $$ = new std::list<ClassNode*>(); }
            ;
Class       : Iden T_LBRACKET Memberlist MethodList T_RBRACKET                { $$ = new ClassNode($1, NULL, $3, $4); }
            ;
Superclass  : Iden T_EXTENDS Iden T_LBRACKET Memberlist MethodList T_RBRACKET { $$ = new ClassNode($1, $3, $5, $6); }
            ;
Memberlist  : Memberlist Member T_SEMICOLON     { $$ = $1; $$->push_back($2); }
            | %empty                            { $$ = new std::list<DeclarationNode*>(); }
            ;
Member      : Type Iden                         { auto shortList = new std::list<IdentifierNode*>(); shortList->push_back($2); $$ = new DeclarationNode($1, shortList); }
            ;
MethodList  : Method MethodList                 { $$ = $2; $$->push_front($1); }
            | %empty                            { $$ = new std::list<MethodNode*>(); }
            ;
Method      : Iden T_LPARENTH ParamlistA T_RPARENTH T_ARROW Returntype T_LBRACKET Body T_RBRACKET { $$ = new MethodNode($1, $3, $6, $8); }
            ;
ParamlistA  : Param ParamlistB                  { $$ = $2; $$->push_front($1); }
            | %empty                            { $$ = new std::list<ParameterNode*>(); }
            ;
ParamlistB  : T_COMMA Param ParamlistB          { $$ = $3; $$->push_front($2); }
            | %empty                            { $$ = new std::list<ParameterNode*>(); }
            ;
Param       : Type Iden                         { $$ = new ParameterNode ($1,$2); }
            ;
Body        : Declarlist Statement Return       { $$ = new MethodBodyNode ($1,$2,$3); }
            | Declarlist Statement              { $$ = new MethodBodyNode ($1,$2,NULL); }
            ;
Declarlist  : Declarlist DeclarA                { $$ = $1; $$->push_back($2); }
            | %empty                            { $$ = new std::list<DeclarationNode*>(); }
            ;
DeclarA     : Type Iden DeclarB T_SEMICOLON     { $3->push_front($2); $$ = new DeclarationNode($1,$3); }
            ;
DeclarB     : T_COMMA Iden DeclarB              { $$ = $3; $$->push_front($2); }
            | %empty                            { $$ = new std::list<IdentifierNode*>(); }
            ;
Statement   : StatementB Statement              { $$ = $2; $$->push_front($1); }
            | %empty                            { $$ = new std::list<StatementNode*>(); }
            ;
StatementB  : Assignment                                                          { $$ = $1; }
            | Call                                                                { $$ = $1; }
            | Ifstatement                                                         { $$ = $1; }
            | While                                                               { $$ = $1; }
            | Dowhile                                                             { $$ = $1; }
            | Print                                                               { $$ = $1; }
            ;
Assignment  : Iden T_EQUALSIGN Expression T_SEMICOLON                             { $$ = new AssignmentNode($1, NULL, $3); }
            | Iden T_PERIOD Iden T_EQUALSIGN Expression T_SEMICOLON               { $$ = new AssignmentNode($1, $3, $5); }
            ;
Call        : Methodcall T_SEMICOLON                                              { $$ = new CallNode($1); }
            ;
Ifstatement : If                                                                  { $$ = $1; }
            | Ifelse                                                              { $$ = $1; }
            ;
If          : T_IF Expression T_LBRACKET Block T_RBRACKET                         { $$ = new IfElseNode($2, $4, NULL); }
            ;
Ifelse      : T_IF Expression T_LBRACKET Block T_RBRACKET T_ELSE T_LBRACKET Block T_RBRACKET  { $$ = new IfElseNode($2, $4, $8); }
            ;
While       : T_WHILE Expression T_LBRACKET Block T_RBRACKET                      { $$ = new WhileNode($2, $4); }
            ;
Dowhile     : T_DO T_LBRACKET Block T_RBRACKET T_WHILE Parenthexp T_SEMICOLON     { $$ = new DoWhileNode($3, $6); }
            ;
Block       : StatementB Statement                                                { $$ = $2; $$->push_front($1); }
            ;
Print       : T_PRINT Expression T_SEMICOLON                                      { $$ = new PrintNode($2); }
            ;
Return      : T_RETURN Expression T_SEMICOLON                                     { $$ = new ReturnStatementNode($2); }
            ;
Expression  : Plus        %prec T_PLUS          { $$ = $1; }
            | Minus       %prec T_MINUS         { $$ = $1; }
            | Multiply    %prec T_MULTIPLY      { $$ = $1; }
            | Divide      %prec T_DIVIDE        { $$ = $1; }
            | Greater     %prec T_GREATER       { $$ = $1; }
            | Greatereq   %prec T_GREATEREQ     { $$ = $1; }
            | Equals      %prec T_EQUALS        { $$ = $1; }
            | And         %prec T_AND           { $$ = $1; }
            | Or          %prec T_OR            { $$ = $1; }
            | Not         %prec T_NOT           { $$ = $1; }
            | Unaryminus  %prec U_MINUS         { $$ = $1; }
            | Variable    { $$ = $1; }
            | MemAccess   { $$ = $1; }
            | Methodcall  { $$ = $1; }
            | Parenthexp  { $$ = $1; }
            | IntLiteral  { $$ = $1; }
            | BoolLiteral { $$ = $1; }
            | New         { $$ = $1; }
            ;
Plus        : Expression T_PLUS Expression      { $$ = new PlusNode($1,$3); }
            ;
Minus       : Expression T_MINUS Expression     { $$ = new MinusNode($1,$3); }
            ;
Multiply    : Expression T_MULTIPLY Expression  { $$ = new TimesNode($1,$3); }
            ;
Divide      : Expression T_DIVIDE Expression    { $$ = new DivideNode($1,$3); }
            ;
Greater     : Expression T_GREATER Expression   { $$ = new GreaterNode($1,$3); }
            ;
Greatereq   : Expression T_GREATEREQ Expression { $$ = new GreaterEqualNode($1,$3); }
            ;
Equals      : Expression T_EQUALS Expression    { $$ = new EqualNode($1,$3); }
            ;
And         : Expression T_AND Expression       { $$ = new AndNode($1,$3); }
            ;
Or          : Expression T_OR Expression        { $$ = new OrNode($1,$3); }
            ;
Not         : T_NOT Expression                  { $$ = new NotNode($2); }
            ;
Unaryminus  : T_MINUS Expression                %prec U_MINUS                     { $$ = new NegationNode($2); }
            ;
Variable    : Iden                              { $$ = new VariableNode($1); }
            ;
MemAccess   : Iden T_PERIOD Iden                { $$ = new MemberAccessNode($1, $3); }
            ;
IntLiteral  : IntNode                           { $$ = new IntegerLiteralNode($1); }
            ;
IntNode     : T_NUMBER                          { $$ = new IntegerNode($1); }
            ;
BoolLiteral : T_TRUE                            { $$ = new BooleanLiteralNode( new IntegerNode(1) ); }
            | T_FALSE                           { $$ = new BooleanLiteralNode( new IntegerNode(0) ); }
            ;
New         : T_NEW Iden                                          { $$ = new NewNode($2, NULL); }
            | T_NEW Iden T_LPARENTH Arguments T_RPARENTH          { $$ = new NewNode($2, $4); }
            ;
Parenthexp  : T_LPARENTH Expression T_RPARENTH                    { $$ = $2; }
            ;
Methodcall  : Iden T_LPARENTH Arguments T_RPARENTH                { $$ = new MethodCallNode($1, NULL, $3); }
            | Iden T_PERIOD Iden T_LPARENTH Arguments T_RPARENTH  { $$ = new MethodCallNode($1, $3, $5); }
            ;
Arguments   : ArgumentsB                        { $$ = $1; }
            | %empty                            { $$ = new std::list<ExpressionNode*>(); }
            ;
ArgumentsB  : ArgumentsB T_COMMA Expression     { $$ = $1; $$->push_back($3); }
            | Expression                        { $$ = new std::list<ExpressionNode*>(); $$->push_back($1); }
            ;
Type        : T_INTEGER       { $$ = new IntegerTypeNode(); }
            | T_BOOLEAN       { $$ = new BooleanTypeNode(); }
            | Object          { $$ = $1; }
            ;
Object      : Iden            { $$ = new ObjectTypeNode($1); }
            ;
Returntype  : T_NONE          { $$ = new NoneNode(); }
            | Type            { $$ = $1; }
            ;
Iden        : T_IDENTIFIER    { $$ = new IdentifierNode($1); }
            ;

%%

extern int yylineno;

void yyerror(const char *s) {
  fprintf(stderr, "%s at line %d\n", s, yylineno);
  exit(0);
}