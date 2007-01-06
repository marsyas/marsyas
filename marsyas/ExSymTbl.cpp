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
    \class ExRecord.cpp
    \brief classes for managing the symbol table - functions and variables
    \author Neil Burroughs  inb@cs.uvic.ca
    \date Jan 4, 2007
*/
#include "ExSymTbl.h"
#include "ExNode.h"

using namespace std;
using namespace Marsyas;

ExRecord::ExRecord(int kind, ExFun* fun, bool reserved)
{
    kind_=kind;
    name_=fun->getSignature(); // fully qualified name
    value_.set(fun);
    reserved_=reserved;
}
ExRecord::ExRecord(int kind, std::string name, ExVal& value, bool reserved)
{
    kind_=kind;
    name_=name;
    value_=value;

    reserved_=reserved;
}
//
ExRecord::~ExRecord()
{
    if (!syms_.empty()) {
        std::map<std::string,ExRecord*>::iterator ni;
        for(ni=syms_.begin();ni!=syms_.end();ni++) {
            ((*ni).second)->deref();
        }
    }
}
//
std::string ExRecord::getType(std::string path)
{
    if (path=="") return value_.getType();
    ExRecord* r=getRecord(path);
    return (r==NULL) ? "" : r->getType();
}
//
int ExRecord::getKind(std::string path)
{
    if (path=="") return kind_;
    ExRecord* r=getRecord(path);
    return (r==NULL) ? 0 : r->getKind();
}
//
bool ExRecord::is_reserved(std::string path)
{
    if (path=="") return reserved_;
    ExRecord* r=getRecord(path);
    return (r==NULL) ? false : r->is_reserved();
}
//
ExRecord* ExRecord::getRecord(std::string path)
{
    ExRecord* r=find_sym(path);
    if (r==NULL&&imports_.size()>0) { // try the imports in case path is not fully qualified
        std::vector<std::string>::iterator iter_;
        for (iter_=imports_.begin();r==NULL&&iter_!=imports_.end();iter_++) {
            r=find_sym((*iter_)+"."+path);
        }
    }
    return r;
}
ExFun* ExRecord::getFunctionCopy(std::string path)
{
    if (path=="") {
        if (kind_!=T_FUN) return NULL;
        ExFun* f=value_.toFun();
        return (f==NULL) ? NULL : f->copy();
    }
    ExRecord* r=getRecord(path);
    return (r==NULL) ? NULL : r->getFunctionCopy();
}
//
void ExRecord::split_on(std::string p, char c, std::string& hd, std::string& tl, bool keep)
{   int adj=(keep) ? 0 : 1;
    unsigned int i; for(i=0;i<p.length()&&p[i]!=c;i++);
    if (p[i]==c) { hd=p.substr(0,i); tl=p.substr(i+adj,p.length()-i-adj); }
    else { hd=p; tl=""; }
}
//
void ExRecord::rsplit_on(std::string p, char c, std::string& hd, std::string& tl)
{
    size_t i; for(i=p.length()-1;i>=0&&p[i]!=c;i++);
    if (p[i]==c) { hd=p.substr(0,i); tl=p.substr(i+1,p.length()-i-1); }
    else { hd=""; tl=p; }
}
//
ExRecord* ExRecord::find_sym(std::string path)
{
    // get leading name in group: a|b|c.d|e|f.g|h|i => a|b|c d|e|f.g|h|i
    std::string ghd, gtl; split_on(path,'.',ghd,gtl);
    if (gtl==""&&ghd[0]!='(') {
        split_on(ghd,'(',ghd,gtl,true);
    }
    if (gtl!="") { // then ghd is the variable name
        // get leading name in name: a|b|c => a b|c
        std::map<std::string, ExRecord*>::iterator gi=syms_.find(ghd);
        if (gi==syms_.end()) {
            std::map<std::string,std::string>::iterator ni=syms_aliases_.find(ghd);
            // a possible error condition since the path given diverges from the possible paths
            if (ni==syms_aliases_.end()) { return NULL; }
            return syms_[ni->second]->find_sym(gtl);
        }
        return (gi->second)->find_sym(gtl);
    }
    std::map<std::string,ExRecord*>::iterator gi=syms_.find(ghd);
    if (gi==syms_.end()) {
        // name not found, so see if it is an alias
        std::map<std::string,std::string>::iterator ni=syms_aliases_.find(ghd);
        if (ni==syms_aliases_.end()) { return NULL; }
        return syms_[ni->second];
    }
    return (gi->second);
}
void ExRecord::addAliases(std::string path, std::string name)
{
    std::string nhd, ntl;
    // the rest of the names are aliases
    split_on(path,'|',nhd,ntl);
    while (nhd!="") {
        syms_aliases_[nhd]=name;
        split_on(ntl,'|',nhd,ntl);
    }
}
//
void ExRecord::addRecord(std::string path, ExRecord* sym)
{
    // get leading name in group: a|b|c.d|e|f.g|h|i => a|b|c d|e|f.g|h|i
    std::string ghd; std::string gtl; split_on(path,'.',ghd,gtl);
    if (gtl!="") { // then ghd is the variable name
        // now split on '|' symbol, get leading name in name: a|b|c => a b|c
        std::string nhd, ntl; split_on(ghd,'|',nhd,ntl);
        std::string name_=nhd;
        std::map<std::string, ExRecord*>::iterator gi=syms_.find(name_);
        ExRecord* syms=NULL;
        // for first split, if name doesn't exist, add it as a record
        if (gi==syms_.end()) { syms=new ExRecord(T_LIB); syms_[name_]=syms; syms->inc_ref(); }
        else { syms=(gi->second); }

        // the rest of the names are aliases
        addAliases(ntl,name_);
        syms->addRecord(gtl,sym);
    } else {
        // discover parameters
        std::string params;
        if (ghd[0]!='(') split_on(ghd,'(',ghd,params,true);
        // get leading name in name: a|b|c => a b|c, becomes the master name
        std::string nhd, ntl; split_on(ghd,'|',nhd,ntl);
        std::map<std::string, ExRecord*>::iterator gi=syms_.find(nhd);
        if (gi==syms_.end()) { // not found, so must be a new name
            if (params!="") {
                ExRecord* e=new ExRecord(T_FUN); e->inc_ref();
                syms_[nhd]=e;
                e->addRecord(params,sym);
            }
            else {
                syms_[nhd]=sym;
                sym->inc_ref();
            }
        }
        else if (params!="") {
            (gi->second)->addRecord(params,sym);
        }
        else {
            MRSWARN("ExRecord::addRecord  '"+nhd+"' already refers to a symbol");
            sym->deref();
            return;
        }
        addAliases(ntl,nhd);
    }
}

void ExRecord::addReserved(std::string path, ExFun* fun)
{
    addRecord(path,new ExRecord(T_FUN,fun,true));
}
void ExRecord::addReserved(std::string path, ExVal v, std::string nm, int kind)
{
    addRecord(path,new ExRecord(kind,nm,v,true));
}
//
void ExRecord::setValue(std::string path, ExVal& v)
{
    if (path!="") {
        // get leading name in group: a|b|c.d|e|f.g|h|i => a|b|c d|e|f.g|h|i
        std::string name; split_on(path,'.',name,path);
        if (path!="") { // then ghd is the variable name
            // get leading name in name: a|b|c => a b|c
            std::map<std::string, ExRecord*>::iterator iter=syms_.find(name);
            ExRecord* syms=NULL;
            if (iter==syms_.end()) { syms=new ExRecord(); syms_[name]=syms; syms->inc_ref(); }
            else { syms=iter->second; }
    
            syms->setValue(path,v);
            return;
        } else {
            // get leading name in name: a|b|c => a b|c
            std::map<std::string, ExRecord*>::iterator iter=syms_.find(name);
            if (iter==syms_.end()) { // not in table, so add it
                ExRecord* r=new ExRecord(T_VAR,name,v,false); r->inc_ref();
                syms_[name]=r;
                return;
            }
            else {
                (iter->second)->setValue("",v);
                return;
            }
        }
    }
    if (getKind()!=T_VAR) {
        MRSWARN("ExRecord::setValue   Attempting assignment to non-variable");
    } else if (getType()!=v.getType()) {
        MRSWARN("ExRecord::setValue   Type mismatch in assignment");
    } else {
        value_=v;
    }
}

// These are pathed names, with no | symbols
ExVal ExRecord::getValue(std::string path)
{
    if (path=="") return value_;
    ExRecord* s=getRecord(path);
    return (s==NULL) ? false : s->getValue();
}
// add the path 'a' to the imports list
void ExRecord::import(std::string a)
{
    std::vector<std::string>::iterator p; bool added=false;
    for (p=imports_.begin();p!=imports_.end();++p) {
        if ((*p)==a) { added=true; break; }
    }
    if (!added) imports_.push_back(a);
}
//
void ExRecord::rmv_import(std::string a)
{
    std::vector<std::string>::iterator p;
    for (p=imports_.begin();p!=imports_.end();++p) {
        if ((*p)==a) { imports_.erase(p); break; }
    }
}

