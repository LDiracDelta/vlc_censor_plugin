lexer grammar Censor;
options {
  language=C;

}

T14 : ':' ;
T15 : '-' ;
T16 : '/' ;

// $ANTLR src "Censor.g" 106
COMMENT    : '#' (~'\n')* { $channel = HIDDEN; } ;
// $ANTLR src "Censor.g" 107
WHITESPACE : ( '\t' | ' ' | '\r' | '\n'| '\u000C' )+  { $channel = HIDDEN; } ;

// Couldn't use just plain 'SKIP', because this is already used by the generated
// code
// $ANTLR src "Censor.g" 111
SKIP_OP : ('s'|'S') ('k'|'K') ('i'|'I') ('p'|'P');
// $ANTLR src "Censor.g" 112
MUTE    : ('m'|'M') ('u'|'U') ('t'|'T') ('e'|'E');
// $ANTLR src "Censor.g" 113
DIM     : ('d'|'D') ('i'|'I') ('m'|'M');



// $ANTLR src "Censor.g" 117
NON_OP_WORD : ROMAN_CHAR ( ROMAN_CHAR|'_'| DIGIT)*;

// $ANTLR src "Censor.g" 119
NUMBER              : DIGIT+ ;
// $ANTLR src "Censor.g" 120
FLOAT               : NUMBER '.' NUMBER;
// $ANTLR src "Censor.g" 121
fragment DIGIT      : '0'..'9' ;
// $ANTLR src "Censor.g" 122
fragment ROMAN_CHAR : ( 'a'..'z'|'A'..'Z');

