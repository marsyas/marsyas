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

#include <marsyas/expr/ExSymTbl.h>
#include <marsyas/expr/ExNode.h>

using std::ostringstream;
using namespace Marsyas;

ExRecord::ExRecord() : ExRefCount()
{
  kind_=0;
  name_="";
  reserved_=false;
}
ExRecord::ExRecord(int kind) : ExRefCount()
{
  kind_=kind;
  name_="";
  reserved_=false;
}

ExRecord::ExRecord(int kind, ExFun* fun, bool reserved) : ExRefCount()
{
  kind_=kind;
  name_=fun->getSignature(); // fully qualified name
  value_.set(fun);
  reserved_=reserved;
}
ExRecord::ExRecord(int kind, std::string name, ExVal& value, bool reserved) : ExRefCount()
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
    std::map<std::string,ExRecord*>::iterator it;
    for(it=syms_.begin(); it!=syms_.end(); ++it) {
      ((*it).second)->deref();
    }
  }
}
//
std::string
ExRecord::getType(std::string path)
{
  if (path=="") return value_.getType();
  ExRecord* r=getRecord(path);
  return (r==NULL) ? "" : r->getType();
}

std::string
ExRecord::getElemType(std::string path)
{
  if (path=="") return value_.getElemType();
  ExRecord* r=getRecord(path);
  return (r==NULL) ? "" : r->getElemType();
}
//

int
ExRecord::getKind(std::string path)
{
  if (path=="") return kind_;
  ExRecord* r=getRecord(path);
  return (r==NULL) ? 0 : r->getKind();
}
//

bool
ExRecord::is_reserved(std::string path)
{
  if (path=="") return reserved_;
  ExRecord* r=getRecord(path);
  return (r==NULL) ? false : r->is_reserved();
}

bool
ExRecord::is_list()
{
  return value_.is_list();
}

bool
ExRecord::is_seq()
{
  return value_.is_seq();
}
//
ExRecord*
ExRecord::getRecord(std::string path)
{
  ExRecord* r=find_sym(path);
  if (r==NULL&&imports_.size()>0) { // try the imports in case path is not fully qualified
    std::vector<std::string>::iterator iter_;
    for (iter_=imports_.begin(); r==NULL&&iter_!=imports_.end(); iter_++) {
      r=find_sym((*iter_)+"."+path);
    }
  }
  return r;
}
bool
ExRecord::params_compare(std::string a, std::string b)
{
  std::string::size_type p1=0;
  std::string::size_type p2=0;
  std::string::size_type len1 = a.length();
  std::string::size_type len2 = b.length();
  std::string::size_type start=1;

  while (p1<len1&&p2<len2) {
    if (a[p1]!=b[p2]) {
      if (a[p1]==','&&b[p2]=='|') {
        while (p2<len2) {
          if (b[p2]==',') break;
          if (b[p2]==')') return false;
          p2++;
        }
        p1++;
        p2++;
      }
      else if (a[p1]==')'&&b[p2]=='|') {
        while (p2<len2) {
          if (b[p2]==',') return false;
          if (b[p2]==')') return true;
          p2++;
        }
        return false;
      }
      else {
        while(p2<len2) {
          if (b[p2]=='|') break;
          if (b[p2]==','||b[p2]==')') return false;
          p2++;
        }
        p1=start; p2++;
      }
    }
    else {
      if (a[p1]==',') start=p1+1;
      p1++; p2++;
    }
    if (a[p1]==')'&&b[p2]==')') return true;
  }
  return false;
}




ExFun*
ExRecord::getFunctionCopy(std::string path)
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
void
ExRecord::split_on(std::string p, char c, std::string& hd, std::string& tl, bool keep)
{
  int adj=(keep) ? 0 : 1;
  size_t i;
  for(i=0; i<p.length()&&p[i]!=c; ++i);
  if (p[i]==c) { hd=p.substr(0,i); tl=p.substr(i+adj,p.length()-i-adj); }
  else { hd=p; tl=""; }
}
//
void
ExRecord::rsplit_on(std::string p, char c, std::string& hd, std::string& tl)
{
  std::string::size_type i = p.length();
  while(i>0)
  {
    --i;
    if(p[i]==c)
    {
      hd=p.substr(0,i); tl=p.substr(i+1,p.length()-i-1); return;
    }
  }
  hd=""; tl=p;
}
//
ExRecord*
ExRecord::find_sym(std::string path)
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
      std::map<std::string,std::string>::iterator it = syms_aliases_.find(ghd);
      // a possible error condition since the path given diverges from the possible paths
      if (it==syms_aliases_.end())
        return NULL;
      else
        return syms_[it->second]->find_sym(gtl);
    }

    return (gi->second)->find_sym(gtl);
  }
  if (path[0]=='(') {
    ExRecord* answer=NULL;
    std::map<std::string,ExRecord*>::iterator is=syms_.begin();
    while (is!=syms_.end()&&!answer) {
      if (params_compare((is->first),path)) { answer=is->second; }
      else is++;
    }
    if (!answer) {
      std::map<std::string,std::string>::iterator it=syms_aliases_.begin();
      while (it!=syms_aliases_.end()&&!answer) {
        if (params_compare((it->first),path))
          answer=syms_[it->second];
        else
          ++it;
      }
    }
    return answer;
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

void
ExRecord::addAliases(std::string path, std::string name)
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
void
ExRecord::addRecord(std::string path, ExRecord* sym)
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
  }
  else {
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
ExRecord*
ExRecord::rmvRecord(std::string path)
{
  // get leading name in group: a|b|c.d|e|f.g|h|i => a|b|c d|e|f.g|h|i
  std::string ghd, gtl; split_on(path,'.',ghd,gtl);
  std::map<std::string, ExRecord*>::iterator rec_i;
  std::map<std::string,std::string>::iterator alias_i;

  if (gtl==""&&ghd[0]!='(') {
    split_on(ghd,'(',ghd,gtl,true);
  }
  rec_i=syms_.find(ghd);
  if (rec_i==syms_.end()) {
    alias_i=syms_aliases_.find(ghd);
    // a possible error condition since the path given diverges from the possible paths
    if (alias_i==syms_aliases_.end()) { return NULL; }
    ghd=alias_i->second;
    rec_i=syms_.find(ghd); // let's assume that gi is valid
  }
  ExRecord* r;
  bool del_rec=false;
  if (gtl!="") r=(rec_i->second)->rmvRecord(gtl);
  else { r=rec_i->second; del_rec=true; }
  if (r!=NULL) {
    std::vector<std::string> names_;
    // eliminate aliases to r by first getting their names
    for (alias_i=syms_aliases_.begin(); alias_i!=syms_aliases_.end(); ++alias_i) {
      if (alias_i->second==ghd) names_.push_back(alias_i->first);
    }
    if (names_.size()>0) {
      // delete aliases from syms_aliases_
      std::vector<std::string>::iterator nmi;
      for (nmi=names_.begin(); nmi!=names_.end(); ++nmi) {
        syms_aliases_.erase(*nmi);
      }
    }
    // eliminate the record from syms_
    syms_.erase(rec_i);
    if (del_rec&&r->size()>0) r->deref();
  }
  return r;
}
void
ExRecord::addReserved(std::string path, ExFun* fun)
{
  addRecord(path,new ExRecord(T_FUN,fun,true));
}
void
ExRecord::addReserved(std::string path, ExVal v, std::string nm, int kind)
{
  addRecord(path,new ExRecord(kind,nm,v,true));
}
//
void
ExRecord::setValue(ExVal& v, std::string path, int elem_pos)
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
      syms->setValue(v,path);
      return;
    }
    else {
      // get leading name in name: a|b|c => a b|c
      std::map<std::string, ExRecord*>::iterator iter=syms_.find(name);
      if (iter==syms_.end()) { // not in table, so add it
        ExRecord* r=new ExRecord(T_VAR,name,v,false); r->inc_ref();
        syms_[name]=r;
        return;
      }
      else {
        (iter->second)->setValue(v);
        return;
      }
    }
  }
  if (getKind()!=T_VAR) {
    MRSWARN("ExRecord::setValue   Attempting assignment to non-variable");
  }
  else if (elem_pos>=0) {
    if (getElemType()!=v.getType()) {
      MRSWARN("ExRecord::setValue   Type mismatch in assignment of element: "+getElemType()+" << "+v.getType());
    }
    else value_.setSeqElem(elem_pos, v);
  }
  else if (getType()!=v.getType()) {
    MRSWARN("ExRecord::setValue   Type mismatch in assignment: "+getType()+" << "+v.getType());
  } else value_=v;
}

// These are pathed names, with no | symbols
ExVal
ExRecord::getValue(std::string path)
{
  if (path=="") return value_;
  ExRecord* s=getRecord(path);
  return (s==NULL) ? false : s->getValue();
}
// add the path 'a' to the imports list
void
ExRecord::import(std::string a)
{
  std::vector<std::string>::iterator p; bool added=false;
  for (p=imports_.begin(); p!=imports_.end(); ++p) {
    if ((*p)==a) { added=true; break; }
  }
  if (!added) imports_.push_back(a);
}
//
void
ExRecord::rmv_import(std::string a)
{
  std::vector<std::string>::iterator p;
  for (p=imports_.begin(); p!=imports_.end(); ++p) {
    if ((*p)==a) { imports_.erase(p); break; }
  }
}

/******************************************************************************/
ExSymTbl::~ExSymTbl()
{
  while (rho_.size()>0) {
    ExRecord* r=rho_.back();
    rho_.pop_back();
    r->deref();
  }
}
void ExSymTbl::block_open()
{
  env_id++;
  ExRecord* r=new ExRecord();
  rho_.push_back(r);
  curr_=r;
  r->inc_ref();
}
void
ExSymTbl::block_close()
{
  if (rho_.size()>0) {
    ExRecord* r=rho_.back();
    rho_.pop_back();
    r->deref();
    if (rho_.size()>0) curr_=rho_.back();
    else curr_=NULL;
  }
}
void
ExSymTbl::addTable(ExRecord* r)
{
  if (r) {
    env_id++;
    rho_.push_back(r);
    curr_=r;
    r->inc_ref();
  }
}
ExRecord*
ExSymTbl::getRecord(std::string nm)
{
  if (rho_.size()>0) {
    std::vector<ExRecord*>::reverse_iterator i;
    for(i=rho_.rbegin(); i!=rho_.rend(); ++i) {
      ExRecord* r=(*i)->getRecord(nm);
      if (r!=NULL) return r;
    }
  }
  return NULL;
}
ExVal
ExSymTbl::getValue(std::string path)
{
  ExRecord* r=getRecord(path);
  return (r) ? r->getValue() : ExVal();
}
ExFun*
ExSymTbl::getFunctionCopy(std::string path)
{
  ExRecord* r=getRecord(path);
  return (r) ? r->getFunctionCopy() : NULL;
}
void
ExSymTbl::import(std::string n)
{
  if (curr_) curr_->import(n);
}
void
ExSymTbl::rmv_import(std::string n)
{
  if (curr_) curr_->rmv_import(n);
}
void
ExSymTbl::addRecord(std::string path, ExRecord* sym)
{
  if (curr_) curr_->addRecord(path,sym);
}
ExRecord*
ExSymTbl::rmvRecord(std::string path)
{
  return (curr_) ? curr_->rmvRecord(path) : NULL;
}
void
ExSymTbl::addReserved(std::string path, ExFun* f)
{
  if (curr_) curr_->addReserved(path,f);
}
void
ExSymTbl::addReserved(std::string path, ExVal v, std::string nm, int kind)
{
  if (curr_) curr_->addReserved(path,v,nm,kind);
}
void
ExSymTbl::setValue(ExVal& v, std::string path)
{
  if (curr_) curr_->setValue(v,path);
}
