/* Lex file for tree2vcg.

   Copyright (C) 2009, 2010 Eric Fisher, joefoxreal@gmail.com. 

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>. */

%{
  #include <assert.h>
  #include <string.h>
  #include <libiberty.h>
  #include "tree2vcg-yacc.h"
%}

blanks [[:blank:]]*
digits [[:digit:]]+
identifier [[:alpha:]][[:alnum:]_]*

%option noyywrap

%s fn
%s bb
%s pred
%s succ
%s stmt

%%

^{blanks}";; Function" {
    BEGIN (fn);
    return FN;
  }

<fn>{identifier} {
    yylval.str = xstrdup (yytext);
    BEGIN (INITIAL);
    return FN_NAME;
  }

^{blanks}"# BLOCK" {
    BEGIN (bb);
    return BB;
  }

<bb>{digits} {
    yylval.str = xstrdup (yytext);
    BEGIN (INITIAL);
    return BB_NUM;
  }

^{blanks}"# PRED:" {
    BEGIN (pred);
    return PRED;
  }

<pred>"ENTRY" {
    yylval.str = xstrdup (yytext);
    return PRED_NUM;
  }

<pred>{digits} {
    yylval.str = xstrdup (yytext);
    return PRED_NUM;
  }

^{blanks}"# SUCC:" {
    BEGIN (succ);
    return SUCC;
  }

<succ>"EXIT" {
    yylval.str = xstrdup (yytext);
    return SUCC_NUM;
  }

<succ>{digits} {
    yylval.str = xstrdup (yytext);
    return SUCC_NUM;
  }

<pred,succ>"("[^\)]*")" { /* omit */ }
<pred,succ>"["[^\]]*"]" { /* omit */ }
<pred>\n { BEGIN (stmt); }
<succ>\n { BEGIN (INITIAL); }

<stmt>^{blanks}{identifier}.* {
    /* char *s, *t;

    for (s = yytext; *s == ' ' || *s == '\t'; s++); 
    t = strchr (yytext, '#');
    if (t == NULL)
      t = yytext + strlen (yytext);*/

    char *t;

    t = yytext + strlen (yytext);
    t--;
    while (*t == ' ' || *t == '\t') t--;
    assert (yytext < t);
    yylval.str = xstrndup (yytext, t - yytext + 1);
    return STMT;
  }

. { /* omit others */ }
\n { /* omit others */ }

%%
