#!/bin/bash
#cd /media/dev/vlc/vlc_new/modules/censor/
export CLASSPATH=/home/ross/antlr
java org.antlr.Tool Censor.g

gcc -I . -I /home/ross/antlrC/include -o censor_file_loader -g3 -O0 -fno-inline \
    -L/home/ross/antlrC/lib/ \
    censor_file_loader.c  CensorLexer.c  CensorParser.c \
    /home/ross/antlrC/lib/libantlr3c.a
