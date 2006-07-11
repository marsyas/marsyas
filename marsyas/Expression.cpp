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
#include "Expression.h"
#include "MarSystem.h"

using namespace std;
using namespace Marsyas;

namespace Marsyas
{

enum {
    SYM_NONE,
    SYM_NATURAL,
    SYM_REAL,
    SYM_NAME,
    SYM_CNAME,
    SYM_FUN,
    SYM_OPER,
    SYM_OP_PLUS,
    SYM_OP_MINUS,
    SYM_OP_MUL,
    SYM_OP_DIV,
    SYM_DOT,
    SYM_LBRKT,
    SYM_RBRKT,
    SYM_COMMA,

    SYM_LAST // not a symbol, for limits checking
};

void clear_expr_list(struct Expr* e) {
    if (e!=NULL) {
        e->usage_count--; // decrement reference count
        if (e->usage_count<1) {
            while (e != NULL) {
                struct Expr* n = e->next;
                delete(e);
                e=n;
            }
        }
    }
}

/*
ostream& operator<< (ostream& o, EvValUpd& e) {
//    sys.put(o);
    o << "EvValUpd<" << e.getCName() << "," << e.getValue() << ">";
    return o;
}
*/

//#define COUGH(_MSG) mrsErr("Expression: "+_MSG); *fail=true;
#define COUGH(_MSG) *fail=true;
/********************/
#define _S s[i]
#define DOT (_S=='.')
#define NUM ((_S>='0') && (_S<='9'))
#define LETTER (((_S>='A')&&(_S<='Z'))||((_S>='a')&&(_S<='z')))
#define WORD (NUM || LETTER || (_S=='_'))
#define WHITE ((_S==' ')||(_S=='\t')||(_S=='\n'))
#define OPER ((_S=='+')||(_S=='-')||(_S=='*')||(_S=='/'))
#define PATH_DELIM (_S=='/')
#define LBRKT (_S=='(')
#define RBRKT (_S==')')
#define COMMA (_S==',')
#define APPENDSYM(_S,_T) \
    if (sym!="") { \
        struct Expr* _e = symbol(fail,_S,_T); \
        if (_e!=NULL) { \
            if (head==NULL) { head=_e; tail=_e; } \
            else { tail->next=_e; tail=_e; } \
        } \
        sym=""; symt=SYM_NONE; \
    }
#define SYMADD sym+=s[i++];
#define CURSYM(_S) symt==_S

struct Expr* lex(bool* fail, string s) {
    char symt=SYM_NONE;
    string sym="";
    unsigned int i=0;
    struct Expr* head = NULL;
    struct Expr* tail = NULL;

    while (i<s.length() && !*fail) {
        if (CURSYM(SYM_NONE)) {
            if (LETTER) { SYMADD; symt=SYM_NAME; }
            else if (NUM) { SYMADD; symt=SYM_NATURAL; }
            else if (OPER) { SYMADD; symt=SYM_OPER; }
            else if (WHITE) { i++; }
            else if (LBRKT) { SYMADD; APPENDSYM(sym,SYM_LBRKT); }
            else if (RBRKT) { SYMADD; APPENDSYM(sym,SYM_RBRKT); }
            else if (DOT) { SYMADD; APPENDSYM(sym,SYM_DOT); }
            else if (COMMA) { SYMADD; APPENDSYM(sym,SYM_COMMA); }
            else { COUGH("Expr (Lex): Invalid char."); }
        } else if (CURSYM(SYM_NAME)) {
            if (WORD) { SYMADD; }
            else if (PATH_DELIM) { SYMADD; symt=SYM_CNAME; }
            else { APPENDSYM(sym,symt); }
        } else if (CURSYM(SYM_CNAME)) {
            if (WORD || PATH_DELIM) { SYMADD; }
            else { APPENDSYM(sym,symt); }
        } else if (CURSYM(SYM_NATURAL)) {
            if (NUM) { SYMADD; }
            else if (DOT) { SYMADD; symt=SYM_REAL; }
            else if (WHITE | RBRKT | OPER | COMMA) { APPENDSYM(sym,symt); }
            else { COUGH("Expr (Lex): Invalid char."); }
        } else if (CURSYM(SYM_REAL)) {
            if (NUM) { SYMADD; }
            else if (WHITE | RBRKT | OPER | COMMA) { APPENDSYM(sym,symt); }
            else { COUGH("Expr (Lex): Invalid char."); }
        } else if (CURSYM(SYM_OPER)) {
            if (sym=="/" && WORD) { SYMADD; symt=SYM_CNAME; }
            else { APPENDSYM(sym,symt); }
        }
    }
    if (!*fail) { APPENDSYM(sym,symt); }
    return head;
}
/* create new symbols for lexer */
struct Expr* symbol(bool* fail, string sym, unsigned char type) {
    if (type<SYM_LAST) {
        Expr* e = new Expr(sym,type);
        if (type==SYM_OPER) {
            if (sym=="+") { e->tsym=SYM_OP_PLUS; }
            else if (sym=="-") { e->tsym=SYM_OP_MINUS; }
            else if (sym=="*") { e->tsym=SYM_OP_MUL; }
            else if (sym=="/") { e->tsym=SYM_OP_DIV; }
            else { COUGH("Expr (Lex): Unknown symbol type."); }
        }
        return e;
    }
    COUGH("Expr (Lex): Unknown symbol type.");
    return NULL;
}

/********************/
#define TOK (*list)
#define CURR(_T) (TOK->sym==_T)
#define NEXT(_T) ((TOK->next!=NULL)&&(TOK->next->sym==_T))
#define NNEXT(_T) ((TOK->next->next!=NULL)&&(TOK->next->next->sym==_T))
#define MATCH1(_T) (CURR(_T))
#define MATCH2(_T1,_T2) (CURR(_T1)&&(NEXT(_T2)))
#define MATCH3(_T1,_T2,_T3) (CURR(_T1)&&(NEXT(_T2))&&(NNEXT(_T3)))
#define TOK_ADVANCE TOK=TOK->next;

/* Main entry point for parsing. */
struct Expr* parse(bool* fail, struct Expr* list) {
    struct Expr* e = list; // preserve the list pointer by copying
    return parse_expr(fail,&e);
}

/* use a Expr** list so that consuming tokens propagates across all function
   calls. Like consuming stream tokens. *fail as the return value cannot be
   used to return errors.
   DO NOT allow the Expr->next pointer to be modified as it is used for garbage
   cleanup.
   */
struct Expr* parse_expr(bool* fail, struct Expr** list) {
    if (list==NULL) { return NULL; }
    struct Expr* root=NULL;
    struct Expr* curr=NULL;
    while (TOK != NULL && !*fail) {
        if (MATCH3(SYM_NAME,SYM_DOT,SYM_NAME)) {
            // combine 'name' '.' 'name' tokens into 'name.name'
            TOK->val = TOK->val + "." + TOK->next->next->val;
            // delete next two
            struct Expr* del1 = TOK->next;
            struct Expr* del2 = del1->next;
            TOK->next=del2->next;
            delete(del1); delete(del2);
        } else {
            if (MATCH2(SYM_NAME,SYM_LBRKT)) { // fun:=name(...)
                // convert to a function token
                TOK->sym=SYM_FUN;
                struct Expr* e = TOK; // save TOK pointer to this func
                TOK=TOK->next->next; // advance to first param or )
                while(TOK!=NULL && TOK->sym!=SYM_RBRKT) {
                    // call parse_expr and append to parameters list
					struct Expr* o = Marsyas::parse_expr(fail,list);
                    if (TOK!=NULL && o!=NULL) {
                        if (e->params==NULL) { e->params=o; }
                        else {
                            struct Expr* p = e->params;
                            while(p->pnext!=NULL) { p=p->pnext; }
                            p->pnext=o; o->pnext=NULL;
                        }
                    }
                    // TOK is now on a COMMA, RBRKT, or NULL
                    if (TOK!=NULL&&CURR(SYM_COMMA)) {
                        TOK_ADVANCE;
                        if (TOK!=NULL&&CURR(SYM_RBRKT)) {
                            COUGH("Expr (Parse): Empty function parameter.");
                        }
                    }
                }
                // finished parsing function, add to expr tree
                if (root==NULL) { root=e; curr=e; }
                else if (curr->rchild!=NULL) {
                    COUGH("Expr (Parse): Missing operator.");
                }
                else { curr->rchild=e; }
            } else if (CURR(SYM_LBRKT)) {
                TOK_ADVANCE;
                // parse the next token as an expression
				struct Expr* e = Marsyas::parse_expr(fail,list);
                if (root==NULL) { root=e; curr=e; }
                else { curr->rchild=e; }
                e->enclosed=true; // signal (expr) is non-modifiable
            } else if (CURR(SYM_RBRKT)||CURR(SYM_COMMA)) {
                return root;
            } else if (CURR(SYM_OPER)) {
                if (TOK->tsym==SYM_OP_PLUS||TOK->tsym==SYM_OP_MINUS) {
                    if (root==NULL&&TOK->tsym==SYM_OP_MINUS) {
                        root=TOK; curr=TOK;
                    } else if (root==NULL) {
                        COUGH("Expr (Parse): Unexpected operator.");
                    } else {
                        TOK->lchild=root;
                        root=TOK; curr=TOK;
                    }
                } else if (TOK->tsym==SYM_OP_MUL||TOK->tsym==SYM_OP_DIV) {
                    if (root==NULL) { COUGH("Expr (Parse): Unexpected operator."); }
                    else {
                        // if the root is an enclosed expression then this op
                        // becomes root
                        if (root->sym!=SYM_OPER||root->enclosed) {
                            TOK->lchild=root;
                            root=TOK; curr=TOK;
                        } else if (root->rchild==NULL) {
                            COUGH("Expr (Parse): Too many operators.");
                        // otherwise this op goes to the right child to preserve
                        // evaluation order > (a+b*c) = + a * b c
                        } else {
                            TOK->lchild=root->rchild;
                            root->rchild=TOK;
                            curr=TOK;
                        }
                    }
                } else { COUGH("Expr (Parse): Unknown operator."); }

#define ADD_TO_RCHILD_WITH_ERROR(_ERR_MSG) \
                if (root==NULL) { root=TOK; curr=TOK; } \
                else if (curr->sym==SYM_OPER) { \
                    if (curr->rchild==NULL) { \
                        curr->rchild=TOK; \
                    } else { COUGH(_ERR_MSG); } } \

            } else if (CURR(SYM_NATURAL)||CURR(SYM_REAL)) {
                ADD_TO_RCHILD_WITH_ERROR("Expr (Parse): Unexpected value.");
            } else if (CURR(SYM_NAME)) {
                ADD_TO_RCHILD_WITH_ERROR("Expr (Parse): Unexpected name.");
            } else if (CURR(SYM_CNAME)) {
                ADD_TO_RCHILD_WITH_ERROR("Expr (Parse): Unexpected cname.");
            }
            else { cout << (int)TOK->sym << endl; COUGH("Expr (Parse): Unknown token."); }
            if (TOK!=NULL) { TOK_ADVANCE; }
        }
    }
    return root;
}
/* quick and dirty evaluator. To add functions place them in the
   SYM_FUN spot. To add name values place them in the SYM_NAME spot. */
Val eval(bool* fail, MarSystem* target_, struct Expr* tree) {
    if (!*fail) {
        if (tree->sym==SYM_FUN) {
            int num_params = 0;
            struct Expr* e = tree->params;
            while (e!=NULL) { e = e->next; num_params++; }
            if (tree->val=="Math.rand" && num_params==0) {
                return Val((mrs_real)rand());
            } else { COUGH("Expr (Eval): Unknown name."); }
        } else if (tree->sym==SYM_NAME) {
            if (tree->val=="Math.RAND_MAX") { return Val((mrs_real)RAND_MAX); }
            else { COUGH("Expr (Eval): Unknown name."); }
        } else if (tree->sym==SYM_REAL) {
            return (mrs_real)atof((tree->val).c_str());
        } else if (tree->sym==SYM_NATURAL) {
            return (mrs_natural)atoi(tree->val.c_str());
        } else if (tree->tsym==SYM_OP_MINUS && tree->lchild==NULL) {
			return - Marsyas::eval(fail,target_,tree->lchild);
        } else if (tree->sym==SYM_OPER) {
			Val l = Marsyas::eval(fail,target_,tree->lchild);
			Val r = Marsyas::eval(fail,target_,tree->rchild);
            if (tree->tsym==SYM_OP_PLUS) { return l+r; }
            else if (tree->tsym==SYM_OP_MINUS) { return l-r; }
            else if (tree->tsym==SYM_OP_MUL) { return l*r; }
            else if (tree->tsym==SYM_OP_DIV) { return l/r; }
            else { COUGH("Expr (Eval): Unknown operator."); }
        } else if (tree->sym==SYM_CNAME) {
            if (target_!=NULL) {
                string s = "/"+target_->getType()+"/"+target_->getName()+"/"+tree->val;
                if (target_->hasControl(s)) {
                    MarControlValue v = target_->getctrl(tree->val);
                    if (v.getType()==mar_real) { return (mrs_real)v.toReal(); }
                    else if (v.getType()==mar_natural) { return (mrs_natural)v.toNatural(); }
                    COUGH("Expr (Eval): Controls must have mrs_real/mrs_natural values.");
                } else { COUGH("Expr (Eval): Control does not exist."); }
            }
        }
        COUGH("Expr (Eval): Unknown symbol.");
    }
    return Val(); // this is an error, but must return something
}

}//namespace Marsyas




