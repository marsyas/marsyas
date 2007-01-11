#
# Copyright (C) 1998-2007 George Tzanetakis <gtzan@cs.uvic.ca>
#  
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software 
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#
#######
# /author Neil Burroughs inb@cs.uvic.ca
# /version 1 January 10, 2007
#
# This script generates library functions for the scheduler expression parser
# language stuff. This script is not guaranteed to be bug free or even useful.
#
# Usage:
#-----------------------------------
#  1: lib Foo|F.Bar|B
#  2:
#  3: pure mrs_string fun|alt(mrs_real a, mrs_natural b)
#  4:     mrs_string s="hello";
#  5:     mrs_bool x;
#  6: {
#  7:     mrs_natural z=a+b;
#  8:     if (x) { s=s+" "+ltos(z); }
#  9:     x = z<0;
# 10:     return s;
# 11: }
#-----------------------------------
# 1. library definition starts with keyword 'lib' the names following denote a
#    path to the library. The true path is Foo.Bar, all functions defined after
#    this statement until a new lib definition will be in this library. This
#    means that the function fun is called by 'Foo.Bar.fun'. Alternate names or
#    aliases for portions of the path can be defined using the | symbol. In the
#    above example F is an alias for Foo so the path to fun could also be
#    written as 'Foo.B.fun' or 'F.B.fun' etc.
#
# 3. the function definition may start with 'pure' where pure implies that if
#    the parameters to the function are constants then the function can be
#    evaluated at parse time to a constant, ie no side-effects. If pure isn't
#    specified then the function is not pure. the return type must be a type
#    supported by the ExVal class (names starting with 'mrs_'). The function
#    name can also have aliases divided by the | symbol where the first name
#    is the true name. Parameters must be defined using the 'mrs_' names.
# 4. Normally functions do not have state but as a bonus variables whose values
#    persist may defined after the parameters definition and prior to the
#    opening function body brace. These types can be the 'mrs_' types or valid
#    C++ types.
# 6. The function body begins with a opening brace {.
# 7-10. The function body contains valid C++ code and will likely use the
#       parameter values defined on line 3.
# 11. The function body ends with a closing brace }.
#
# Built in functions:
# A few built in functions for string conversions can be used:
#     ltos(v) - converts v from a mrs_natural value to a std::string
#     dtos(v) - converts v from a mrs_real value to a std::string
#     btos(v) - converts v from a mrs_bool value to a std::string
#     stol(v) - converts v from a std::string value to a mrs_natural
#
import re
import getopt
import sys

natural_t=['char','unsigned char','signed char',
           'short','unsigned short','signed short',
           'int','unsigned int','signed int',
           'long','unsigned long','signed long',
           'mrs_natural']
real_t=['float','double','mrs_real']
bool_t=['mrs_bool','bool']
string_t=['mrs_string','string','std::string']
timer_t=['mrs_timer','TmTimer**']

methods=[
    ('toNatural',['mrs_natural']),#natural_t),
    ('toReal',['mrs_real']),      #real_t),
    ('toBool',bool_t),
    ('toString',string_t),
    ('toTimer',timer_t)
]
conversions=[
    ('std::string',string_t),
    ('bool',bool_t),
    ('TmTimer**',timer_t)
]
defaults=[
    ('0', natural_t),
    ('0.0',real_t),
    ('false',bool_t),
    ('""',string_t),
    ('NULL',timer_t)
]

default_value={}
for (a,b) in defaults:
    for t in b: default_value[t]=a

to_method={}
for (t,xs) in methods:
    for x in xs:
        to_method[x]=t

conversion={}
for (t,xs) in conversions:
    for x in xs:
        conversion[x]=t

valid_types=['mrs_string','mrs_bool','mrs_natural','mrs_real','mrs_timer']




def trim_nl(line):
    if line==None: return ''
    a=line.find('\n')
    if a<0: return line.strip()
    return line[:a].strip()

def count_leading_whsp(line):
    i=0
    while i<len(line):
        if line[i]<>' ': return i#-1
        i+=1
    return i#-1

class Reader:
    def __init__(self):
        self.me=0
        self.totypes={}
        self.toctypes={}
        self.lnum=0
        self.errors=0
    def complain(self,msg):
        print "Error(line:"+str(self.lnum)+"):",msg
        self.errors+=1
    def reduce_indent(self,lines):
        # discover minimum whitespace tab amount
        lwsp=-1
        for line in lines:
            x=count_leading_whsp(line)
            if lwsp<0 or x<lwsp: lwsp=x
        # remove leading whitespace
        if lwsp>0:
            i=0
            while i<len(lines):
                lines[i]=lines[i][lwsp:]
                i+=1
        return lines
    def un_indent(self,lines,body_start,min_indent):
        i=len(lines)-1
        while i>=body_start:
            lines[i]=lines[i][min_indent:]
            i-=1
    def split_brackets(self,line,(pos,count,quotes,body)):
        new_line=''
        if (pos==0 and len(body)>0) or pos==2:
            new_line=body.pop()
        for c in line:
            if not quotes:
                if c=='{':
                   count+=1
                   if pos==0:
                       pos=1 # discard {
                       if len(new_line)>0:
                           body.append(new_line)
                           new_line=''
#                       body.append('##__FUNCTION_BODY__')
                   else:
                       new_line+=c
                elif c=='}':
                    count-=1
                    if count>0:
                        new_line+=c
                    elif count==0:
                        pos=2
                        if len(new_line)>0:
                            body.append(new_line)
                            new_line=''
                        body.append('##__FUNCTION_END__')
                    else:
                        self.complain("Too many '}' brackets")
                elif c=='"':
                    quotes=True
                    new_line+=c
                else:
                    new_line+=c
            elif c=='"':
                quotes=False
                new_line+=c
            else:
                new_line+=c
        if len(new_line)>0:
            body.append(new_line)
        return (pos,count,quotes,body)
    def parse_fun_name(self,line):
        line=re.sub(r'(\w)\s+(\|)', '\1\2', line) # \w   | => \w|
        line=re.sub(r'(\|)\s+(\w)', '\1\2', line) # \w   | => \w|
        line.strip()
        if line.find(' ')>=0:
            self.complain("invalid function name declaration")
            return ''
        p=line.find('|')
        if p<0: fun_name=line
        else: fun_name=line[:p]
        return (fun_name,line)
    def parse_lib_name(self,line):
        line=re.sub(r'(\w)\s+(\||\.)', '\1\2', line) # \w   | => \w|
        line=re.sub(r'(\||\.)\s+(\w)', '\1\2', line) # \w   | => \w|
        line.strip()
        if line.find(' ')>=0:
            self.complain("invalid function name declaration")
            return None
        xs=line.split('.')
        ys=[]
        for x in xs:
            p=x.find('|')
            if p<0: ys.append(x)
            else: ys.append(x[:p])
        lib_name=''
        lib_path=''
        for y in ys:
            lib_name=lib_name+y.capitalize()
            if len(lib_path)==0: lib_path=y
            else: lib_path=lib_path+'.'+y
        return (lib_name,lib_path,line)
    def chop(self,regex,split_char,params):
        r=re.compile(regex) #
        params=trim_nl(params)
        if len(params)==0: return []
        ps=params.split(split_char)
        nps=[]
        for p in ps:
            p=trim_nl(p)
            if len(p)>0:
                m=r.match(p)
                if m: nps.append([ trim_nl(a) for a in m.groups() ])
                else: return None
        return nps
    def fix_to_methods(self,params):
        new_params=[]
        for p in params:
            if p[0] in to_method.keys():
                t=to_method(p[0])
                new_params.append(p.append(t))
            else:
                self.complain("Invalid parameter type "+p[0])
                return None
        return new_params
    def form_params_tuple(self,params):
        new_params=[]
        s='('
        for p in params:
            if p[0] in to_method.keys():
                t=to_method[p[0]]
                new_params.append(p.append(t))
            else:
                self.complain("Invalid parameter type "+p[0])
                return None
            if len(s)>1: s+=','
            s+=p[0]
        s+=')'
        return (s,params)
    def form_predefs(self,predefs):
        new_predefs=[]
        for p in predefs:
            if p[3]=='':
                if p[0] in default_value.keys():
                    new_predefs.append((p[0],p[1],default_value[p[0]]))
                else:
                    self.complain("invalid pre-defined parameter")
                    return None
            else: new_predefs.append((p[0],p[1],p[3]))
        return new_predefs
    def form_fun_tuple(self,data):
        # mrs_natural huh|who(mrs_real+ c)    mrs_natural a;    mrs_natural b=5;
        regex_fun=re.compile('^\s*(pure\ |)\s*(mrs_\w+)\s+(\w[\w|\|\ ]*)\(([^\)]*)\)\s*(.*)')
        a=data[0]
        body=data[1:]
        m=regex_fun.match(a)
        if not m:
            self.complain("invalid function declaration")
            return None
        g=m.groups()
        pure='false'
        if g[0]=='pure ': pure='true'
        ret_type=g[1]
        if not (ret_type in valid_types):
            self.complain("invalid function return type")
            return None
        fname=self.parse_fun_name(g[2])
        if fname==None: return None
        params=self.chop('(\w+)\s*(\w+)',',',g[3])
        if params==None:
            self.complain("invalid parameter definition")
            return None
        params=self.form_params_tuple(params)
#        params=self.fix_to_methods(params)
        predefs=self.chop('(\w+)\s+(\w+)\s*(\=\s*(.*))?',';',g[4])
        if predefs==None:
            self.complain("invalid class var definition")
            return None
        predefs=self.form_predefs(predefs)
        return (pure,ret_type,fname,params,predefs,body)
    def rd(self,in_file):
        regex_fun=re.compile('^\s*(pure\ |)\s*(mrs_\w+)\s+(.*)')
        regex_lib=re.compile('^\s*lib\s+(.+)')

        fh  = open(in_file, "r")
        body=0
        min_indent=-1
        in_function=False
        fbody=(0,0,False,[])
        result=[]
        for line in fh:
            line=trim_nl(line)
            if in_function:
                fbody=self.split_brackets(line,fbody)
                if fbody[0]==2:
                    data=fbody[3]
                    if data[len(data)-1]<>'##__FUNCTION_END__':
                        c=data.pop() # not doing anything with this, oh well
                    data.pop() # kill the ## thing
                    x=self.form_fun_tuple(data)
                    if x==None: return None
                    result.append(("fun",x))
                    in_function=False
            elif len(line)>0:
                m=regex_fun.match(line)
                if m:
                    fbody=self.split_brackets(line,(0,0,False,[]))#line.find('{')
                    if fbody[0]==2:
                        x=self.form_fun_tuple(fbody[3])
                        if x==None: return None
                        result.append(("fun",x))
                    else: in_function=True
                else:
                    m=regex_lib.match(line)
                    if m:
                        x=self.parse_lib_name(m.groups()[0])
                        if x==None: return None
                        result.append(("lib",x))
        return result

class Gen:
    def __init__(self):
        self.me=0

# ('lib', (
# l1:      'FooBar',
# l2:      'Foo.Bar',
# l3:      'Foo|F.Bar|B'
#          )),
# ('fun', (
# f1        'true',
# f2        'mrs_string',
#           (
# f3            'fun',
# f4            'fun|alt'
#           ),(
# f5            '(mrs_real,mrs_natural)',
# f6            [['mrs_real', 'a', 'toReal'], ['mrs_natural', 'b', 'toNatural']]
#           ),
# f7        [('mrs_string', 's', '"hello"'), ('mrs_bool', 'x', 'false')],
# f8        ['mrs_natural z=a+b;', 'if (x) { s=s+" "+ltos(z); }', 'x = z<0;', 'return s;']
#         ))
    def gen_lib(self,lib,fun):
        (l1,l2,l3)=lib
        (f1,f2,(f3,f4),(f5,f6),f7,f8)=fun
        fname="ExFun_"+l1+f3.capitalize()
#    st->addReserved("Real|R.cos(mrs_real)|(mrs_natural)",new ExFun_RealCos("mrs_real","Real.cos(mrs_real)"));
        s='"'
        if l3<>'': s+=l3+'.'
        s+=f4+f5+'",new '+fname+'("'+f2+'","'
        if l2<>'': s+=l2+'.'
        s+=f3+f5+'")'
        return s
    def gen_fun(self,lib,fun):
        (l1,l2,l3)=lib
        (f1,f2,(f3,f4),(f5,f6),f7,f8)=fun
        fname="ExFun_"+l1+f3.capitalize()
        s ="class "+fname+" : public ExFun { public:\n"
        for p in f7:
            s+="    "+p[0]+" "+p[1]+";\n"
        s+="    "+fname+'() : ExFun("'+f2+'","'+f5+'",'+f1+') {'
        if len(f7)==0: s+=' }\n'
        else:
            s+="\n       "
            for p in f7:
                s+=" "+p[1]+"="
                if p[2]=='': s+=default_value[p[0]]+";"
                else: s+=p[2]+";"
            s+='\n    }\n'
        s+='    virtual ExVal calc() {\n'
        px=0
        for p in f6:
            t=p[0]
            if t in ('mrs_string','string'): t='std::string'
            elif t=='mrs_bool': t='bool'
            s+='        '+t+' '+p[1]+'=(params['+str(px)+']->eval()).'+p[2]+'();\n' # fix this to_method error
            px+=1
        for x in f8: s+=x+'\n'
        s+='    }\n'
        s+='    ExFun* copy() { return new '+fname+'(); }\n'
        s+='};\n'
        return s
    def format_libs(self,data):
        v='void Marsyas::load_symbols(ExRecord* st)\n{\n'
        s=''
        opened=False
        for d in data:
            if d[0]=='#':
                if opened: ret+='}'
                nm='loadlib_'+d[1:]
                s+='void Marsyas::'+nm+'(ExRecord* st)\n{\n'
                v+='    '+nm+'(st);\n'
                opened=True
            else:
                s+='    st->addReserved('+d+');\n'
        if opened: s+='}\n'
        v+='}\n'
        return (v,s)
    def format_funs(self,data):
        s=''
        for d in data: s+=d
        return s
    def gen(self,data):
        funs=[]
        libs=[]
        lib=('','','')
        for (a,b) in data:
            # ('lib', ('OtherWho', 'Other.Who', 'Other|Oth.Who|W'))
            if a=="lib":
                lib=b
                libs.append("#"+b[0])
            elif a=="fun":
                funs.append(self.gen_fun(lib,b))
                libs.append(self.gen_lib(lib,b))
        return (self.format_libs(libs),self.format_funs(funs))

def usage():
    print "Generate Scheduler Expression Functions"
    print "Usage: python scheduler_libs.py infile"

loadsym_h="""/**
 * The load_symbols function is the master function called by the parser to load
 * all libraries. It already exists in ExNode.cpp. Therefore, you need to copy
 * the lines within it into the Marsyas::load_symbols(..) function in ExNode.cpp
 */"""
loadlibs_h="""/***
 * These are the new library functions you defined and should be placed in
 * their own header file, ie "NewLibs.h". You will need to add the line
 * #include "NewLibs.h" at the top of ExNode.cpp so that the load_symbols
 * function can find your header.
 */"""
funs_h="""/***
 * These functions may be placed in their own header file, ie "Fun.h". That
 * header file must be included prior to the definition of the loadlib functions
 * that use these functions, so if the loadlib functions are in "NewLibs.h" then
 * add the lib \'#include "Fun.h"\' at the top of "NewLibs.h"
 */"""

def main():
    try:
        opts, args = getopt.getopt(sys.argv[1:], "o", ["output="])
    except getopt.GetoptError:
        # print help information and exit:
        usage()
        sys.exit(2)
    xs=[]
    r=Reader()
    if len(args)==0:
        usage()
        sys.exit(0)
    for a in args: xs.extend(r.rd(a))
    g=Gen()
    ((loadsym,loadlibs),funs)=g.gen(xs)
    loadsym=loadsym_h+loadsym
    loadlibs=loadlibs_h+loadlibs
    funs=funs_h+funs
    fh = open('Append_to_ExLibs.h', 'w')
    fh.write(loadlibs)
    fh.write(loadsym)
    fh.close()
    fh = open('Append_to_ExFuns.h', 'w')
    fh.write(funs)
    fh.close()

main()
