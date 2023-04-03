%option c++

%{
    #include "parser.tab.hpp"    
%}

WS        [ \t\n]+
NAME      [A-Za-z][A-Za-z0-9_]*
INTEGER   -?[1-9][0-9]*
FLOAT     -?[0-9]+\.[0-9]+
STRING    \"[^"]*\"
COMMENT   \/\/.*\n

%%
{COMMENT} //do nothing
"MATCH"|"match"        {return yy::parser::token_type::MATCH_KEYWORD       ;}
"WHERE"|"where"        {return yy::parser::token_type::WHERE_KEYWORD       ;}
"RETURN"|"return"      {return yy::parser::token_type::RETURN_KEYWORD      ;}
"CREATE"|"create"      {return yy::parser::token_type::CREATE_KEYWORD      ;}
"DELETE"|"delete"      {return yy::parser::token_type::DELETE_KEYWORD      ;}
"SET"|"set"            {return yy::parser::token_type::SET_KEYWORD         ;}
"AND"|"and"            {return yy::parser::token_type::AND_KEYWORD         ;}
"OR"|"or"              {return yy::parser::token_type::OR_KEYWORD          ;}
"NOT"|"not"            {return yy::parser::token_type::NOT_KEYWORD         ;}
">"                    {return yy::parser::token_type::GREATER_CMP         ;}
">="                   {return yy::parser::token_type::GREATER_OR_EQUAL_CMP;}
"<"                    {return yy::parser::token_type::LESS_CMP            ;}
">="                   {return yy::parser::token_type::LESS_OR_EQUAL_CMP   ;}
"=="                   {return yy::parser::token_type::EQUAL_CMP           ;}
"contains"             {return yy::parser::token_type::CONTAINS_OP         ;}
"="                    {return yy::parser::token_type::ASSIGNMENT          ;}
"-"                    {return yy::parser::token_type::DASH                ;}
"--"                   {return yy::parser::token_type::DOUBLE_DASH         ;}
"->"                   {return yy::parser::token_type::RIGHT_ARROW         ;}
"<-"                   {return yy::parser::token_type::LEFT_ARROW          ;}
":"                    {return yy::parser::token_type::COLON               ;}
";"                    {return yy::parser::token_type::SCOLON              ;}
"."                    {return yy::parser::token_type::PERIOD              ;}
","                    {return yy::parser::token_type::COMMA               ;}
"("                    {return yy::parser::token_type::LPAR                ;}
")"                    {return yy::parser::token_type::RPAR                ;}
"["                    {return yy::parser::token_type::LBRACKET            ;}
"]"                    {return yy::parser::token_type::RBRACKET            ;}
"{"                    {return yy::parser::token_type::LBRACE              ;}
"}"                    {return yy::parser::token_type::RBRACE              ;}
"true"|"false"         {return yy::parser::token_type::BOOL_LITERAL        ;}
{INTEGER}              {return yy::parser::token_type::INT_LITERAL         ;}
{FLOAT}                {return yy::parser::token_type::FLOAT_LITERAL       ;}
{STRING}               {return yy::parser::token_type::STRING_LITERAL      ;}
{NAME}                 {return yy::parser::token_type::NAME                ;}
<<EOF>>                {return yy::parser::token_type::END_OF_FILE         ;}
{WS} //do nothing
%%