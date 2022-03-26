/*********************************************************************
  Name: Caleb Kirkland             NetID: cok18
  Course: CSE 4714              
  Project Part 2 - Parsing
  Purpose of File: Recursive Descent Parser for subset of TIPs. 
**********************************************************************/
#ifndef PRODUCTIONS_H
#define PRODUCTIONS_H

#include <iostream>

extern set<string> symbolTable;
set<string>::iterator itr; // Iterator I for going over the Symbol Table
int nextToken = 0;  // token returned from yylex
int level = 0;  // used to indent output to approximate parse tree
int declarationCount = 0;

extern "C"
{
	// Instantiate global variables used by flex
	extern char* yytext;       // text of current lexeme
	extern int   yylex();      // the generated lexical analyzer
}

// Production Functions
void program();
void block();
void read(); 
void write(); 
void statement(); 
void assignment();  
void compound_statement();
void If();
void While();
void expression();
void simple_expression();
void term();
void factor();
void indent();

// Functions to check if the first token sets are correct.
bool first_of_read();
bool first_of_write();
bool first_of_program();
bool first_of_block();
bool first_of_statement();
bool first_of_expression();
bool first_of_if();
bool first_of_assignment();
bool first_of_while();
bool first_of_simple_expression();
bool first_of_term();
bool first_of_factor();
bool first_of_compound();

int lex() {
    nextToken = yylex(); // Get next token

    if( nextToken == TOK_EOF ) {
        yytext[0] = 'E'; yytext[1] = 'O'; yytext[2] = 'F'; yytext[3] = 0; 
    } else if (nextToken == TOK_IDENT) { // If its an identifier, add to table
        symbolTable.insert(yytext);
    } 
    return nextToken;
}
inline void indent(){
    for (int i = 0; i<level; i++)
        cout << ("    ");
}
void output(){
    indent();
    cout << "-->found " << yytext << endl;
}

//********************** PRODUCTION FUNCTIONS ******************************
void program() {
    if (!first_of_program()) {
        throw ("3: 'PROGRAM' expected");
    }
    cout << "enter <program>" << endl;
    indent();
    ++level;
    output();
    
    nextToken = yylex();

    output();

    // If the first token isnt an identifier, throw error
    if (nextToken != TOK_IDENT) {
        throw ("2: identifier expected");
    }
    
    nextToken = yylex();


    // If there isnt semi-colon after the ID, throw error
    if (nextToken != TOK_SEMICOLON) {
        throw ("14 : ';' expected");
    }
    output();
    
    nextToken = yylex();

    if (!first_of_block()) {
        // Block didnt start correctly
        throw ("18: error in declaration part OR 17: 'BEGIN' expected");
    }
    
    // As long as the current token isn't "End", block will be called recursively
    while (nextToken != TOK_END && nextToken != TOK_EOF) {
        block();
    }
 
    --level;
    indent();
    cout << "exit <program>" << endl;
    nextToken = yylex();
}

void block() {
    indent();
    cout << "enter <block>" << endl;
    ++level;
    
    // First token must be var
    if (nextToken == TOK_VAR) {
        output();
        nextToken = yylex();
        // Must be an Identifier after
        if (nextToken != TOK_IDENT) {
            throw ("2: identifier expected");
        }
        // Make sure the freshly declared Identifier is not in the symbol Table
        // If it is not, then add it, and then output().
        for (itr = symbolTable.begin(); itr != symbolTable.end(); ++itr) {
            string temp;
            temp = string(*itr);
            if (temp == yytext) {
                throw ("101: identifier declared twice");
            }
	    }
        symbolTable.insert(yytext);
        output();


        nextToken = yylex();

        output();
        // Expecting colon
        if (nextToken != TOK_COLON) {
            throw ("5 : ':' expected");
        }
        nextToken = yylex();

        output();
        // Expecting int or real
        if (nextToken != TOK_INTEGER && nextToken != TOK_REAL){
            throw ("10: error in type");
        }
        nextToken = yylex();

        output();
        // Expecting Semicolon
        if (nextToken != TOK_SEMICOLON) {
            throw ("14: ';' expected");
        }

        nextToken = yylex();


        // Loop as long as not at start or end of block
        while (nextToken != TOK_SEMICOLON && nextToken != TOK_BEGIN) {
            cout << endl;
            // Token must be identifier
            if (nextToken != TOK_IDENT){
                throw ("2: identifier expected");
            }
            // Make sure the freshly declared Identifier is not in the symbol Table
            // If it is not, then add it, and then output().
            for (itr = symbolTable.begin(); itr != symbolTable.end(); ++itr) {
                string temp;
                temp = string(*itr);
                if (temp == yytext) {
                    throw ("101: identifier declared twice");
                }
            }
            symbolTable.insert(yytext);
            output();

            nextToken = yylex();

            output();
            // Expecting colon before the int or real
            if (nextToken != TOK_COLON) {
                throw ("5 : ':' expected");
            }
            nextToken = yylex();

            output();
            // Expecting an int or real
            if (nextToken != TOK_INTEGER && nextToken != TOK_REAL) {
                throw ("10: error in type");
            }
            nextToken = yylex();

            output();
            // Make sure theres a semi
            if (nextToken != TOK_SEMICOLON){
                throw ("14: ';' expected");
            }
            nextToken = yylex();
        }
        cout << endl;
    }
    if (nextToken != TOK_BEGIN)  {
        throw ("18: error in declaration part OR 17: 'BEGIN' expected");
    }
    // Go into compound statement part of the block
    compound_statement(); 

    while (nextToken != TOK_END && nextToken != TOK_EOF) {
        if (nextToken != TOK_SEMICOLON) {
            throw ("14: ';' expected");
        }
        compound_statement();
    }
    yylex(); // Weird thing that makes my code work and breaks everything if I touch it.

    --level;
    indent();
    cout << "exit <block>" << endl;
    
}

// <statement> -> <assignment> | <compound> | <if> | <while> | <read> | <write>
void statement() {
    indent();
    cout << "enter <statement>" << endl;
    ++level;

    if (nextToken == TOK_IDENT) {
        // Want assignement if the token is an identifier
        assignment();
    } else if (nextToken == TOK_BEGIN || nextToken == TOK_THEN) {
        // Want compound statement if the token is BEGIN or THEN
        compound_statement();
    } else if (nextToken == TOK_IF) {
        If();
        // If the current token is END, then we want to lex, but not 
        // If the token after is EOF
        if (nextToken == TOK_END){
            nextToken = yylex();
        }
    } else if (nextToken == TOK_WHILE) {
        While();
    } else if (nextToken == TOK_READ) {
        read();
    } else if (nextToken = TOK_WRITE) {
        write();
    } else {
        if (nextToken != TOK_SEMICOLON) {
            throw ("14: ';' expected");
        }
    }

    --level;
    indent();
    cout << "exit <statement>" << endl;
}
//<assignment> -> TOK_IDENT TOK_ASSIGN <expression>
void assignment() {
    indent();
    cout << "enter <assignment>" << endl;
    ++level;
    output();
    
    // We want all assigened identifiers added to the symbol table
    symbolTable.insert(yytext);

    nextToken = yylex();

    output();

    // Expecting an walrus operator
    if (nextToken != TOK_ASSIGN) {
        throw ("51: ':=' expected");
    }

    nextToken = yylex();

    // Expecting to have the right expression start
    if (!first_of_expression()) {
        throw ("901: illegal type of simple expression");
    }
    // Go into expression part of the assignment
    expression();

    --level;
    indent();
    cout << "exit <assignment>" << endl;
    
}

// <compound> -> TOK_BEGIN <statement> { TOK_SEMICOLON <statement> } TOK_END 
void compound_statement() {
    indent();
    cout  << "enter <compound_statement>" << endl;
    ++level; 
    output();

    nextToken = yylex();

    // If it doesnt qualify as the first of a statement, the same applies for compound
    if (!first_of_statement()) {
        throw ("900: illegal type of statement");
    }
    // Go into statement of compound statemnet
    statement();
    
    // As long as token is semicolon, then we want to go back into statment
    while (nextToken == TOK_SEMICOLON) {
        output();
        nextToken = yylex();

        // Expecting a statement start
        if (!first_of_statement()) {
            throw ("900: illegal type of statement");
        }
        statement();
        // We found the end of the file if the current token is EOF
        if(nextToken == TOK_EOF) {
            indent();
            cout << "-->found END" << endl;
        }

    }
    // Output the find
    if (nextToken != TOK_EOF) {
        output();
    }
    --level;
    indent();
    cout << "exit <compound_statement>" << endl; 
}

//<if> -> TOK_IF <expression> TOK_THEN <statement> [ TOK_ELSE <statement> ]
void If() {
    indent();
    cout << "enter <if statement>" << endl;
    ++level;

    output();
    nextToken = yylex();

    
    // Expecting an expression start
    if (!first_of_expression()) {
        throw ("144: illegal type of expression");
    }
    // Going into expression part of the If-Statement
    expression();
    // Expecting a then to go with the If
    if (nextToken != TOK_THEN) {
        throw ("52: 'THEN' expected");
    }
    output();
    nextToken = yylex();

    if (!first_of_statement()) {
        throw ("900: illegal type of statement");
    }
    // Going into the next part of the If statement after the "then"
    statement();

    //If the token is else, then we want into another statement
    // We are expecting an else here, so we lex again to see the next token
    if (nextToken != TOK_ELSE) {
        nextToken = yylex();

        if (nextToken == TOK_ELSE) {
            output();

            nextToken = yylex();

        
            // Make sure the first part of the else statement is good
            if (!first_of_statement()) {
                throw ("900: illegal type of statement");
            }
            statement();
        }
    } else {
        output();
        // If its not, then just continue. This is weird but its to control
        // The amount of times we lex. I know, not the best design. 
        nextToken = yylex();

        
        // Make sure the first part of the else statement is good
        if (!first_of_statement()) {
            throw ("900: illegal type of statement");
        }
        statement();
    }

    --level;
    indent();
    cout << "exit <if statement>" << endl;
    
}

// <while> ->TOK_WHILE <expression> <statement>
void While() { 
    indent();
    cout << "enter <while statement>" << endl;
    ++level;
    output();

    if (nextToken == TOK_WHILE) {
        nextToken = yylex();

        // Expecting a correct expression start
        if (!first_of_expression()) {
            throw ("144: illegal type of expression");
        }
        // Go into the condition part of the while loop
        expression();
        // Expecting a correct statement start
        if (!first_of_statement()) {
            throw ("900: illegal type of statement");
        }
        // Go into the statemnet of the while loop
        statement();
    }

    nextToken = yylex();
    --level;
    indent();
    cout << "exit <while statement>" << endl;
}

//<read> -> TOK_READ TOK_OPENPAREN TOK_IDENT TOK_CLOSEPAREN
void read(){
    indent();
    cout << "enter <read>" << endl;
    ++level;
    output();
    
    // Make sure the first part of the read statement starts with (
    lex();
    if (nextToken != TOK_OPENPAREN) {
        throw ("9: '(' expected");
    }
    output();
    // Make sure theres an identifier being printed
    lex();
    if(nextToken != TOK_IDENT) {
        throw ("2: identifier expected");
    }
    output();
    // Make sure the read statement ends with )
    lex();
    if(nextToken != TOK_CLOSEPAREN) {
        throw ("4: ')' expected");
    }
    output();

    lex();
    --level;
    indent();
    cout << "exit <read>" << endl;
}

// <write> -> TOK_WRITE TOK_OPENPAREN ( TOK_IDENT | TOK_STRINGLIT ) TOK_CLOSEPAREN
void write() {
    indent();
    cout << "enter <write>" << endl;
    ++level;
    output();

    // Make sure the first part of the read statement starts with (
    lex();
    if (nextToken != TOK_OPENPAREN) {
        throw ("9: '(' expected");
    }
    output();

    // Make sure theres an identifier being printed
    lex();
    if (nextToken != TOK_IDENT && nextToken != TOK_STRINGLIT){
        throw ("134: illegal type of operand(s)");
    }
    output();

    // Make sure the read statement ends with )
    lex();
    if (nextToken != TOK_CLOSEPAREN) {
        throw ("4: ')' expected");
    }
    output();

    lex();
    --level;
    indent();
    cout << "exit <write>" << endl;
}


void expression() {
    indent();
    cout << "enter <expression>" << endl;
    ++level;
    
    // Make sure the expression starts correctly
    if(!first_of_simple_expression()) {
        throw("144: illegal type of expression");
    }
    // Go into the simple expression portion of the expression
    simple_expression();

    // If token is par tof expression grammar, then output 
    if (nextToken == TOK_EQUALTO || nextToken == TOK_LESSTHAN || 
        nextToken == TOK_GREATERTHAN || nextToken == TOK_NOTEQUALTO) {
        output();
        lex();
        // Make sure simple expression is starting correcly
        if(!first_of_simple_expression()) {
            throw("901: illegal type of simple expression");
        }
        simple_expression();
    }

    --level;
    indent();
    cout << "exit <expression>" << endl;
}

// <simple expression> -> <term> { ( TOK_PLUS | TOK_MINUS | TOK_OR ) <term> }
void simple_expression() {
    indent();
    cout << "enter <simple expression>" << endl;
    ++level;

    // Make sure the first part of the term is correct
    if (!first_of_term()) {
        throw "902: illegal type of term";
    }
    // Go into term part of the simple expression
    term();
    // While a part of the simple expression grammar, keep going into term
    while (nextToken != TOK_SEMICOLON && nextToken != TOK_END && nextToken != TOK_EOF &&
           nextToken != TOK_EQUALTO && nextToken != TOK_GREATERTHAN && nextToken != TOK_LESSTHAN &&
           nextToken != TOK_NOTEQUALTO && nextToken != TOK_THEN && nextToken != TOK_ELSE &&
           nextToken != TOK_CLOSEPAREN && nextToken != TOK_BEGIN) {
        
        if (nextToken == TOK_PLUS) {
            output();
            nextToken = yylex();

            if (!first_of_term())
            {
                throw "902: illegal type of term";
            }
            term();
        } else if (nextToken == TOK_MINUS) {
            output();
            nextToken = yylex();

            if (!first_of_term())
            {
                throw "902: illegal type of term";
            }
            term();
        } else if (nextToken = TOK_OR) {
            output();
            nextToken = yylex();

            if (!first_of_term())
            {
                throw "902: illegal type of term";
            }
            term();
        } else {
            //break;
        }
    }
    --level;
    indent();
    cout << "exit <simple expression>" << endl;
}


void term() {
    indent();
    cout << "enter <term>" << endl;

    ++level;
    
    // Make sure the start of factor is correct
    if (!first_of_factor()) {
        throw "903 : illegal type of factor";
    }
    // Go into factor part of the term
    factor();
    // While a part of the term grammar, check if token is multiply, divide, or and, 
    // and keep going into factor
    while (nextToken != TOK_NOT && nextToken != TOK_MINUS && nextToken != TOK_SEMICOLON &&
           nextToken != TOK_EOF && nextToken != TOK_GREATERTHAN && nextToken != TOK_LESSTHAN &&
           nextToken != TOK_EQUALTO && nextToken != TOK_NOTEQUALTO && nextToken != TOK_THEN &&
           nextToken != TOK_ELSE && nextToken != TOK_CLOSEPAREN && nextToken != TOK_BEGIN &&
           nextToken != TOK_PLUS && nextToken != TOK_END) {
        if (nextToken == TOK_MULTIPLY || nextToken == TOK_DIVIDE || nextToken == TOK_AND){
            output();
            nextToken = yylex();
            // Make sure factor part is starting correctly
            if (!first_of_factor()) {
                throw ("903: illegal type of factor");
            }
            factor();
        }
    }
    --level;
    indent();
    cout << "exit <term>" << endl;
}


void factor() {
    indent();
    cout << "enter <factor>" << endl;
    ++level;
    
    // If int literal or float literal, then output and lex
    if (nextToken == TOK_INTLIT || nextToken == TOK_FLOATLIT) {
        output();
        nextToken = yylex();

    } else if (nextToken == TOK_IDENT) {
        
        // Check symbol table for identifier. See if its already declared or declared twice
        for (itr = symbolTable.begin(); itr != symbolTable.end(); ++itr) {
            // Iterator becomes string in the symbol table, and then we compare the current token
            // to it.
            string temp;
            temp = string(*itr);
            if (temp == yytext) {
                ++declarationCount;
            }
        }
        // If we dont find it, its not declared
        if (declarationCount == 0) {
            throw ("104: identifier not declared");
        }
        // Clear the counter
        --declarationCount;
        // If we do find more than an occurence, its declared twice.
        if (declarationCount > 1) {
            throw ("101: identifier declared twice");
        }
        output();

        nextToken = yylex();

    } else if (nextToken == TOK_OPENPAREN) {
        output();
        nextToken = yylex();


        if (!first_of_expression()) {
            throw ("144: illegal type of expression");
        }
        expression();
        output();
        if (nextToken != TOK_CLOSEPAREN) {
            throw ("4: ')' expected");
        }
        nextToken = yylex();

    } else if (nextToken == TOK_NOT){
        output();
        nextToken = yylex();

        if (!first_of_factor()){
            throw ("903: illegal type of factor");
        }

        for (itr = symbolTable.begin(); itr != symbolTable.end(); ++itr) {
            string temp;
            temp = string(*itr);
            if (temp == yytext) {
                ++declarationCount;
            }
        }
        if (declarationCount == 0) {
            throw ("104: identifier not declared");
        }

        --declarationCount;

        if (declarationCount > 1) {
            throw ("101: identifier declared twice");
        }
        // Go into factor after not
        factor();

    } else if (nextToken == TOK_MINUS) {
        output();
        nextToken = yylex();


        if (!first_of_factor()) {
            throw "903: illegal type of factor";
        }

        if (nextToken == TOK_IDENT) {
	        for (itr = symbolTable.begin(); itr != symbolTable.end(); ++itr) {
	            string temp;
	            temp = string(*itr);
	            if (temp == yytext) {
	                ++declarationCount;
	            }
	        }
	        if (declarationCount == 0) {
	            throw ("104: identifier not declared");
            }

	        --declarationCount;

	        if (declarationCount > 1) {
	            throw "101: identifier declared twice";
            }
        }
        // Go into factor after minus
        factor();
    } else {
        // If no match, then its not a correct factor type
        throw ("903: illegal type of factor");
    }

    --level;
    indent();
    cout << "exit <factor>" << endl;
}

//********************************************************************************
bool first_of_program() {
    return nextToken == TOK_PROGRAM;
}
bool first_of_block() {
    return nextToken == TOK_VAR || nextToken == TOK_BEGIN;
}
bool first_of_statement() {
    return nextToken == TOK_IDENT || nextToken == TOK_BEGIN || nextToken == TOK_IF ||
           nextToken == TOK_WHILE || nextToken == TOK_READ || nextToken == TOK_WRITE;
}
bool first_of_assignment() {
    return nextToken == TOK_IDENT;
}
bool first_of_compound() {
    return nextToken == TOK_BEGIN;
}
bool first_of_if() {
    return nextToken == TOK_IF;
}
bool first_of_while() {
    return nextToken == TOK_WHILE;
}
bool first_of_read() {
    return nextToken == TOK_READ;
}
bool first_of_write() {
    return nextToken == TOK_WRITE;
}
bool first_of_expression() {
    return nextToken == TOK_INTLIT || nextToken == TOK_FLOATLIT || nextToken == TOK_IDENT ||
           nextToken == TOK_OPENPAREN || nextToken == TOK_NOT || nextToken == TOK_MINUS;
}
bool first_of_simple_expression() {
    return nextToken == TOK_INTLIT || nextToken == TOK_FLOATLIT || nextToken == TOK_IDENT ||
           nextToken == TOK_OPENPAREN || nextToken == TOK_NOT || nextToken == TOK_MINUS;
}
bool first_of_term() {
    return nextToken == TOK_INTLIT || nextToken == TOK_FLOATLIT || nextToken == TOK_IDENT ||
           nextToken == TOK_OPENPAREN || nextToken == TOK_NOT || nextToken == TOK_MINUS;
}
bool first_of_factor() {
    return nextToken == TOK_INTLIT || nextToken == TOK_FLOATLIT || nextToken == TOK_IDENT ||
           nextToken == TOK_OPENPAREN || nextToken == TOK_NOT || nextToken == TOK_MINUS;
}
#endif