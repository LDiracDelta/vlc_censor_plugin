/** \file
 *  This C header file was generated by $ANTLR version 3.0.1
 *
 *     -  From the grammar source file : Censor.g
 *     -                            On : 2009-12-03 07:19:11
 *     -                for the parser : CensorParserParser *
 * Editing it, at least manually, is not wise. 
 *
 * C language generator and runtime by Jim Idle, jimi|hereisanat|idle|dotgoeshere|ws.
 *
 * View this file with tabs set to 8 (:set ts=8 in gvim) and indent at 4 (:set sw=4 in gvim)
 *
 * The parser CensorParserhas the callable functions (rules) shown below,
 * which will invoke the code for the associated rule in the source grammar
 * assuming that the input stream is pointing to a token/text stream that could begin
 * this rule.
 * 
 * For instance if you call the first (topmost) rule in a parser grammar, you will
 * get the results of a full parse, but calling a rule half way through the grammar will
 * allow you to pass part of a full token stream to the parser, such as for syntax checking
 * in editors and so on.
 *
 * The parser entry points are called indirectly (by function pointer to function) via
 * a parser context typedef pCensorParser, which is returned from a call to CensorParserNew().
 *
 * The entry points for CensorParser are  as follows:
 *
 *  - dynamic_array_t *      pCensorParser->censorship_file(pCensorParser)
 *  - censor_edit_section_t *      pCensorParser->section(pCensorParser)
 *  - CensorParser_section_title_return      pCensorParser->section_title(pCensorParser)
 *  - censor_edit_node_t *      pCensorParser->censorship_expr(pCensorParser)
 *  - censor_category_rating_t *      pCensorParser->rating_association(pCensorParser)
 *  - edit_t *      pCensorParser->censorship_op(pCensorParser)
 *  - struct timeval *      pCensorParser->time_rule(pCensorParser)
 *  - dynamic_array_t *      pCensorParser->category(pCensorParser)
 *  - float      pCensorParser->rating(pCensorParser)
 *
 * The return type for any particular rule is of course determined by the source
 * grammar file.
 */
#ifndef	_CensorParser_H
#define _CensorParser_H
/* =============================================================================
 * Standard antlr3 C runtime definitions
 */
#include    <antlr3.h>

/* End of standard antlr 3 runtime definitions
 * =============================================================================
 */

     #include "censor.hpp"


#ifdef	WIN32
// Disable: Unreferenced parameter,                - Rules with parameters that are not used
//          constant conditional,                  - ANTLR realizes that a prediction is always true (synpred usually)
//          initialized but unused variable        - tree rewrite vairables declared but not needed
//          Unreferenced local variable            - lexer rulle decalres but does not always use _type
//          potentially unitialized variable used  - retval always returned from a rule 
//
// These are only really displayed at warning level /W4 but that is the code ideal I am aiming at
// and the codegen must generate some of these warnings by necessity, apart from 4100, which is
// usually generated when a parser rule is given a parameter that it does not use. Mostly though
// this is a matter of orthogonality hence I disable that one.
//
#pragma warning( disable : 4100 )
#pragma warning( disable : 4101 )
#pragma warning( disable : 4127 )
#pragma warning( disable : 4189 )
#pragma warning( disable : 4701 )
#endif
typedef struct CensorParser_section_title_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;   
}
    CensorParser_section_title_return;





/** Context tracking structure for CensorParser
 */
typedef struct CensorParser_Ctx_struct
{
    /** Built in ANTLR3 context tracker contains all the generic elements
     *  required for context tracking.
     */
    pANTLR3_PARSER   pParser;

    dynamic_array_t * (*censorship_file)	(struct CensorParser_Ctx_struct * ctx);
    censor_edit_section_t * (*section)	(struct CensorParser_Ctx_struct * ctx);
    CensorParser_section_title_return (*section_title)	(struct CensorParser_Ctx_struct * ctx);
    censor_edit_node_t * (*censorship_expr)	(struct CensorParser_Ctx_struct * ctx);
    censor_category_rating_t * (*rating_association)	(struct CensorParser_Ctx_struct * ctx);
    edit_t * (*censorship_op)	(struct CensorParser_Ctx_struct * ctx);
    struct timeval * (*time_rule)	(struct CensorParser_Ctx_struct * ctx);
    dynamic_array_t * (*category)	(struct CensorParser_Ctx_struct * ctx);
    float (*rating)	(struct CensorParser_Ctx_struct * ctx);    unsigned char * (*getGrammarFileName)();
    void	    (*free)   (struct CensorParser_Ctx_struct * ctx);
        
}
    CensorParser, * pCensorParser;

/* Function protoypes for the parser functions that external translation units
 * may wish to call.
 */
ANTLR3_API pCensorParser CensorParserNew         (pANTLR3_COMMON_TOKEN_STREAM     instream);/** Symbolic definitions of all the tokens that the parser will work with.
 * \{
 *
 * Antlr will define EOF, but we can't use that as it it is too common in
 * in C header files and that would be confusing. There is no way to filter this out at the moment
 * so we just undef it here for now. That isn't the value we get back from C recognizers
 * anyway. We are looking for ANTLR3_TOKEN_EOF.
 */
#ifdef	EOF
#undef	EOF
#endif
#ifdef	Tokens
#undef	Tokens
#endif 
#define SKIP_OP      5
#define MUTE      6
#define NUMBER      8
#define DIM      7
#define WHITESPACE      11
#define FLOAT      9
#define DIGIT      13
#define COMMENT      10
#define ROMAN_CHAR      12
#define EOF      -1
#define NON_OP_WORD      4
#ifdef	EOF
#undef	EOF
#define	EOF	ANTLR3_TOKEN_EOF
#endif

/* End of token definitions for CensorParser
 * =============================================================================
 */
/** \} */

#endif
/* END - Note:Keep extra linefeed to satisfy UNIX systems */
