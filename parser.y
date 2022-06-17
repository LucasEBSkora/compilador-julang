%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.7.4"
%defines
%define api.parser.class { Parser }

%define api.token.constructor
%define api.value.type variant
%define parse.assert
%define api.namespace { Julang }

%code requires
{
    #include <iostream>
    #include <string>
    #include <vector>
    #include <stdint.h>
    #include <sstream>
    #include <memory>
    #include <fstream>

    using namespace std;

    namespace Julang {
        class Scanner;
        class Interpreter;
    }

    #include "statements.h"
    #include "expression.h"
}

%code top
{
    #include <iostream>
    #include <fstream>
    #include "scanner.h"
    #include "parser.hpp"
    #include "interpreter.h"
    #include "location.hh"
    
    
    // yylex() arguments are defined in parser.y
    static Julang::Parser::symbol_type yylex(Julang::Scanner &scanner, Julang::Interpreter &driver) {
        return scanner.get_next_token();
    }
    
    using namespace Julang;
}

%lex-param {Julang::Scanner &scanner}
%lex-param {Julang::Interpreter &driver}

%parse-param {Julang::Scanner &scanner}
%parse-param {Julang::Interpreter &driver}

%locations
%define parse.trace
%define parse.error verbose

%define api.token.prefix {TOKEN_}

%token EOF 0;

%token <std::string> IDENTIFIER;

%token DECLARE "declare";
%token DELETE "delete";
%token IF "if";
%token WHILE "while";

/* Literals */
%token <int> INTEGER;

%token SEMICOLON ";";

/* Operators */
%token <Operator> ASSIGNMENT "=";
%token <Operator> PLUS "+";
%token <Operator> MINUS "-";
%token <Operator> EQ "==";
%token <Operator> NE "!=";
%token <Operator> GT ">";
%token <Operator> GE ">=";
%token <Operator> LT "<";
%token <Operator> LE "<=";

%token LEFTPAR "(";
%token RIGHTPAR ")";
%token LSBRACKET "[";
%token RSBRACKET "]";
%token LCBRACKET "{";
%token RCBRACKET "}";


%left LT GT EQ NE LE GE
%right LEFTPAR LSBRACKET LCBRACKET ASSIGN 
%left PLUS MINUS RIGHTPAR RSBRACKET RCBRACKET

%type <Statement*> statement;
%type <Statement*> if_statement;
%type <Statement*> while_statement;
%type <Statement*> declare_statement;
%type <Statement*> delete_statement;
%type <Statement*> assign_statement;

%type <std::vector<Statement*>*> statements;

%type <Expression*> expression;


%start program

%%

program : %empty {   
            driver.clear();
        }
        | program statement {
                std::cout << $2->toString() << std::endl;
        }
        ;

statement : if_statement 
          | while_statement
          | declare_statement
          | delete_statement
          | assign_statement;
            
if_statement : "if" "(" expression  ")" "{" statements "}" {
                    $$ = new IfStatement($3, $6);
                };

while_statement : "while" "(" expression ")" "{" statements "}" {
                    $$ = new WhileStatement($3, $6);
                };

declare_statement : "declare" IDENTIFIER ";" {
                    $$ = new DeclareScalarStatement($2);
                  }
                  | "declare" IDENTIFIER "=" expression ";" {
                    $$ = new DeclareScalarStatement($2, $4);
                  }
                  | "declare" IDENTIFIER "[" INTEGER "]" ";"{
                    $$ = new DeclareArrayStatement($2, $4);
                  };

delete_statement : "delete" IDENTIFIER ";"{
                    $$ = new DeleteStatement($2);
                  };

assign_statement : IDENTIFIER "=" expression ";" {
                    $$ = new AssignmentStatement($1, $3);
                  }
                 | IDENTIFIER "[" expression "]" "=" expression ";" {
                    $$ = new AssignmentStatement($1, $6, $3);
                  };

statements : statement {
                $$ = new std::vector<Statement*>();
                $$->push_back($1);
            }
           | statement statements {
                $$ = $2;
                $$->insert($$->begin(), $1);
           };

expression : IDENTIFIER {
                $$ = new ScalarReference($1);
            } 
            | IDENTIFIER "[" expression "]" {
                $$ = new ArrayReference($1, $3);
            }
            | INTEGER {
                $$ = new IntegerLiteral($1);
            }
            | expression "+" expression {
                $$ = new BinaryOperation($1, $2, $3);
            }
            | expression "-" expression {
                $$ = new BinaryOperation($1, $2, $3);
            }
            | expression "==" expression {
                $$ = new BinaryOperation($1, $2, $3);
            }
            | expression  "!="  expression {
                $$ = new BinaryOperation($1, $2, $3);
            }
            | expression ">" expression {
                $$ = new BinaryOperation($1, $2, $3);
            }
            | expression ">=" expression {
                $$ = new BinaryOperation($1, $2, $3);
            }
            | expression "<" expression {
                $$ = new BinaryOperation($1, $2, $3);
            }
            | expression "<=" expression {
                $$ = new BinaryOperation($1, $2, $3);
            }
            ;

%%

// Bison expects us to provide implementation - otherwise linker complains
void Julang::Parser::error(const location &loc , const std::string &message) {
        
        // Location should be initialized inside scanner action, but is not in this example.
        // Let's grab location directly from driver class.
	// cout << @$ << " " << "Error: " << message << endl << "Location: " << loc << endl;
	
        cout << "Error: " << message << endl << "Error location: " << loc << endl;
}
