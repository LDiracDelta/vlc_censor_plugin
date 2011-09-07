/** \file
 *  This C source file was generated by $ANTLR version 3.0.1
 *
 *     -  From the grammar source file : Censor.g
 *     -                            On : 2009-12-03 07:19:11
 *     -                 for the lexer : CensorLexerLexer *
 * Editing it, at least manually, is not wise. 
 *
 * C language generator and runtime by Jim Idle, jimi|hereisanat|idle|dotgoeshere|ws.
 *
 * View this file with tabs set to 8 (:set ts=8 in gvim) and indent at 4 (:set sw=4 in gvim)
 *
*/
/* -----------------------------------------
 * Include the ANTLR3 generated header file.
 */
#include    "CensorLexer.h"
/* ----------------------------------------- */



/* Aids in accessing scopes for grammar programmers
 */
#undef	SCOPE_TYPE
#undef	SCOPE_STACK
#undef	SCOPE_TOP
#define	SCOPE_TYPE(scope)   pCensorLexer_##scope##_SCOPE
#define SCOPE_STACK(scope)  pCensorLexer_##scope##Stack
#define	SCOPE_TOP(scope)    ctx->pCensorLexer_##scope##Top
#define	SCOPE_SIZE(scope)			(ctx->SCOPE_STACK(scope)->size(ctx->SCOPE_STACK(scope)))
#define SCOPE_INSTANCE(scope, i)	(ctx->SCOPE_STACK(scope)->get(ctx->SCOPE_STACK(scope),i))

/* MACROS that hide the C interface implementations from the
 * generated code, which makes it a little more understandable to the human eye.
 * I am very much against using C pre-processor macros for function calls and bits
 * of code as you cannot see what is happening when single stepping in debuggers
 * and so on. The exception (in my book at least) is for generated code, where you are
 * not maintaining it, but may wish to read and understand it. If you single step it, you know that input()
 * hides some indirect calls, but is always refering to the input stream. This is
 * probably more readable than ctx->input->istream->input(snarfle0->blarg) and allows me to rejig
 * the runtime interfaces without changing the generated code too often, without
 * confusing the reader of the generated output, who may not wish to know the gory
 * details of the interface inheritence.
 */
 
/* Macros for accessing things in a lexer
 */
#undef	    LEXER
#undef	    RECOGNIZER		    
#undef	    RULEMEMO		    
#undef	    GETCHARINDEX
#undef	    GETLINE
#undef	    GETCHARPOSITIONINLINE
#undef	    EMIT
#undef	    EMITNEW
#undef	    MATCHC
#undef	    MATCHS
#undef	    MATCHRANGE
#undef	    LTOKEN
#undef	    HASFAILED
#undef	    FAILEDFLAG
#undef	    INPUT
#undef	    STRSTREAM
#undef	    LA
#undef	    HASEXCEPTION
#undef	    EXCEPTION
#undef	    CONSTRUCTEX
#undef	    CONSUME
#undef	    LRECOVER
#undef	    MARK
#undef	    REWIND
#undef	    REWINDLAST
#undef	    BACKTRACKING
#undef		MATCHANY
#undef		MEMOIZE
#undef		HAVEPARSEDRULE
#undef		GETTEXT
#undef		INDEX
#undef		SEEK
#undef		PUSHSTREAM
#undef		POPSTREAM
#undef		SETTEXT
#undef		SETTEXT8

#define	    LEXER					ctx->pLexer
#define	    RECOGNIZER			    LEXER->rec
#define	    RULEMEMO				RECOGNIZER->ruleMemo
#define	    GETCHARINDEX()			LEXER->getCharIndex(LEXER)
#define	    GETLINE()				LEXER->getLine(LEXER)
#define	    GETTEXT()				LEXER->getText(LEXER)
#define	    GETCHARPOSITIONINLINE() LEXER->getCharPositionInLine(LEXER)
#define	    EMIT()					LEXER->type = _type; LEXER->emit(LEXER)
#define	    EMITNEW(t)				LEXER->emitNew(LEXER, t)
#define	    MATCHC(c)				LEXER->matchc(LEXER, c)
#define	    MATCHS(s)				LEXER->matchs(LEXER, s)
#define	    MATCHRANGE(c1,c2)	    LEXER->matchRange(LEXER, c1, c2)
#define	    MATCHANY()				LEXER->matchAny(LEXER)
#define	    LTOKEN  				LEXER->token
#define	    HASFAILED()				(RECOGNIZER->failed == ANTLR3_TRUE)
#define	    BACKTRACKING			RECOGNIZER->backtracking
#define	    FAILEDFLAG				RECOGNIZER->failed
#define	    INPUT					LEXER->input
#define	    STRSTREAM				INPUT
#define		INDEX()					INPUT->istream->index(INPUT->istream)
#define		SEEK(n)					INPUT->istream->seek(INPUT->istream, n)
#define	    EOF_TOKEN				&(LEXER->tokSource->eofToken)
#define	    HASEXCEPTION()			(RECOGNIZER->error == ANTLR3_TRUE)
#define	    EXCEPTION				RECOGNIZER->exception
#define	    CONSTRUCTEX()			RECOGNIZER->exConstruct(RECOGNIZER)
#define	    LRECOVER()				LEXER->recover(LEXER)
#define	    MARK()					INPUT->istream->mark(INPUT->istream)
#define	    REWIND(m)				INPUT->istream->rewind(INPUT->istream, m)
#define	    REWINDLAST()			INPUT->istream->rewindLast(INPUT->istream)
#define		MEMOIZE(ri,si)			RECOGNIZER->memoize(RECOGNIZER, ri, si)
#define		HAVEPARSEDRULE(r)		RECOGNIZER->alreadyParsedRule(RECOGNIZER, r)
#define		PUSHSTREAM(str)			LEXER->pushCharStream(LEXER, str)
#define		POPSTREAM()				LEXER->popCharStream(LEXER)
#define		SETTEXT(str)			LEXER->text = str
#define		USER1					LEXER->user1
#define		USER2					LEXER->user2
#define		USER3					LEXER->user3
#define		CUSTOM					LEXER->custom

/* If we have been told we can rely on the standard 8 bit or 16 bit input
 * stream, then we can define our macros to use the direct pointers
 * in the input object, which is much faster than indirect calls. This
 * is really only significant to lexers wiht a lot of fragment rules (which
 * do not place LA(1) in a temporary at the moment) and even then
 * only if there is a lot of input (order of say 1M or so).
 */
#if	defined(ANTLR3_INLINE_INPUT_ASCII) || defined(ANTLR3_INLINE_INPUT_UTF16)

# ifdef	ANTLR3_INLINE_INPUT_ASCII

/* 8 bit "ASCII" (actually any 8 bit character set) */

#  define	    NEXTCHAR			((pANTLR3_UINT8)(INPUT->nextChar))
#  define	    DATAP				((pANTLR3_UINT8)(INPUT->data))

# else

#  define	    NEXTCHAR			((pANTLR3_UINT16)(INPUT->nextChar)) 
#  define	    DATAP				((pANTLR3_UINT16)(INPUT->data))

# endif

# define	    LA(n) ((NEXTCHAR + n) > (DATAP + INPUT->sizeBuf) ? ANTLR3_CHARSTREAM_EOF : (ANTLR3_UCHAR)(*(NEXTCHAR + n - 1)))
# define	    CONSUME()											\
{																	\
    if	(NEXTCHAR < (DATAP + INPUT->sizeBuf))					\
    {																\
		INPUT->charPositionInLine++;								\
		if  ((ANTLR3_UCHAR)(*NEXTCHAR) == INPUT->newlineChar)		\
		{															\
			INPUT->line++;										\
			INPUT->charPositionInLine	= 0;						\
			INPUT->currentLine		= (void *)(NEXTCHAR + 1);	\
		}															\
		INPUT->nextChar = (void *)(NEXTCHAR + 1);					\
    }																\
}

#else

// Pick up the input character by calling the input stream implementation.
//
#define	    CONSUME()				INPUT->istream->consume(INPUT->istream)
#define	    LA(n)					INPUT->istream->_LA(INPUT->istream, n)

#endif
#define		TOKTEXT(tok, txt)				tok, (pANTLR3_UINT8)txt

/* The 4 tokens defined below may well clash with your own #defines or token types. If so
 * then for the present you must use different names for your defines as these are hard coded
 * in the code generator. It would be better not to use such names internally, and maybe
 * we can change this in a forthcoming release. I deliberately do not #undef these
 * here as this will at least give you a redefined error somewhere if they clash.
 */
#define	    UP	    ANTLR3_TOKEN_UP
#define	    DOWN    ANTLR3_TOKEN_DOWN
#define	    EOR	    ANTLR3_TOKEN_EOR
#define	    INVALID ANTLR3_TOKEN_INVALID


/* =============================================================================
 * Functions to create and destroy scopes. First come the rule scopes, followed
 * by the global declared scopes.
 */



/* ============================================================================= */

/* =============================================================================
 * Start of recognizer
 */


/* Forward declare the locally static matching functions we have generated and any predicate functions.
 */
static ANTLR3_INLINE void	mT14    (pCensorLexer ctx);
static ANTLR3_INLINE void	mT15    (pCensorLexer ctx);
static ANTLR3_INLINE void	mT16    (pCensorLexer ctx);
static ANTLR3_INLINE void	mCOMMENT    (pCensorLexer ctx);
static ANTLR3_INLINE void	mWHITESPACE    (pCensorLexer ctx);
static ANTLR3_INLINE void	mSKIP_OP    (pCensorLexer ctx);
static ANTLR3_INLINE void	mMUTE    (pCensorLexer ctx);
static ANTLR3_INLINE void	mDIM    (pCensorLexer ctx);
static ANTLR3_INLINE void	mNON_OP_WORD    (pCensorLexer ctx);
static ANTLR3_INLINE void	mNUMBER    (pCensorLexer ctx);
static ANTLR3_INLINE void	mFLOAT    (pCensorLexer ctx);
static ANTLR3_INLINE void	mDIGIT    (pCensorLexer ctx);
static ANTLR3_INLINE void	mROMAN_CHAR    (pCensorLexer ctx);
static ANTLR3_INLINE void	mTokens    (pCensorLexer ctx);
static void	CensorLexerFree(pCensorLexer ctx);

/* =========================================================================
 * Lexer matching rules end.
 * =========================================================================
 */



static void
CensorLexerFree  (pCensorLexer ctx)
{
    LEXER->free(LEXER);
    
    ANTLR3_FREE(ctx);
}

/** \brief Name of the gramar file that generated this code
 */
static unsigned char fileName[] = "Censor.g";

/** \brief Return the name of the grammar file that generated this code.
 */
static unsigned char * getGrammarFileName()
{
	return fileName;
}

/** \brief Create a new lexer called CensorLexer
 *
 * \param[in] instream Pointer to an initialized input stream
 *
 * \return 
 *     - Success pCensorLexer initialized for the lex start
 *     - Fail (pCensorLexer)(ANTLR3_ERR_NOMEM)
 */
ANTLR3_API pCensorLexer CensorLexerNew         (pANTLR3_INPUT_STREAM     instream)
{
    pCensorLexer lexCtx; /* Context structure we will build and return   */

    lexCtx = (pCensorLexer) ANTLR3_MALLOC(sizeof(CensorLexer));

    if  (lexCtx == NULL)
    {
        /* Failed to allocate memory for lexer context */
        return  (pCensorLexer)ANTLR3_ERR_NOMEM;
    }

    /* -------------------------------------------------------------------
     * Memory for basic structure is allocated, now to fill in
     * in base ANTLR3 structures. We intialize the function pointers
     * for the standard ANTLR3 lexer function set, but upon return
     * from here, the programmer may set the pointers to provide custom
     * implementations of each function. 
     *
     * We don't use the macros defined in CensorLexer.h here so you can get a sense
     * of what goes where.
     */
    
    /* Create a base lexer, using the supplied input stream
     */
    lexCtx->pLexer	= antlr3LexerNewStream(ANTLR3_SIZE_HINT, instream);

    /* Check that we allocated the memory correctly
     */
    if	(lexCtx->pLexer == (pANTLR3_LEXER)ANTLR3_ERR_NOMEM)
    {
	ANTLR3_FREE(lexCtx);
	return  (pCensorLexer)ANTLR3_ERR_NOMEM;
    }
    /* Install the implementation of our CensorLexer interface
     */
    lexCtx->mT14	= mT14;
    lexCtx->mT15	= mT15;
    lexCtx->mT16	= mT16;
    lexCtx->mCOMMENT	= mCOMMENT;
    lexCtx->mWHITESPACE	= mWHITESPACE;
    lexCtx->mSKIP_OP	= mSKIP_OP;
    lexCtx->mMUTE	= mMUTE;
    lexCtx->mDIM	= mDIM;
    lexCtx->mNON_OP_WORD	= mNON_OP_WORD;
    lexCtx->mNUMBER	= mNUMBER;
    lexCtx->mFLOAT	= mFLOAT;
    lexCtx->mDIGIT	= mDIGIT;
    lexCtx->mROMAN_CHAR	= mROMAN_CHAR;
    lexCtx->mTokens	= mTokens;
    
    /** When the nextToken() call is made to this lexer's pANTLR3_TOKEN_SOURCE
     *  it will call mTokens() in this generated code, and will pass it the ctx
     * pointer of this lexer, not the context of the base lexer, so store that now.
     */
    lexCtx->pLexer->ctx	    = lexCtx;
    
    /** Install the token matching function
     */
    lexCtx->pLexer->mTokens = (void (*) (void *))(mTokens);
    
    lexCtx->getGrammarFileName	= getGrammarFileName;
    lexCtx->free		= CensorLexerFree;
    
	 
    /* Return the newly built lexer to the caller
     */
    return  lexCtx;
}

/* =========================================================================
 * DFA tables for the lexer
 */
/** Static dfa state tables for Cyclic dfa:
 *    1:1: Tokens : ( T14 | T15 | T16 | COMMENT | WHITESPACE | SKIP_OP | MUTE | DIM | NON_OP_WORD | NUMBER | FLOAT );
 */
static const ANTLR3_INT32 dfa5_eot[24] =
    {
	-1, -1, -1, -1, -1, -1, 9, 9, 9, -1, 14, 9, 9, 9, -1, -1, 9, 9, 21, 22, 
	23, -1, -1, -1
    };
static const ANTLR3_INT32 dfa5_eof[24] =
    {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1
    };
static const ANTLR3_INT32 dfa5_min[24] =
    {
	9, -1, -1, -1, -1, -1, 75, 85, 73, -1, 46, 73, 84, 77, -1, -1, 80, 69, 
	48, 48, 48, -1, -1, -1
    };
static const ANTLR3_INT32 dfa5_max[24] =
    {
	122, -1, -1, -1, -1, -1, 107, 117, 105, -1, 57, 105, 116, 109, -1, -1, 
	112, 101, 122, 122, 122, -1, -1, -1
    };
static const ANTLR3_INT32 dfa5_accept[24] =
    {
	-1, 1, 2, 3, 4, 5, -1, -1, -1, 9, -1, -1, -1, -1, 10, 11, -1, -1, -1, -1, 
	-1, 8, 6, 7
    };
static const ANTLR3_INT32 dfa5_special[24] =
    {	
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1
    };

/** Used when there is no transition table entry for a particular state */
#define dfa5_T_empty	    NULL

static const ANTLR3_INT32 dfa5_T0[] =
    {
	12, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 12
    };static const ANTLR3_INT32 dfa5_T1[] =
    {
	9, 9, 9, 9, 9, 9, 9, 9, 9, 9, -1, -1, -1, -1, -1, -1, -1, 9, 9, 9, 9, 9, 
	9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, -1, -1, 
	-1, -1, 9, -1, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 
	9, 9, 9, 9, 9, 9, 9
    };static const ANTLR3_INT32 dfa5_T2[] =
    {
	5, 5, -1, 5, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, 5, -1, -1, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, 2, -1, 
	3, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 1, -1, -1, -1, -1, -1, -1, 9, 
	9, 9, 8, 9, 9, 9, 9, 9, 9, 9, 9, 7, 9, 9, 9, 9, 9, 6, 9, 9, 9, 9, 9, 9, 
	9, -1, -1, -1, -1, -1, -1, 9, 9, 9, 8, 9, 9, 9, 9, 9, 9, 9, 9, 7, 9, 9, 
	9, 9, 9, 6, 9, 9, 9, 9, 9, 9, 9
    };static const ANTLR3_INT32 dfa5_T3[] =
    {
	11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 11
    };static const ANTLR3_INT32 dfa5_T4[] =
    {
	16, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 16
    };static const ANTLR3_INT32 dfa5_T5[] =
    {
	19, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 19
    };static const ANTLR3_INT32 dfa5_T6[] =
    {
	13, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 13
    };static const ANTLR3_INT32 dfa5_T7[] =
    {
	20, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 20
    };static const ANTLR3_INT32 dfa5_T8[] =
    {
	17, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 17
    };static const ANTLR3_INT32 dfa5_T9[] =
    {
	15, -1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10
    };static const ANTLR3_INT32 dfa5_T10[] =
    {
	18, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 18
    };

/* Transition tables are a table of sub tables, with some tables
 * reused for efficiency.
 */
static const ANTLR3_INT32 * const dfa5_transitions[] =
{
    dfa5_T2, dfa5_T_empty, dfa5_T_empty, dfa5_T_empty, dfa5_T_empty, dfa5_T_empty, 
    dfa5_T3, dfa5_T0, dfa5_T6, dfa5_T_empty, dfa5_T9, dfa5_T4, dfa5_T8, 
    dfa5_T10, dfa5_T_empty, dfa5_T_empty, dfa5_T5, dfa5_T7, dfa5_T1, dfa5_T1, 
    dfa5_T1, dfa5_T_empty, dfa5_T_empty, dfa5_T_empty	
};


/* Declare tracking structure for Cyclic DFA 5
 */
static
ANTLR3_CYCLIC_DFA cdfa5
    =	{
	    5,		    /* Decision number of this dfa	    */
	    /* Which decision this represents:   */
	    (const pANTLR3_UCHAR)"1:1: Tokens : ( T14 | T15 | T16 | COMMENT | WHITESPACE | SKIP_OP | MUTE | DIM | NON_OP_WORD | NUMBER | FLOAT );",	
	    (CDFA_SPECIAL_FUNC) antlr3dfaspecialStateTransition,	/* Default special state transition function	*/
	    antlr3dfaspecialTransition,		/* DFA specialTransition is currently just a default function in the runtime */
	    antlr3dfapredict,			/* DFA simulator function is in the runtime */
	    dfa5_eot,	    /* EOT table			    */
	    dfa5_eof,	    /* EOF table			    */
	    dfa5_min,	    /* Minimum tokens for each state    */
	    dfa5_max,	    /* Maximum tokens for each state    */
	    dfa5_accept,	/* Accept table			    */
	    dfa5_special,	/* Special transition states	    */
	    dfa5_transitions	/* Table of transition tables	    */

	};	    
/* End of Cyclic DFA 5
 * ---------------------
 */ 
/* =========================================================================
 * End of DFA tables for the lexer
 */ 

/* =========================================================================
 * Functions to match the lexer grammar defined tokens from the input stream
 */

//   Comes from: 7:5: ( ':' )
/** \brief Lexer rule generated by ANTLR3
 *
 * $ANTLR start T14
 *
 * Looks to match the characters the constitute the token T14
 * from the attached input stream.
 *
 *
 * \remark
 *  - lexer->error == ANTLR3_TRUE if an exception was thrown.
 */
static ANTLR3_INLINE
void mT14(pCensorLexer ctx)
{
	ANTLR3_UINT32	_type;

   
    _type	    = T14;
       
    
    // Censor.g:7:5: ( ':' )
    // Censor.g:7:7: ':'
    {
        MATCHC(':'); 


    }

	LEXER->type = _type;

    // This is where rules clean up and exit
    //
    goto ruleT14Ex; /* Prevent compiler warnings */
    ruleT14Ex: ;

}
// $ANTLR end T14

//   Comes from: 8:5: ( '-' )
/** \brief Lexer rule generated by ANTLR3
 *
 * $ANTLR start T15
 *
 * Looks to match the characters the constitute the token T15
 * from the attached input stream.
 *
 *
 * \remark
 *  - lexer->error == ANTLR3_TRUE if an exception was thrown.
 */
static ANTLR3_INLINE
void mT15(pCensorLexer ctx)
{
	ANTLR3_UINT32	_type;

   
    _type	    = T15;
       
    
    // Censor.g:8:5: ( '-' )
    // Censor.g:8:7: '-'
    {
        MATCHC('-'); 


    }

	LEXER->type = _type;

    // This is where rules clean up and exit
    //
    goto ruleT15Ex; /* Prevent compiler warnings */
    ruleT15Ex: ;

}
// $ANTLR end T15

//   Comes from: 9:5: ( '/' )
/** \brief Lexer rule generated by ANTLR3
 *
 * $ANTLR start T16
 *
 * Looks to match the characters the constitute the token T16
 * from the attached input stream.
 *
 *
 * \remark
 *  - lexer->error == ANTLR3_TRUE if an exception was thrown.
 */
static ANTLR3_INLINE
void mT16(pCensorLexer ctx)
{
	ANTLR3_UINT32	_type;

   
    _type	    = T16;
       
    
    // Censor.g:9:5: ( '/' )
    // Censor.g:9:7: '/'
    {
        MATCHC('/'); 


    }

	LEXER->type = _type;

    // This is where rules clean up and exit
    //
    goto ruleT16Ex; /* Prevent compiler warnings */
    ruleT16Ex: ;

}
// $ANTLR end T16

//   Comes from: 106:12: ( '#' (~ '\\n' )* )
/** \brief Lexer rule generated by ANTLR3
 *
 * $ANTLR start COMMENT
 *
 * Looks to match the characters the constitute the token COMMENT
 * from the attached input stream.
 *
 *
 * \remark
 *  - lexer->error == ANTLR3_TRUE if an exception was thrown.
 */
static ANTLR3_INLINE
void mCOMMENT(pCensorLexer ctx)
{
	ANTLR3_UINT32	_type;

   
    _type	    = COMMENT;
       
    
    // Censor.g:106:12: ( '#' (~ '\\n' )* )
    // Censor.g:106:14: '#' (~ '\\n' )*
    {
        MATCHC('#'); 


        // Censor.g:106:18: (~ '\\n' )*

        for (;;)
        {
            int alt1=2;
            {
               /* dfaLoopbackState(k,edges,eotPredictsAlt,description,stateNumber,semPredState)
                */
                int LA1_0 = LA(1);
                if ( (((LA1_0 >= 0x0000) && (LA1_0 <= '\t')) || ((LA1_0 >= 0x000B) && (LA1_0 <= 0xFFFE))) ) 
                {
                    alt1=1;
                }

            }
            switch (alt1) 
            {
        	case 1:
        	    // Censor.g:106:19: ~ '\\n'
        	    {
        	        if ( ((LA(1) >= 0x0000) && (LA(1) <= '\t')) || ((LA(1) >= 0x000B) && (LA(1) <= 0xFFFE)) )
        	        {
        	            CONSUME();

        	        }
        	        else 
        	        {
        	            CONSTRUCTEX();
        	            EXCEPTION->type         = ANTLR3_MISMATCHED_SET_EXCEPTION;
        	            EXCEPTION->name         = ANTLR3_MISMATCHED_SET_NAME;

        	            LRECOVER();    goto ruleCOMMENTEx;
        	        }


        	    }
        	    break;

        	default:
        	    goto loop1;	/* break out of the loop */
        	    break;
            }
        }
        loop1: ; /* Jump out to here if this rule does not match */

        {
             LEXER->channel = HIDDEN; 
        }

    }

	LEXER->type = _type;

    // This is where rules clean up and exit
    //
    goto ruleCOMMENTEx; /* Prevent compiler warnings */
    ruleCOMMENTEx: ;

}
// $ANTLR end COMMENT

//   Comes from: 107:12: ( ( '\\t' | ' ' | '\\r' | '\\n' | '\\u000C' )+ )
/** \brief Lexer rule generated by ANTLR3
 *
 * $ANTLR start WHITESPACE
 *
 * Looks to match the characters the constitute the token WHITESPACE
 * from the attached input stream.
 *
 *
 * \remark
 *  - lexer->error == ANTLR3_TRUE if an exception was thrown.
 */
static ANTLR3_INLINE
void mWHITESPACE(pCensorLexer ctx)
{
	ANTLR3_UINT32	_type;

   
    _type	    = WHITESPACE;
       
    
    // Censor.g:107:12: ( ( '\\t' | ' ' | '\\r' | '\\n' | '\\u000C' )+ )
    // Censor.g:107:14: ( '\\t' | ' ' | '\\r' | '\\n' | '\\u000C' )+
    {
        // Censor.g:107:14: ( '\\t' | ' ' | '\\r' | '\\n' | '\\u000C' )+
        {
            int cnt2=0;

            for (;;)
            {
                int alt2=2;
        	{
        	   /* dfaLoopbackState(k,edges,eotPredictsAlt,description,stateNumber,semPredState)
        	    */
        	    int LA2_0 = LA(1);
        	    if ( (((LA2_0 >= '\t') && (LA2_0 <= '\n')) || ((LA2_0 >= '\f') && (LA2_0 <= '\r')) || LA2_0 == ' ') ) 
        	    {
        	        alt2=1;
        	    }

        	}
        	switch (alt2) 
        	{
        	    case 1:
        	        // Censor.g:
        	        {
        	            if ( ((LA(1) >= '\t') && (LA(1) <= '\n')) || ((LA(1) >= '\f') && (LA(1) <= '\r')) || LA(1) == ' ' )
        	            {
        	                CONSUME();

        	            }
        	            else 
        	            {
        	                CONSTRUCTEX();
        	                EXCEPTION->type         = ANTLR3_MISMATCHED_SET_EXCEPTION;
        	                EXCEPTION->name         = ANTLR3_MISMATCHED_SET_NAME;

        	                LRECOVER();    goto ruleWHITESPACEEx;
        	            }


        	        }
        	        break;

        	    default:
        	    
        		if ( cnt2 >= 1 )
        		{
        		    goto loop2;
        		}
        		/* mismatchedSetEx()
        		 */
        		CONSTRUCTEX();
        		EXCEPTION->type = ANTLR3_EARLY_EXIT_EXCEPTION;
        		EXCEPTION->name = ANTLR3_EARLY_EXIT_NAME;


        		goto ruleWHITESPACEEx;
        	}
        	cnt2++;
            }
            loop2: ;	/* Jump to here if this rule does not match */
        }
        {
             LEXER->channel = HIDDEN; 
        }

    }

	LEXER->type = _type;

    // This is where rules clean up and exit
    //
    goto ruleWHITESPACEEx; /* Prevent compiler warnings */
    ruleWHITESPACEEx: ;

}
// $ANTLR end WHITESPACE

//   Comes from: 111:9: ( ( 's' | 'S' ) ( 'k' | 'K' ) ( 'i' | 'I' ) ( 'p' | 'P' ) )
/** \brief Lexer rule generated by ANTLR3
 *
 * $ANTLR start SKIP_OP
 *
 * Looks to match the characters the constitute the token SKIP_OP
 * from the attached input stream.
 *
 *
 * \remark
 *  - lexer->error == ANTLR3_TRUE if an exception was thrown.
 */
static ANTLR3_INLINE
void mSKIP_OP(pCensorLexer ctx)
{
	ANTLR3_UINT32	_type;

   
    _type	    = SKIP_OP;
       
    
    // Censor.g:111:9: ( ( 's' | 'S' ) ( 'k' | 'K' ) ( 'i' | 'I' ) ( 'p' | 'P' ) )
    // Censor.g:111:11: ( 's' | 'S' ) ( 'k' | 'K' ) ( 'i' | 'I' ) ( 'p' | 'P' )
    {
        if ( LA(1) == 'S' || LA(1) == 's' )
        {
            CONSUME();

        }
        else 
        {
            CONSTRUCTEX();
            EXCEPTION->type         = ANTLR3_MISMATCHED_SET_EXCEPTION;
            EXCEPTION->name         = ANTLR3_MISMATCHED_SET_NAME;

            LRECOVER();    goto ruleSKIP_OPEx;
        }

        if ( LA(1) == 'K' || LA(1) == 'k' )
        {
            CONSUME();

        }
        else 
        {
            CONSTRUCTEX();
            EXCEPTION->type         = ANTLR3_MISMATCHED_SET_EXCEPTION;
            EXCEPTION->name         = ANTLR3_MISMATCHED_SET_NAME;

            LRECOVER();    goto ruleSKIP_OPEx;
        }

        if ( LA(1) == 'I' || LA(1) == 'i' )
        {
            CONSUME();

        }
        else 
        {
            CONSTRUCTEX();
            EXCEPTION->type         = ANTLR3_MISMATCHED_SET_EXCEPTION;
            EXCEPTION->name         = ANTLR3_MISMATCHED_SET_NAME;

            LRECOVER();    goto ruleSKIP_OPEx;
        }

        if ( LA(1) == 'P' || LA(1) == 'p' )
        {
            CONSUME();

        }
        else 
        {
            CONSTRUCTEX();
            EXCEPTION->type         = ANTLR3_MISMATCHED_SET_EXCEPTION;
            EXCEPTION->name         = ANTLR3_MISMATCHED_SET_NAME;

            LRECOVER();    goto ruleSKIP_OPEx;
        }


    }

	LEXER->type = _type;

    // This is where rules clean up and exit
    //
    goto ruleSKIP_OPEx; /* Prevent compiler warnings */
    ruleSKIP_OPEx: ;

}
// $ANTLR end SKIP_OP

//   Comes from: 112:9: ( ( 'm' | 'M' ) ( 'u' | 'U' ) ( 't' | 'T' ) ( 'e' | 'E' ) )
/** \brief Lexer rule generated by ANTLR3
 *
 * $ANTLR start MUTE
 *
 * Looks to match the characters the constitute the token MUTE
 * from the attached input stream.
 *
 *
 * \remark
 *  - lexer->error == ANTLR3_TRUE if an exception was thrown.
 */
static ANTLR3_INLINE
void mMUTE(pCensorLexer ctx)
{
	ANTLR3_UINT32	_type;

   
    _type	    = MUTE;
       
    
    // Censor.g:112:9: ( ( 'm' | 'M' ) ( 'u' | 'U' ) ( 't' | 'T' ) ( 'e' | 'E' ) )
    // Censor.g:112:11: ( 'm' | 'M' ) ( 'u' | 'U' ) ( 't' | 'T' ) ( 'e' | 'E' )
    {
        if ( LA(1) == 'M' || LA(1) == 'm' )
        {
            CONSUME();

        }
        else 
        {
            CONSTRUCTEX();
            EXCEPTION->type         = ANTLR3_MISMATCHED_SET_EXCEPTION;
            EXCEPTION->name         = ANTLR3_MISMATCHED_SET_NAME;

            LRECOVER();    goto ruleMUTEEx;
        }

        if ( LA(1) == 'U' || LA(1) == 'u' )
        {
            CONSUME();

        }
        else 
        {
            CONSTRUCTEX();
            EXCEPTION->type         = ANTLR3_MISMATCHED_SET_EXCEPTION;
            EXCEPTION->name         = ANTLR3_MISMATCHED_SET_NAME;

            LRECOVER();    goto ruleMUTEEx;
        }

        if ( LA(1) == 'T' || LA(1) == 't' )
        {
            CONSUME();

        }
        else 
        {
            CONSTRUCTEX();
            EXCEPTION->type         = ANTLR3_MISMATCHED_SET_EXCEPTION;
            EXCEPTION->name         = ANTLR3_MISMATCHED_SET_NAME;

            LRECOVER();    goto ruleMUTEEx;
        }

        if ( LA(1) == 'E' || LA(1) == 'e' )
        {
            CONSUME();

        }
        else 
        {
            CONSTRUCTEX();
            EXCEPTION->type         = ANTLR3_MISMATCHED_SET_EXCEPTION;
            EXCEPTION->name         = ANTLR3_MISMATCHED_SET_NAME;

            LRECOVER();    goto ruleMUTEEx;
        }


    }

	LEXER->type = _type;

    // This is where rules clean up and exit
    //
    goto ruleMUTEEx; /* Prevent compiler warnings */
    ruleMUTEEx: ;

}
// $ANTLR end MUTE

//   Comes from: 113:9: ( ( 'd' | 'D' ) ( 'i' | 'I' ) ( 'm' | 'M' ) )
/** \brief Lexer rule generated by ANTLR3
 *
 * $ANTLR start DIM
 *
 * Looks to match the characters the constitute the token DIM
 * from the attached input stream.
 *
 *
 * \remark
 *  - lexer->error == ANTLR3_TRUE if an exception was thrown.
 */
static ANTLR3_INLINE
void mDIM(pCensorLexer ctx)
{
	ANTLR3_UINT32	_type;

   
    _type	    = DIM;
       
    
    // Censor.g:113:9: ( ( 'd' | 'D' ) ( 'i' | 'I' ) ( 'm' | 'M' ) )
    // Censor.g:113:11: ( 'd' | 'D' ) ( 'i' | 'I' ) ( 'm' | 'M' )
    {
        if ( LA(1) == 'D' || LA(1) == 'd' )
        {
            CONSUME();

        }
        else 
        {
            CONSTRUCTEX();
            EXCEPTION->type         = ANTLR3_MISMATCHED_SET_EXCEPTION;
            EXCEPTION->name         = ANTLR3_MISMATCHED_SET_NAME;

            LRECOVER();    goto ruleDIMEx;
        }

        if ( LA(1) == 'I' || LA(1) == 'i' )
        {
            CONSUME();

        }
        else 
        {
            CONSTRUCTEX();
            EXCEPTION->type         = ANTLR3_MISMATCHED_SET_EXCEPTION;
            EXCEPTION->name         = ANTLR3_MISMATCHED_SET_NAME;

            LRECOVER();    goto ruleDIMEx;
        }

        if ( LA(1) == 'M' || LA(1) == 'm' )
        {
            CONSUME();

        }
        else 
        {
            CONSTRUCTEX();
            EXCEPTION->type         = ANTLR3_MISMATCHED_SET_EXCEPTION;
            EXCEPTION->name         = ANTLR3_MISMATCHED_SET_NAME;

            LRECOVER();    goto ruleDIMEx;
        }


    }

	LEXER->type = _type;

    // This is where rules clean up and exit
    //
    goto ruleDIMEx; /* Prevent compiler warnings */
    ruleDIMEx: ;

}
// $ANTLR end DIM

//   Comes from: 117:13: ( ROMAN_CHAR ( ROMAN_CHAR | '_' | DIGIT )* )
/** \brief Lexer rule generated by ANTLR3
 *
 * $ANTLR start NON_OP_WORD
 *
 * Looks to match the characters the constitute the token NON_OP_WORD
 * from the attached input stream.
 *
 *
 * \remark
 *  - lexer->error == ANTLR3_TRUE if an exception was thrown.
 */
static ANTLR3_INLINE
void mNON_OP_WORD(pCensorLexer ctx)
{
	ANTLR3_UINT32	_type;

   
    _type	    = NON_OP_WORD;
       
    
    // Censor.g:117:13: ( ROMAN_CHAR ( ROMAN_CHAR | '_' | DIGIT )* )
    // Censor.g:117:15: ROMAN_CHAR ( ROMAN_CHAR | '_' | DIGIT )*
    {
        /* 117:15: ROMAN_CHAR ( ROMAN_CHAR | '_' | DIGIT )* */
        mROMAN_CHAR(ctx ); 


        // Censor.g:117:26: ( ROMAN_CHAR | '_' | DIGIT )*

        for (;;)
        {
            int alt3=2;
            {
               /* dfaLoopbackState(k,edges,eotPredictsAlt,description,stateNumber,semPredState)
                */
                int LA3_0 = LA(1);
                if ( (((LA3_0 >= '0') && (LA3_0 <= '9')) || ((LA3_0 >= 'A') && (LA3_0 <= 'Z')) || LA3_0 == '_' || ((LA3_0 >= 'a') && (LA3_0 <= 'z'))) ) 
                {
                    alt3=1;
                }

            }
            switch (alt3) 
            {
        	case 1:
        	    // Censor.g:
        	    {
        	        if ( ((LA(1) >= '0') && (LA(1) <= '9')) || ((LA(1) >= 'A') && (LA(1) <= 'Z')) || LA(1) == '_' || ((LA(1) >= 'a') && (LA(1) <= 'z')) )
        	        {
        	            CONSUME();

        	        }
        	        else 
        	        {
        	            CONSTRUCTEX();
        	            EXCEPTION->type         = ANTLR3_MISMATCHED_SET_EXCEPTION;
        	            EXCEPTION->name         = ANTLR3_MISMATCHED_SET_NAME;

        	            LRECOVER();    goto ruleNON_OP_WORDEx;
        	        }


        	    }
        	    break;

        	default:
        	    goto loop3;	/* break out of the loop */
        	    break;
            }
        }
        loop3: ; /* Jump out to here if this rule does not match */


    }

	LEXER->type = _type;

    // This is where rules clean up and exit
    //
    goto ruleNON_OP_WORDEx; /* Prevent compiler warnings */
    ruleNON_OP_WORDEx: ;

}
// $ANTLR end NON_OP_WORD

//   Comes from: 119:21: ( ( DIGIT )+ )
/** \brief Lexer rule generated by ANTLR3
 *
 * $ANTLR start NUMBER
 *
 * Looks to match the characters the constitute the token NUMBER
 * from the attached input stream.
 *
 *
 * \remark
 *  - lexer->error == ANTLR3_TRUE if an exception was thrown.
 */
static ANTLR3_INLINE
void mNUMBER(pCensorLexer ctx)
{
	ANTLR3_UINT32	_type;

   
    _type	    = NUMBER;
       
    
    // Censor.g:119:21: ( ( DIGIT )+ )
    // Censor.g:119:23: ( DIGIT )+
    {
        // Censor.g:119:23: ( DIGIT )+
        {
            int cnt4=0;

            for (;;)
            {
                int alt4=2;
        	{
        	   /* dfaLoopbackState(k,edges,eotPredictsAlt,description,stateNumber,semPredState)
        	    */
        	    int LA4_0 = LA(1);
        	    if ( (((LA4_0 >= '0') && (LA4_0 <= '9'))) ) 
        	    {
        	        alt4=1;
        	    }

        	}
        	switch (alt4) 
        	{
        	    case 1:
        	        // Censor.g:119:23: DIGIT
        	        {
        	            /* 119:23: DIGIT */
        	            mDIGIT(ctx ); 


        	        }
        	        break;

        	    default:
        	    
        		if ( cnt4 >= 1 )
        		{
        		    goto loop4;
        		}
        		/* mismatchedSetEx()
        		 */
        		CONSTRUCTEX();
        		EXCEPTION->type = ANTLR3_EARLY_EXIT_EXCEPTION;
        		EXCEPTION->name = ANTLR3_EARLY_EXIT_NAME;


        		goto ruleNUMBEREx;
        	}
        	cnt4++;
            }
            loop4: ;	/* Jump to here if this rule does not match */
        }

    }

	LEXER->type = _type;

    // This is where rules clean up and exit
    //
    goto ruleNUMBEREx; /* Prevent compiler warnings */
    ruleNUMBEREx: ;

}
// $ANTLR end NUMBER

//   Comes from: 120:21: ( NUMBER '.' NUMBER )
/** \brief Lexer rule generated by ANTLR3
 *
 * $ANTLR start FLOAT
 *
 * Looks to match the characters the constitute the token FLOAT
 * from the attached input stream.
 *
 *
 * \remark
 *  - lexer->error == ANTLR3_TRUE if an exception was thrown.
 */
static ANTLR3_INLINE
void mFLOAT(pCensorLexer ctx)
{
	ANTLR3_UINT32	_type;

   
    _type	    = FLOAT;
       
    
    // Censor.g:120:21: ( NUMBER '.' NUMBER )
    // Censor.g:120:23: NUMBER '.' NUMBER
    {
        /* 120:23: NUMBER '.' NUMBER */
        mNUMBER(ctx ); 

        MATCHC('.'); 

        /* 120:23: NUMBER '.' NUMBER */
        mNUMBER(ctx ); 


    }

	LEXER->type = _type;

    // This is where rules clean up and exit
    //
    goto ruleFLOATEx; /* Prevent compiler warnings */
    ruleFLOATEx: ;

}
// $ANTLR end FLOAT

//   Comes from: 121:21: ( '0' .. '9' )
/** \brief Lexer rule generated by ANTLR3
 *
 * $ANTLR start DIGIT
 *
 * Looks to match the characters the constitute the token DIGIT
 * from the attached input stream.
 *
 *
 * \remark
 *  - lexer->error == ANTLR3_TRUE if an exception was thrown.
 */
static ANTLR3_INLINE
void mDIGIT(pCensorLexer ctx)
{
	ANTLR3_UINT32	_type = NULL;
    _type = _type;

        
    // Censor.g:121:21: ( '0' .. '9' )
    // Censor.g:121:23: '0' .. '9'
    {
        MATCHRANGE('0', '9'); 


    }



    // This is where rules clean up and exit
    //
    goto ruleDIGITEx; /* Prevent compiler warnings */
    ruleDIGITEx: ;

}
// $ANTLR end DIGIT

//   Comes from: 122:21: ( ( 'a' .. 'z' | 'A' .. 'Z' ) )
/** \brief Lexer rule generated by ANTLR3
 *
 * $ANTLR start ROMAN_CHAR
 *
 * Looks to match the characters the constitute the token ROMAN_CHAR
 * from the attached input stream.
 *
 *
 * \remark
 *  - lexer->error == ANTLR3_TRUE if an exception was thrown.
 */
static ANTLR3_INLINE
void mROMAN_CHAR(pCensorLexer ctx)
{
	ANTLR3_UINT32	_type = NULL;
    _type = _type;

        
    // Censor.g:122:21: ( ( 'a' .. 'z' | 'A' .. 'Z' ) )
    // Censor.g:122:23: ( 'a' .. 'z' | 'A' .. 'Z' )
    {
        if ( ((LA(1) >= 'A') && (LA(1) <= 'Z')) || ((LA(1) >= 'a') && (LA(1) <= 'z')) )
        {
            CONSUME();

        }
        else 
        {
            CONSTRUCTEX();
            EXCEPTION->type         = ANTLR3_MISMATCHED_SET_EXCEPTION;
            EXCEPTION->name         = ANTLR3_MISMATCHED_SET_NAME;

            LRECOVER();    goto ruleROMAN_CHAREx;
        }


    }



    // This is where rules clean up and exit
    //
    goto ruleROMAN_CHAREx; /* Prevent compiler warnings */
    ruleROMAN_CHAREx: ;

}
// $ANTLR end ROMAN_CHAR

/** This is the entry point in to the lexer from an object that
 *  wants to generate the next token, such as a pCOMMON_TOKEN_STREAM
 */
static void 
mTokens(pCensorLexer ctx)
{
    {
        //  Censor.g:1:8: ( T14 | T15 | T16 | COMMENT | WHITESPACE | SKIP_OP | MUTE | DIM | NON_OP_WORD | NUMBER | FLOAT )
        
        ANTLR3_UINT32 alt5;

        alt5=11;

        alt5 = cdfa5.predict(ctx, RECOGNIZER, INPUT->istream, &cdfa5);
        switch (alt5) 
        {
    	case 1:
    	    // Censor.g:1:10: T14
    	    {
    	        /* 1:10: T14 */
    	        mT14(ctx ); 


    	    }
    	    break;
    	case 2:
    	    // Censor.g:1:14: T15
    	    {
    	        /* 1:14: T15 */
    	        mT15(ctx ); 


    	    }
    	    break;
    	case 3:
    	    // Censor.g:1:18: T16
    	    {
    	        /* 1:18: T16 */
    	        mT16(ctx ); 


    	    }
    	    break;
    	case 4:
    	    // Censor.g:1:22: COMMENT
    	    {
    	        /* 1:22: COMMENT */
    	        mCOMMENT(ctx ); 


    	    }
    	    break;
    	case 5:
    	    // Censor.g:1:30: WHITESPACE
    	    {
    	        /* 1:30: WHITESPACE */
    	        mWHITESPACE(ctx ); 


    	    }
    	    break;
    	case 6:
    	    // Censor.g:1:41: SKIP_OP
    	    {
    	        /* 1:41: SKIP_OP */
    	        mSKIP_OP(ctx ); 


    	    }
    	    break;
    	case 7:
    	    // Censor.g:1:49: MUTE
    	    {
    	        /* 1:49: MUTE */
    	        mMUTE(ctx ); 


    	    }
    	    break;
    	case 8:
    	    // Censor.g:1:54: DIM
    	    {
    	        /* 1:54: DIM */
    	        mDIM(ctx ); 


    	    }
    	    break;
    	case 9:
    	    // Censor.g:1:58: NON_OP_WORD
    	    {
    	        /* 1:58: NON_OP_WORD */
    	        mNON_OP_WORD(ctx ); 


    	    }
    	    break;
    	case 10:
    	    // Censor.g:1:70: NUMBER
    	    {
    	        /* 1:70: NUMBER */
    	        mNUMBER(ctx ); 


    	    }
    	    break;
    	case 11:
    	    // Censor.g:1:77: FLOAT
    	    {
    	        /* 1:77: FLOAT */
    	        mFLOAT(ctx ); 


    	    }
    	    break;

        }
    }

    
    goto ruleTokensEx; /* Prevent compiler warnings */
ruleTokensEx: ;
}

/* =========================================================================
 * Lexer matching rules end.
 * =========================================================================
 */
/* End of Lexer code
 * ================================================
 * ================================================
 */ 


/* End of code
 * =============================================================================
 */