%{
	#include <iostream>
	#include <cstdlib>
	#include "scanner.h"
	#include "interpreter.h"
	#include "parser.hpp"
	#include "location.hh"

	using namespace std;


	// Original yyterminate() macro returns int. Since we're using Bison 3 variants
	// as tokens, we must redefine it to change type from `int` to `Parser::semantic_type`
	#define yyterminate() Julang::Parser::make_EOF(Julang::location(m_driver.getFileName(), yylineno, m_driver.getLocationCol()));

	// This will track current scanner location.
	// Action is called when length of the token is known.
	#define YY_USER_ACTION m_driver.increaseLocationCol(yyleng);
	
	// !!!WARNING!!!
	// Location API is used, but the location is not initialized, 'cause I'm lazy. When making
	// a token with make_{something} method you can pass detailed token location. Current location
	// is accessible with m_driver.getLocationCol() method. All puzzle elements are there - just
	// pass location value in every action code block below. I'm going to waste more time writing
	// this excuse than putting this boilerplate below...
	//
	// Location class can be found in location.hh and posistion.hh files. It's just a bit too much
	// boilerplate for this small example. Bummer.
%}

%option nodefault
%option noyywrap
%option c++
%option yyclass="Scanner"
%option prefix="Julang_"
%option yylineno

integer_value   [-+]?[0-9]+
identifier      [a-zA-Z\_]([a-zA-Z0-9\_])*
%%

"//".* {
}

DECLARE {
    return Julang::Parser::make_DECLARE(Julang::location(m_driver.getFileName(), yylineno, m_driver.getLocationCol())); 
}


DELETE {
    return Julang::Parser::make_DELETE(Julang::location(m_driver.getFileName(), yylineno, m_driver.getLocationCol())); 
}


IF {
    return Julang::Parser::make_IF(Julang::location(m_driver.getFileName(), yylineno, m_driver.getLocationCol())); 
}


WHILE {
    return Julang::Parser::make_WHILE(Julang::location(m_driver.getFileName(), yylineno, m_driver.getLocationCol())); 
}


"[" {
    return Julang::Parser::make_LSBRACKET(Julang::location(m_driver.getFileName(), yylineno, m_driver.getLocationCol()));
}

"]" {
    return Julang::Parser::make_RSBRACKET(Julang::location(m_driver.getFileName(), yylineno, m_driver.getLocationCol()));
}

"{" {
    return Julang::Parser::make_LCBRACKET(Julang::location(m_driver.getFileName(), yylineno, m_driver.getLocationCol()));
}

"}" {
    return Julang::Parser::make_RCBRACKET(Julang::location(m_driver.getFileName(), yylineno, m_driver.getLocationCol()));
}


\+ {
    return Julang::Parser::make_PLUS(Operator::SUM, Julang::location(m_driver.getFileName(), yylineno, m_driver.getLocationCol()));
}

\- {
    return Julang::Parser::make_MINUS(Operator::SUB, Julang::location(m_driver.getFileName(), yylineno, m_driver.getLocationCol()));
}

= {
    return Julang::Parser::make_ASSIGNMENT(Operator::ASSIGN, Julang::location(m_driver.getFileName(), yylineno, m_driver.getLocationCol()));
}


== {
    return Julang::Parser::make_EQ(Operator::EQ, Julang::location(m_driver.getFileName(), yylineno, m_driver.getLocationCol()));
}

!= {
    return Julang::Parser::make_NE(Operator::NE, Julang::location(m_driver.getFileName(), yylineno, m_driver.getLocationCol()));
}

\> {
    return Julang::Parser::make_GT(Operator::GT, Julang::location(m_driver.getFileName(), yylineno, m_driver.getLocationCol()));
}

">=" {
    return Julang::Parser::make_GE(Operator::GE, Julang::location(m_driver.getFileName(), yylineno, m_driver.getLocationCol()));
}

\< {
    return Julang::Parser::make_LT(Operator::LT, Julang::location(m_driver.getFileName(), yylineno, m_driver.getLocationCol()));
}

"<=" {
    return Julang::Parser::make_LE(Operator::LE, Julang::location(m_driver.getFileName(), yylineno, m_driver.getLocationCol()));
}
         
\( {
    return Julang::Parser::make_LEFTPAR(Julang::location(m_driver.getFileName(), yylineno, m_driver.getLocationCol()));
}
            
\) { 
    return Julang::Parser::make_RIGHTPAR(Julang::location(m_driver.getFileName(), yylineno, m_driver.getLocationCol()));
}
            
";" {
    return Julang::Parser::make_SEMICOLON(Julang::location(m_driver.getFileName(), yylineno, m_driver.getLocationCol()));
}
         
[\n] { 
    m_driver.increaseLocationCol(-m_driver.getLocationCol() + 1);
}

[\t ] {
}

{integer_value} {
    int integer = std::stoi(yytext, 0, 10);
    return Julang::Parser::make_INTEGER(integer, Julang::location(m_driver.getFileName(), yylineno, m_driver.getLocationCol()));
}

{identifier} {
    return Julang::Parser::make_IDENTIFIER(yytext, Julang::location(m_driver.getFileName(), yylineno, m_driver.getLocationCol())); 
}

. {
    cerr << "line " << yylineno << ": illegal character '" << yytext[0] << endl;
}
            
<<EOF>> {
    return yyterminate();
}


%%
