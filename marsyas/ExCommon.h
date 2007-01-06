/*
** Copyright (C) 1998-2007 George Tzanetakis <gtzan@cs.uvic.ca>
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software 
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/
/**
   \class ExNode
   \brief ExNode is the base class for an expression tree node.
   \author Neil Burroughs  inb@cs.uvic.ca
   \version 1.0
   \date    Jan 04, 2007
*/
#ifndef __EX_COMMON_H__
#define __EX_COMMON_H__

enum {
    NONE=0,
    T_CONST,
    T_LIB,
    T_FUN, // a function call
    T_VAR,

    T_REAL,
    T_NATURAL,
    T_STR,
    T_BOOL,
    T_NAME,
    T_CNAME,
    T_COND, // conditional
    OP_BNEG,
    OP_MNEG,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_MOD,
    OP_AND,
    OP_NE,
    OP_OR,
    OP_EQ,
    OP_GT,
    OP_LT,
    OP_GE,
    OP_LE,
    OP_LINK,
    OP_ASGN, // lchild=expr, rchild=nm
    OP_SETCTRL, // lchild=expr, rchild=nm
    OP_GETCTRL,
    OP_CONV
};

#endif

