/*
 *  COPYRIGHT AND LICENSE
 *
 *  Copyright (C) 2006 Daigo Moriwaki <daigo@debian.org>
 *
 *  Permission to use, copy, modify, and distribute this software for
 *  any purpose with or without fee is hereby granted, provided that
 *  the above copyright notice and this permission notice appear in all
 *  copies.
 *
 *  THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 *  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL THE AUTHORS AND COPYRIGHT HOLDERS AND THEIR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 *  USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 *  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 *  OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 *  SUCH DAMAGE.
 *
 */

#include "ruby.h"
#include "uuid.h"

VALUE rb_cUUID4R;
VALUE rb_cUUID4RCommon;
VALUE rb_cUUID4Rv1;
VALUE rb_cUUID4Rv3;
VALUE rb_cUUID4Rv4;
VALUE rb_cUUID4Rv5;
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
    else
        rb_raise(rb_eArgError, "wrong argument");

    return result;
}

static VALUE
export(uuid, fmt)
    const uuid_t *uuid;
    uuid_fmt_t fmt;
{
    uuid_rc_t rc;
    char *str;
    char *ptr; //uuid_uint8_t
    VALUE result;

    str = NULL;
    ptr = NULL;

    /* dispatch into format-specific functions */
    switch (fmt) {
        case UUID_FMT_BIN:
            rc = uuid_export(uuid, fmt, (void **)&ptr, NULL);
            result = rb_str_new(ptr, UUID_LEN_BIN);
            free(ptr);
            break;
        case UUID_FMT_STR:
            rc = uuid_export(uuid, fmt, (void **)&str, NULL);
            result = rb_str_new2(str);
            free(str);
            break;
        case UUID_FMT_TXT:
            rc = uuid_export(uuid, fmt, (void **)&str, NULL);
            result = rb_str_new2(str);
            free(str);
            break;
        default:
            rb_raise(rb_eArgError, "wrong argument");
    }

    return result;
}




static void
uuid4r_free(uuid)
    uuid_t *uuid;
{
    uuid_destroy(uuid);
}

static VALUE
uuid4r_alloc(klass)
    VALUE klass;
{
    VALUE obj;
    uuid_t *uuid;

    uuid_create(&uuid);
    obj = Data_Wrap_Struct(klass, 0, uuid4r_free, uuid);

    return obj;
}

/* UUID4R common */

VALUE
uuid4r_compare(lhs, rhs)
    VALUE lhs, rhs;
{
    const uuid_t *uuid_lhs;
    const uuid_t *uuid_rhs;
    int value;

    Data_Get_Struct(lhs, uuid_t, uuid_lhs);
    Data_Get_Struct(rhs, uuid_t, uuid_rhs);
    uuid_compare(uuid_lhs, uuid_rhs, &value);

    return INT2NUM(value);
}

VALUE
uuid4r_export(argc, argv, self)
    int argc;
    VALUE *argv;
    VALUE self;
{
    VALUE format;
    uuid_fmt_t fmt;
    uuid_t *uuid;

    if ( rb_scan_args(argc, argv, "01", &format) == 1)
        fmt = rb2uuid_fmt(format);
    else
        fmt = UUID_FMT_STR;

    Data_Get_Struct(self, uuid_t, uuid);

    return export(uuid, fmt);
}

VALUE
uuid4r_import(self, format, str)
    VALUE self, format, str;
{
    uuid_fmt_t fmt;
    uuid_t *uuid;

    uuid_create(&uuid);
    fmt = rb2uuid_fmt(format);
    StringValue(str);
    uuid_import(uuid, fmt, RSTRING_PTR(str), RSTRING_LEN(str));

    return Data_Wrap_Struct(rb_cUUID4RCommon, 0, uuid4r_free, uuid);
}

/* UUID4Rv1 */

VALUE
uuid4rv1_initialize(self)
    VALUE self;
{
    uuid_t *uuid;

    Data_Get_Struct(self, uuid_t, uuid);
    uuid_make(uuid, UUID_MAKE_V1);

    return self;
}

/* UUID4Rv3 */

VALUE
uuid4rv3_initialize(self, namespace, namespace_str)
    VALUE self;
    VALUE namespace;
    VALUE namespace_str;
{
    uuid_t *uuid;
    uuid_t *uuid_ns;
    char *uuid_ns_str;

    uuid_ns     = NULL;
    uuid_ns_str = NULL;
    uuid_create(&uuid_ns);
    uuid_load(uuid_ns, StringValueCStr(namespace));
    uuid_ns_str = StringValueCStr(namespace_str);

    Data_Get_Struct(self, uuid_t, uuid);
    uuid_make(uuid, UUID_MAKE_V5, uuid_ns, uuid_ns_str);

    return self;
}

/* UUID4Rv4 */

VALUE
uuid4rv4_initialize(self)
    VALUE self;
{
    uuid_t *uuid;

    Data_Get_Struct(self, uuid_t, uuid);
    uuid_make(uuid, UUID_MAKE_V4);

    return self;
}

/* UUID4Rv5 */

VALUE
uuid4rv5_initialize(self, namespace, namespace_str)
    VALUE self;
    VALUE namespace;
    VALUE namespace_str;
{
    uuid_t *uuid;
    uuid_t *uuid_ns;
    char *uuid_ns_str;

    uuid_ns     = NULL;
    uuid_ns_str = NULL;
    uuid_create(&uuid_ns);
    uuid_load(uuid_ns, StringValueCStr(namespace));
    uuid_ns_str = StringValueCStr(namespace_str);

    Data_Get_Struct(self, uuid_t, uuid);
    uuid_make(uuid, UUID_MAKE_V5, uuid_ns, uuid_ns_str);

    return self;
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
    unsigned int mode = 0; /* suppress compiler warning */
    uuid_t *uuid;
    VALUE result;

    uuid_ns     = NULL;
    uuid_ns_str = NULL;

    switch (argc) {
        case 1:
           rb_scan_args(argc, argv, "11", &version, &format);
           fmt = UUID_FMT_STR;
           break;
        case 2:
           rb_scan_args(argc, argv, "11", &version, &format);
           fmt = rb2uuid_fmt(format);
           break;
        case 3:
           rb_scan_args(argc, argv, "31", &version, &namespace, &namespace_str, &format);
           fmt = UUID_FMT_STR;
           break;
        case 4:
           rb_scan_args(argc, argv, "31", &version, &namespace, &namespace_str, &format);
           fmt = rb2uuid_fmt(format);
           break;
        default:
           rb_raise(rb_eArgError, "wrong argument");
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
    result = export(uuid, fmt);
    if (uuid_ns)
      uuid_destroy(uuid_ns);
    uuid_destroy(uuid);

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

    /* ------ UUID4R ------ */
    rb_cUUID4R      = rb_define_class("UUID4R", rb_cObject);
    /* utilities */
    rb_define_module_function(rb_cUUID4R, "uuid",    uuid4r_uuid,    -1);
    rb_define_module_function(rb_cUUID4R, "uuid_v1", uuid4r_uuid_v1, -1);
    rb_define_module_function(rb_cUUID4R, "uuid_v3", uuid4r_uuid_v3, -1);
    rb_define_module_function(rb_cUUID4R, "uuid_v4", uuid4r_uuid_v4, -1);
    rb_define_module_function(rb_cUUID4R, "uuid_v5", uuid4r_uuid_v5, -1);
    rb_define_module_function(rb_cUUID4R, "import",  uuid4r_import,   2);

    /* ------ UUID4RCommon ------ */
    rb_cUUID4RCommon = rb_define_class_under(rb_cUUID4R, "UUID4RCommon", rb_cObject);
    rb_define_alloc_func(rb_cUUID4RCommon, uuid4r_alloc);
    rb_define_method(rb_cUUID4RCommon, "export",  uuid4r_export, -1);
    rb_define_method(rb_cUUID4RCommon, "compare", uuid4r_compare, 1);
    rb_define_alias(rb_cUUID4RCommon,  "<=>", "compare");

    /* ------ UUID4Rv1 ------ */
    rb_cUUID4Rv1 = rb_define_class_under(rb_cUUID4R, "UUID4Rv1", rb_cUUID4RCommon);
    rb_define_alloc_func(rb_cUUID4Rv1, uuid4r_alloc);
    rb_define_method(rb_cUUID4Rv1, "initialize", uuid4rv1_initialize, 0);
    /* ------ UUID4Rv3 ------ */
    rb_cUUID4Rv3 = rb_define_class_under(rb_cUUID4R, "UUID4Rv3", rb_cUUID4RCommon);
    rb_define_alloc_func(rb_cUUID4Rv3, uuid4r_alloc);
    rb_define_method(rb_cUUID4Rv3, "initialize", uuid4rv3_initialize, 2);
    /* ------ UUID4Rv4 ------ */
    rb_cUUID4Rv4 = rb_define_class_under(rb_cUUID4R, "UUID4Rv4", rb_cUUID4RCommon);
    rb_define_alloc_func(rb_cUUID4Rv4, uuid4r_alloc);
    rb_define_method(rb_cUUID4Rv4, "initialize", uuid4rv4_initialize, 0);
    /* ------ UUID4Rv5 ------ */
    rb_cUUID4Rv5 = rb_define_class_under(rb_cUUID4R, "UUID4Rv5", rb_cUUID4RCommon);
    rb_define_alloc_func(rb_cUUID4Rv5, uuid4r_alloc);
    rb_define_method(rb_cUUID4Rv5, "initialize", uuid4rv5_initialize, 2);
}

