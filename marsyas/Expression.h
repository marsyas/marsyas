/*
** Copyright (C) 1998-2005 George Tzanetakis <gtzan@cs.uvic.ca>
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
   \class MarEvent
   \brief MarEvent 

*/
#if 0
#include "EvExpUpd.h"
#include "MarSystem.h"
#include "MrsLog.h" 

namespace Marsyas
{

struct Expr {
    public:
    unsigned char sym; unsigned char tsym;
    std::string val; bool enclosed;
    int usage_count;
    Expr() { init("",0,0); }
    Expr(std::string v) { init(v,0,0); }
    Expr(std::string v, char s) { init(v,s,0); }
    Expr(std::string v, char s, char t) { init(v,s,t); }
    void init(std::string v, char s, char t) {
        next=NULL;
        lchild=rchild=NULL;
        params=pnext=NULL;
        sym=s; tsym=t; val=v;
        enclosed=false;
        usage_count=0;
    }
    Expr* next; // list pointers
    Expr* lchild; Expr* rchild; // tree pointers
    Expr* params; Expr* pnext; // function parameters
};

struct Val {
    public:
    mrs_real r;
    mrs_natural n;
    bool isreal;
    Val() {r=0;};
    Val(const Val& v) { isreal=v.isreal; r=v.r; n=v.n; };
    Val(mrs_real nr) { r=nr; isreal=true; };
    Val(mrs_natural nn) { n=nn; isreal=false; };
    bool operator==(const Val& v) {
        if (isreal!=v.isreal) { return false; }
        if (isreal) { return r==v.r; }
        return n==v.n;
    };
    Val& operator= (const Val& v) {
        if (this != &v) { r=v.r; n=v.n; isreal=v.isreal; }
        return *this;
    };
    Val operator-(void) {
        if (isreal) { return Val(-r); }
        return Val(-n);
    };
#define ADD_OPERATOR_OVERRIDE(_T) \
    friend Val operator _T(const Val& v1, const Val& v2) {\
        if (v1.isreal&&v2.isreal) { return Val(v1.r  _T v2.r); }\
        if (v1.isreal&&!v2.isreal) { return Val(v1.r _T v2.n); }\
        if (!v1.isreal&&v2.isreal) { return Val(v1.n _T v2.r); }\
        return Val(v1.n _T v2.n);\
    }
    ADD_OPERATOR_OVERRIDE(+);
    ADD_OPERATOR_OVERRIDE(-);
    ADD_OPERATOR_OVERRIDE(*);
    ADD_OPERATOR_OVERRIDE(/);
};

struct Expr* lex(bool* fail, std::string s);
struct Expr* symbol(bool *fail, std::string sym, unsigned char type);
struct Expr* parse(bool* fail, struct Expr* list);
struct Expr* parse_expr(bool* fail, struct Expr** list);

mrs_real string2real(std::string s);
mrs_natural string2natural(std::string s);
Val eval(bool* fail, MarSystem* target_, struct Expr* tree);
void clear_expr_list(struct Expr* e);

}//namespace Marsyas


#endif // if 0
