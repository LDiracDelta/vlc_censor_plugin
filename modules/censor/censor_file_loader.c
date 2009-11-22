#include "censor.hpp"
#include "CensorLexer.h"
#include "CensorParser.h"


int ANTLR3_CDECL main (int argc, char *argv[]) {

    pANTLR3_UINT8 fName;
    pANTLR3_INPUT_STREAM input;
    pCensorLexer lxr;
    pANTLR3_COMMON_TOKEN_STREAM tstream;
    pCensorParser psr;


    if (argc < 2 || argv[1] == NULL)
    {
        fName =(pANTLR3_UINT8)"./input";
    } else {
        fName = (pANTLR3_UINT8)argv[1];
    }

    input  = antlr3AsciiFileStreamNew(fName);

    if ( input == NULL ) {
        ANTLR3_FPRINTF(stderr, "Unable to open file %s due to malloc() failure1\n", (char *)fName);
    }

    lxr = CensorLexerNew(input);
    if ( lxr == NULL ) {
        ANTLR3_FPRINTF(stderr, "Unable to create the lexer due to malloc() failure1\n");
        exit(ANTLR3_ERR_NOMEM);
    }

    tstream = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT, TOKENSOURCE(lxr));

    if (tstream == NULL) {
        ANTLR3_FPRINTF(stderr, "Out of memory trying to allocate token stream\n");
        exit(ANTLR3_ERR_NOMEM);
    }

    psr = CensorParserNew(tstream);
    if (psr == NULL) {
        ANTLR3_FPRINTF(stderr, "Out of memory trying to allocate parser\n");
        exit(ANTLR3_ERR_NOMEM);
    }

    dynamic_array_t *parser_sections = psr->censorship_file(psr);


    psr     ->free  (psr);      psr     = NULL;
    tstream ->free  (tstream);  tstream = NULL;
    lxr     ->free  (lxr);      lxr     = NULL;
    input   ->close (input);    input   = NULL;

    return 0;
}


