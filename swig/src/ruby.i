%module marsyas_ruby

%{
#include <ruby.h>

/* Helper functions */
/* Convert string to Ruby VALUE */
static VALUE STR2VAL (string s) { return rb_str_new(s.c_str(),s.size()); }

/* Convert MarControlPtr to Ruby VALUE */
static VALUE MCP2VAL (MarControlPtr mcp)
{
	if (mcp.isInvalid()) return Qnil;
	if (mcp->getType() == "mrs_natural") return INT2NUM(mcp->to<mrs_natural>()());
	if (mcp->getType() == "mrs_real" ) return rb_float_new(mcp->to<mrs_real>()()());
	if (mcp->getType() == "mrs_string" ) return STR2VAL(mcp->toString());
	if (mcp->getType() == "mrs_bool" ) return mcp->toBool() ? Qtrue : Qfalse ;
	return Qnil;
}

static string VAL2STR ( VALUE val ) {
        Check_Type(val,T_STRING);
        return string(STR2CSTR(val));
}

static MarControlPtr VAL2MCP (VALUE val)
{
	switch(TYPE(val)){
        	case T_FLOAT: return MarControlPtr(NUM2DBL(val));
        	case T_BIGNUM:
        	case T_FIXNUM: return MarControlPtr(NUM2INT(val));
        	case T_STRING: return MarControlPtr(VAL2STR(val));
        	case T_TRUE: return MarControlPtr(true);
        	case T_FALSE: return MarControlPtr(false);
        	default:
        	case T_NIL: return MarControlPtr();
        }
}

%}

%typemap(out) map<string,MarControlPtr> {
        $result = rb_hash_new();
        VALUE key,value;
        for (map<string,MarControlPtr>::iterator i = $1.begin(); i != $1.end(); i++ )
        {
                key = STR2VAL(i->first);
                value = MCP2VAL(i->second);
                rb_hash_aset($result,key,value);
        }
}

%typemap(out) vector<string> {
        $result = rb_ary_new2($1.size());
        for (int i=0; i<$1.size(); i++)
                rb_ary_push($result,STR2VAL($1.at(i)));
}

%typemap(in) string = char*;
%typemap(typecheck) string = char *;
%typemap(out) string { $result = STR2VAL($1); };

%typemap(in) MarControlPtr { $1 = VAL2MCP($input); }
%typemap(out) MarControlPtr { $result = MCP2VAL($1); }

/*
class MarControlPtr {
        public:
		MarControlPtr(MarControlPtr);
                bool isInvalid();
};

%extend MarControlPtr {
        bool setValue ( VALUE v ) {  return (*self)->setValue(VAL2MCP(v)); }
	VALUE getValue (void) { return MCP2VAL(*self); }
}
*/
