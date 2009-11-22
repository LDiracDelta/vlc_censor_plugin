grammar Censor;

options {
    language = C;
}
@parser::includes {
    #include "censor.hpp"
}

/*------------------------------------------------------------------
 * PARSER RULES
 *------------------------------------------------------------------*/
censorship_file returns [ dynamic_array_t *p_sections ]
    @init { p_sections = new_dynamic_array(8); } :
    (
        s=section
        {
            dynamic_array_append(p_sections,s);
        }
    )+ EOF;

section returns [ censor_edit_section_t *p_section]
    @init { p_section = censor_edit_section_new(); } :
    (
        s=section_title
        {
            p_section->psz_section_title = $s.text->chars;
        }
    )?
    (
        c=censorship_expr
        {
            dynamic_array_append(p_section->p_edits,c);
        }
    )+;

section_title:
    NON_OP_WORD+ ':';

censorship_expr returns [ censor_edit_node_t *p_censor_edit_node]
    @init { p_censor_edit_node = censor_edit_node_new(); } :
    p_edit=censorship_op start_time=time_rule '-' end_time=time_rule
        (ra=rating_association
        {
            dynamic_array_append(p_censor_edit_node->p_category_ratings,ra);
        }
        )*
    {
        p_censor_edit_node->edit = *p_edit;
        p_censor_edit_node->p_start_time = start_time;
        p_censor_edit_node->p_end_time   = end_time;
    };
 
rating_association returns [censor_category_rating_t *p_rating]
    @init { p_rating = malloc(sizeof(*p_rating)); } :
    ( c=category ':' r=rating)
    {
        p_rating->rating = r;
        p_rating->p_category_hierarchy = c;
    };

censorship_op returns [ edit_t *p_edit]
    @init { $p_edit = malloc(sizeof(*$p_edit)); } :
    (
        SKIP_OP {*$p_edit=SKIP_OP;} |
        MUTE    {*$p_edit=MUTE;   } |
        DIM     {*$p_edit=DIM;    }
    )
    { printf("edit \%d\n",*$p_edit); };

time_rule returns [ struct timeval *p_time]
    @init { $p_time = malloc(sizeof(*$p_time)); } :
    hour=NUMBER ':' minute=NUMBER ':' second=(FLOAT|NUMBER)
    {
        printf("hour = \%s\n",$hour.text->chars);
        printf("minute = \%s\n",$minute.text->chars);
        printf("second = \%s\n",$second.text->chars);
        printf("$p_time = \%p\n",$p_time);
        $p_time->tv_sec = 60*60*atoi($hour.text->chars);
        $p_time->tv_sec += 60*atoi($minute.text->chars);
        float f_seconds = atof($second.text->chars);
        $p_time->tv_sec += (int)f_seconds;
        $p_time->tv_usec = (int)((f_seconds-((int)f_seconds))*1000);
    };


category returns [ dynamic_array_t *p_dynamic_array]
    @init { p_dynamic_array = new_dynamic_array(1); } :
    nop=NON_OP_WORD { dynamic_array_append(p_dynamic_array, $nop.text->chars); }
    (
        '/' nop=NON_OP_WORD { dynamic_array_append(p_dynamic_array, $nop.text->chars); }
    )*;

rating returns [ float the_rating]
    : r=(FLOAT|NUMBER)
    {
        the_rating = atof($r.text->chars);
        if ( the_rating < 0.0  ) { the_rating = 0.0;  };
        if ( the_rating > 10.0 ) { the_rating = 10.0; };
    };

/*------------------------------------------------------------------
 * LEXER RULES
 *------------------------------------------------------------------*/
 
COMMENT    : '#' (~'\n')* { $channel = HIDDEN; } ;
WHITESPACE : ( '\t' | ' ' | '\r' | '\n'| '\u000C' )+  { $channel = HIDDEN; } ;

// Couldn't use just plain 'SKIP', because this is already used by the generated
// code
SKIP_OP : ('s'|'S') ('k'|'K') ('i'|'I') ('p'|'P');
MUTE    : ('m'|'M') ('u'|'U') ('t'|'T') ('e'|'E');
DIM     : ('d'|'D') ('i'|'I') ('m'|'M');



NON_OP_WORD : ROMAN_CHAR ( ROMAN_CHAR|'_'| DIGIT)*;

NUMBER              : DIGIT+ ;
FLOAT               : NUMBER '.' NUMBER;
fragment DIGIT      : '0'..'9' ;
fragment ROMAN_CHAR : ( 'a'..'z'|'A'..'Z');

