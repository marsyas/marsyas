%{

/* Header files used in this code */
#include <vector>
#include <string>
#include <map>
#include <marsyas/MarControlValue.h>
#include <marsyas/Repeat.h>
#include <ruby.h>

using namespace std;

/* Helper functions */
/* Convert string to Ruby VALUE */
static VALUE STR2VAL (string s) { return rb_str_new(s.c_str(),s.size()); }

/* Convert MarControlValue to Ruby VALUE */
static VALUE MCV2VAL (MarControlValue mcv) {
        switch(mcv.getType()) {
        case mar_real: return rb_float_new(mcv.toReal());
        case mar_bool: return ( mcv.toBool() ? Qtrue : Qfalse );
        case mar_natural: return INT2NUM(mcv.toNatural());
        case mar_string: return STR2VAL(mcv.toString());
        case mar_vec:
        default:
        case mar_null: return Qnil;
        }
}

/* Convert Ruby VALUE to string */
static string VAL2STR ( VALUE val ) {
        Check_Type(val,T_STRING);
        return string(STR2CSTR(val));
}

/* Convert Ruby VALUE to MarControlValue */
static MarControlValue VAL2MCV ( VALUE val ) {
        switch(TYPE(val)){
        case T_FLOAT: return MarControlValue(NUM2DBL(val));
        case T_TRUE: return MarControlValue(true);
        case T_FALSE: return MarControlValue(false);
        case T_BIGNUM:
        case T_FIXNUM: return MarControlValue(NUM2INT(val));
        case T_STRING: return MarControlValue(VAL2STR(val));
        default:
        case T_NIL: return MarControlValue();
        }
}

static Repeat VAL2RPT ( VALUE val ) {
	VALUE v1,v2;
	switch(TYPE(val)) {
	case T_STRING: return Repeat(VAL2STR(val));
	case T_ARRAY:
		if (RARRAY(val)->len == 1 && TYPE(v1=(RARRAY(val)->ptr[0]))==T_STRING)
			return Repeat(VAL2STR(v1));
		if (
			RARRAY(val)->len >= 2 &&
			TYPE(v1=(RARRAY(val)->ptr[0]))==T_STRING &&
			TYPE(v2=(RARRAY(val)->ptr[1]))==T_FIXNUM
		)
			return Repeat(VAL2STR(v1),NUM2INT(v2));
	}
	return Repeat();
}

%}

/* Typemaps: Tells SWIG how to handle "close" types so that
 * Class descriptions and / or opaque pointers can be avoided
 */

/* Handle a string as a return value */
%typemap(out) string {
        $result = STR2VAL($1);
}

/* Handle returning a vector of strings
 * as a Ruby Array of String VALUEs
 */
%typemap(out) vector<string> {
        $result = rb_ary_new2($1.size());
        for (int i=0; i<$1.size(); i++)
                rb_ary_push($result,STR2VAL($1.at(i)));
}

/* Handle a MarControlValue return value as the nearest
 * appropriate Ruby VALUE type
 */
%typemap(out) MarControlValue {
        $result = MCV2VAL($1);
}

/* Handle returning a map of strings->MarControlValues
 * as a Ruby hash of Strings -> VALUES
 */
%typemap(out) map<string,MarControlValue> {
        $result = rb_hash_new();
        VALUE key,value;
        for (map<string,MarControlValue>::iterator i = $1.begin(); i != $1.end(); i++ )
        {
                key = STR2VAL((*i).first);
                value = MCV2VAL(i->second);
                rb_hash_aset($result,key,value);
        }
}

/* Handle a MarControlValue input to a function as
 * calling the appropriate constructor.
 */
%typemap(in) MarControlValue {
        $1 = VAL2MCV($input);
}

/* Handle a Repeat value as follows:
 * - NIL -> Repeat()
 * - [] -> Repeat()
 * - string -> Repeat(string)
 * - [string] -> Repeat(string)
 * - [string,int] -> Repeat(string,int)
 * - [string,int,...] -> Repeat(string,int)
 * - Anything Else -> Repeat()
 */
%typemap(in) Repeat { $1 = VAL2RPT($input); }

/* Rename the .so's module, and provide
 * Marsyas:: in marsyas.rb
 */
%module marsyas_ruby

/* Use the common descriptions */
%include "../common/marsyas.i"
