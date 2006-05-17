#include "ruby.h"
#include "ossp/uuid.h"


VALUE rb_cUUID;
/* symbols */
ID    id_fmt_bin, id_fmt_str, id_fmt_txt;

static uuid_fmt_t
rb2uuid_fmt(symbol)
    VALUE symbol;
{
    ID fmt;
    uuid_fmt_t result;
    
    fmt = rb_to_id(symbol);
    if (fmt == id_fmt_bin)
        result = UUID_FMT_BIN;
    else if (fmt == id_fmt_str)
        result = UUID_FMT_STR;
    else if (fmt == id_fmt_txt)
        result = UUID_FMT_TXT;

    return result;
}

static VALUE
export(const uuid_t *uuid, uuid_fmt_t fmt, void **data_ptr, size_t *data_len)
{
    uuid_rc_t rc;
    VALUE result;
    
    /* dispatch into format-specific functions */
    switch (fmt) {
        case UUID_FMT_BIN:
            rc = uuid_export(uuid, fmt, data_ptr, data_len);
            result = rb_str_new(*data_ptr, UUID_LEN_BIN);
            break;
        case UUID_FMT_STR: 
            rc = uuid_export(uuid, fmt, data_ptr, data_len);
            result = rb_str_new2(*data_ptr);
            break;
        case UUID_FMT_TXT:
            rc = uuid_export(uuid, fmt, data_ptr, data_len);
            result = rb_str_new2(*data_ptr);
            break;
        default:
            rc = UUID_RC_ARG;
    }
 
    return result;
}

/* ------------ */

/**
 *  call-seq:
 *     UUID4R::uuid(1, format = :str)   => uuid
 *     UUID4R::uuid(3, namespace, namespace_str, format = :str)   => uuid
 *     UUID4R::uuid(4, format = :str)   => uuid
 *     UUID4R::uuid(5, namespace, namespace_str, format = :str)   => uuid
 *  
 *  Generates a DCE 1.1 with a specified version: 1, 3, 4 or 5.
 *  Returns an uuid as a specified format (:str, :bin, :txt). 
 */
VALUE
uuid4r_uuid(argc, argv, self)
    int argc;
    VALUE *argv;
    VALUE self;
{
    VALUE version;
    VALUE format;
    VALUE namespace;
    VALUE namespace_str;
    uuid_t *uuid_ns;
    char *uuid_ns_str;
    uuid_fmt_t fmt;
    unsigned int mode;
    uuid_t *uuid;
    char *str;
    VALUE result;

    uuid_ns     = NULL;
    uuid_ns_str = NULL;
    str         = NULL;
    
    switch (argc) {
        case 1:
           rb_scan_args(argc, argv, "11", &version); 
           fmt = UUID_FMT_STR;
           break;
        case 2:
           rb_scan_args(argc, argv, "11", &version, &format); 
           fmt = rb2uuid_fmt(format);           
           break;
        case 3:
           rb_scan_args(argc, argv, "31", &version, &namespace, &namespace_str); 
           fmt = UUID_FMT_STR;
           break;
        case 4:
           rb_scan_args(argc, argv, "31", &version, &namespace, &namespace_str, &format); 
           fmt = rb2uuid_fmt(format);           
           break;
        default:
           rb_raise(rb_eArgError, "wrong argumens");
           break;
    }
    
    switch (NUM2INT(version)) {
        case 1: mode = UUID_MAKE_V1; break;
        case 4: mode = UUID_MAKE_V4; break;
        case 3:
            mode = UUID_MAKE_V3;
            uuid_create(&uuid_ns);
            uuid_load(uuid_ns, StringValueCStr(namespace)); 
            uuid_ns_str = StringValueCStr(namespace_str);
            break;
        case 5: 
            mode = UUID_MAKE_V5;
            uuid_create(&uuid_ns);
            uuid_load(uuid_ns, StringValueCStr(namespace)); 
            uuid_ns_str = StringValueCStr(namespace_str);
            break;
    }
    uuid_create(&uuid);
    uuid_make(uuid, mode, uuid_ns, uuid_ns_str);
    result = export(uuid, fmt, (void **)&str, NULL);
    uuid_destroy(uuid);
    free(str); // str should be freed.

    return result;
}

/**
 *  call-seq:
 *     UUID4R::uuid_v1(format = :str)   => uuid
 *  
 *  Generates a DCE 1.1 v1 UUID from system environment.
 *  Returns an uuid as a specified format (:str, :bin, :txt). 
 */
VALUE
uuid4r_uuid_v1(argc, argv, self)
    int argc;
    VALUE *argv;
    VALUE self;
{
    VALUE rest;
    
    if ( rb_scan_args(argc, argv, "01", &rest) == 1)
        return rb_funcall(self, rb_intern("uuid"), 2, INT2NUM(1), rest);
    else
        return rb_funcall(self, rb_intern("uuid"), 1, INT2NUM(1));        
}

/**
 *  call-seq:
 *     UUID4R::uuid_v4(format = :str)   => uuid
 *  
 *  Generates a DCE 1.1 v4 UUID based a random number.
 *  Returns an uuid as a specified format (:str, :bin, :txt). 
 */
VALUE
uuid4r_uuid_v4(argc, argv, self)
    int argc;
    VALUE *argv;
    VALUE self;
{
    VALUE rest;
    
    if ( rb_scan_args(argc, argv, "01", &rest) == 1)
        return rb_funcall(self, rb_intern("uuid"), 2, INT2NUM(4), rest);
    else
        return rb_funcall(self, rb_intern("uuid"), 1, INT2NUM(4));        
}

/**
 *  call-seq:
 *     UUID4R::uuid_v3(namespace, namespace_str, format = :str)   => uuid
 *  
 *  Generates a DCE 1.1 v3 UUID with a name based with MD5.
 *  Returns an uuid as a specified format (:str, :bin, :txt). 
 */
VALUE
uuid4r_uuid_v3(argc, argv, self)
    int argc;
    VALUE *argv;
    VALUE self;
{
    VALUE namespace, namespace_str;
    VALUE rest;
    
    if ( rb_scan_args(argc, argv, "21", &namespace, &namespace_str, &rest) == 3)
        return rb_funcall(self, rb_intern("uuid"), 4, 
            INT2NUM(3), namespace, namespace_str, rest);
    else
        return rb_funcall(self, rb_intern("uuid"), 3, 
            INT2NUM(3), namespace, namespace_str);        
}

/**
 *  call-seq:
 *     UUID4R::uuid_v5(namespace, namespace_str, format = :str)   => uuid
 *  
 *  Generates a DCE 1.1 v5 UUID with a name based with SHA-1.
 *  Returns an uuid as a specified format (:str, :bin, :txt). 
 */
VALUE
uuid4r_uuid_v5(argc, argv, self)
    int argc;
    VALUE *argv;
    VALUE self;
{
    VALUE namespace, namespace_str;
    VALUE rest;
    
    if ( rb_scan_args(argc, argv, "21", &namespace, &namespace_str, &rest) == 3)
        return rb_funcall(self, rb_intern("uuid"), 4, 
            INT2NUM(5), namespace, namespace_str, rest);
    else
        return rb_funcall(self, rb_intern("uuid"), 3, 
            INT2NUM(5), namespace, namespace_str);        
}

void Init_uuid4r (void) {
    /* regist symbols */
    id_fmt_bin = rb_intern("bin");
    id_fmt_str = rb_intern("str");
    id_fmt_txt = rb_intern("txt");

    rb_cUUID      = rb_define_class("UUID4R", rb_cObject);

    /* utilities */
    rb_define_module_function(rb_cUUID, "uuid", uuid4r_uuid, -1);
    rb_define_module_function(rb_cUUID, "uuid_v1", uuid4r_uuid_v1, -1);
    rb_define_module_function(rb_cUUID, "uuid_v3", uuid4r_uuid_v3, -1);
    rb_define_module_function(rb_cUUID, "uuid_v4", uuid4r_uuid_v4, -1);
    rb_define_module_function(rb_cUUID, "uuid_v5", uuid4r_uuid_v5, -1);
}

