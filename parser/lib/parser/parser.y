/*
    Parser for my Cypher-based querry language
*/
%require "3.2"
%language "c++"

%skeleton "lalr1.cc"

%param {ParserDriver *driver}

%code requires 
{
#include <string>
#include <iostream>
#include "../ast.hpp"

#define YYDEBUG 1

namespace yy { class ParserDriver; }

}

%code
{
#include "parser_driver.hpp"

namespace yy
{

void parser::error(const std::string& e){
    std::cout << "Syntax error has been foud: " << e << std::endl;
    driver->markBadInput();
}

parser::token_type yylex(parser::semantic_type* yylval,                         
                         ParserDriver* driver);

}
}

%union
{
    std::string               *name;
    std::string               *string;
    INode                    *iNode;
    RequestNode              *requestNode;
    ExpressionNode           *expressionNode;
    MatchExpressionNode      *matchExpressionNode;
    CreateExpressionNode     *createExpressionNode;
    SetExpressionNode        *setExpressionNode;
    ReturnExpressionNode     *returnExpressionNode;
    DeleteExpressionNode     *deleteExpressionNode;
    VariableMatchNode        *variableMatchNode;
    RelationMatchNode        *relationMatchNode;
    ValueNode                *value;
    FilterNode               *filterNode;
    PredicateNode            *predicateNode;
    LogicalExpressionNode    *logicalExpressionNode;
    AttributeListNode        *attributeListNode;
    int                      integer;
    float                    real;
    bool                     boolean;
}

%token
    MATCH_KEYWORD
    WHERE_KEYWORD
    RETURN_KEYWORD
    CREATE_KEYWORD
    DELETE_KEYWORD
    SET_KEYWORD
    AND_KEYWORD
    OR_KEYWORD
    NOT_KEYWORD
    GREATER_CMP
    GREATER_OR_EQUAL_CMP
    LESS_CMP
    LESS_OR_EQUAL_CMP
    EQUAL_CMP
    CONTAINS_OP
    ASSIGNMENT
    DASH
    RIGHT_ARROW
    LEFT_ARROW
    DOUBLE_DASH
    COLON
    SCOLON
    PERIOD
    COMMA
    LPAR
    RPAR
    LBRACKET
    RBRACKET
    LBRACE
    RBRACE
    END_OF_FILE
;

%token <string>     STRING_LITERAL
%token <integer>    INT_LITERAL
%token <real>       FLOAT_LITERAL
%token <boolean>    BOOL_LITERAL
%token <name>       NAME

%nterm <requestNode>              REQUEST
%nterm <requestNode>              REQUEST_B
%nterm <matchExpressionNode>      MATCH_EXPRESSION
%nterm <variableMatchNode>        VARIABLE_MATCH
%nterm <relationMatchNode>        ANY_RELATION_MATCH
%nterm <relationMatchNode>        RELATION_MATCH
%nterm <returnExpressionNode>     RETURN_EXPRESSION
%nterm <value>                    VALUE
%nterm <createExpressionNode>     CREATE_EXPRESSION 
%nterm <setExpressionNode>        SET_EXPRESSION
%nterm <deleteExpressionNode>     DELETE_EXPRESSION
%nterm <filterNode>               FILTER
%nterm <predicateNode>            PREDICATE
%nterm <logicalExpressionNode>    LOGICAL_EXPRESSION
%nterm <attributeListNode>        ATTRIBUTE_LIST

%left OR_KEYWORD
%left AND_KEYWORD
%left NOT_KEYWORD

%start REQUEST

%%
REQUEST: REQUEST_B SCOLON              { driver->insert($1); return 0; }
       | REQUEST_B END_OF_FILE         { driver->insert($1); return 0; }
;

REQUEST_B: MATCH_EXPRESSION            { $$ = new RequestNode($1); }
         | CREATE_EXPRESSION           { $$ = new RequestNode($1); }
         | REQUEST_B MATCH_EXPRESSION  { $$ = $1; $$->addExpr($2); }
         | REQUEST_B SET_EXPRESSION    { $$ = $1; $$->addExpr($2); }
         | REQUEST_B CREATE_EXPRESSION { $$ = $1; $$->addExpr($2); }
         | REQUEST_B DELETE_EXPRESSION { $$ = $1; $$->addExpr($2); }
         | REQUEST_B RETURN_EXPRESSION { $$ = $1; $$->addExpr($2); }
;

MATCH_EXPRESSION: MATCH_KEYWORD VARIABLE_MATCH                                       { $$ = new MatchExpressionNode($2);         }
                | MATCH_KEYWORD VARIABLE_MATCH RELATION_MATCH VARIABLE_MATCH         { $$ = new MatchExpressionNode($2, $4, $3); }
                | MATCH_KEYWORD VARIABLE_MATCH ANY_RELATION_MATCH VARIABLE_MATCH     { $$ = new MatchExpressionNode($2, $4, $3); }
;

VARIABLE_MATCH: LPAR NAME COLON NAME PREDICATE RPAR                    { $$ = new VariableFilterMatchNode($2, $4, $5);  }
              | LPAR NAME COLON NAME LBRACE ATTRIBUTE_LIST RBRACE RPAR { $$ = new VariablePatternMatchNode($2, $4, $6); }
              | LPAR NAME COLON NAME RPAR                              { $$ = new VariableMatchNode($2, $4); }
              | LPAR NAME RPAR                                         { $$ = new VariableMatchNode($2, new std::string("")); }
;

RELATION_MATCH: DASH LBRACKET NAME COLON NAME RBRACKET RIGHT_ARROW { $$ = new RelationMatchNode($3, $5, FORWARD); }
              | LEFT_ARROW LBRACKET NAME COLON NAME RBRACKET DASH  { $$ = new RelationMatchNode($3, $5, REVERSE); }
;

ANY_RELATION_MATCH: DOUBLE_DASH { $$ = new RelationMatchNode(new std::string(""), new std::string(""), ANY); }
;

PREDICATE: WHERE_KEYWORD LOGICAL_EXPRESSION { $$ = new PredicateNode($2); }
;

LOGICAL_EXPRESSION: LOGICAL_EXPRESSION AND_KEYWORD LOGICAL_EXPRESSION { $$ = new AndOperationNode($1, $3); }
                  | LOGICAL_EXPRESSION OR_KEYWORD LOGICAL_EXPRESSION  { $$ = new OrOperationNode($1, $3);  }
                  | NOT_KEYWORD LOGICAL_EXPRESSION                    { $$ = new NotOperationNode($2);     }
                  | FILTER                                            { $$ = new FilterByPassNode($1);     }
;

FILTER: VALUE LESS_CMP VALUE             { $$ = new FilterNode($1, $3, LESS);             }
      | VALUE LESS_OR_EQUAL_CMP VALUE    { $$ = new FilterNode($1, $3, LESS_OR_EQUAL);    }
      | VALUE GREATER_CMP VALUE          { $$ = new FilterNode($1, $3, GREATER);          }
      | VALUE GREATER_OR_EQUAL_CMP VALUE { $$ = new FilterNode($1, $3, GREATER_OR_EQUAL); }
      | VALUE EQUAL_CMP VALUE            { $$ = new FilterNode($1, $3, EQUAL);            }
      | VALUE CONTAINS_OP VALUE          { $$ = new FilterNode($1, $3, CONTAINS);         }
;

SET_EXPRESSION: SET_KEYWORD NAME PERIOD NAME ASSIGNMENT VALUE { $$ = new SetExpressionNode(new VariableValueNode($2, $4), $6); }
;

DELETE_EXPRESSION: DELETE_KEYWORD NAME { $$ = new DeleteExpressionNode($2); }
;

RETURN_EXPRESSION: RETURN_EXPRESSION COMMA VALUE { $$ = $1; $$->addElement($3);                         }
                 | RETURN_KEYWORD VALUE          { $$ = new ReturnExpressionNode(); $$->addElement($2); }

ATTRIBUTE_LIST: NAME COLON VALUE COMMA ATTRIBUTE_LIST { $$ = $5; $$->addAttribute($1, $3);                      }
              | NAME COLON VALUE                      { $$ = new AttributeListNode(); $$->addAttribute($1, $3); }
;

CREATE_EXPRESSION: CREATE_KEYWORD VARIABLE_MATCH { $$ = new CreateExpressionNode($2); }
                 | CREATE_KEYWORD VARIABLE_MATCH RELATION_MATCH VARIABLE_MATCH { $$ = new CreateExpressionNode($2, $4, $3); }
;

VALUE: NAME             { $$ = new VariableValueNode($1, new std::string("")); }
     | BOOL_LITERAL     { $$ = new BoolLiteralNode($1);       }
     | INT_LITERAL      { $$ = new IntLiteralNode($1);        }
     | FLOAT_LITERAL    { $$ = new FloatLiteralNode($1);      }
     | STRING_LITERAL   { $$ = new StringLiteralNode($1);     }
     | NAME PERIOD NAME { $$ = new VariableValueNode($1, $3); }
;
%%

namespace yy {

parser::token_type yylex(parser::semantic_type* yylval,                         
                         ParserDriver* driver)
{
  return driver->yylex(yylval);
}

}
