#ifndef PHP_HELLO_H
#define PHP_HELLO_H 1
#define PHP_HELLO_WORLD_VERSION "1.0"
#define PHP_HELLO_WORLD_EXTNAME "hello"

PHP_FUNCTION(hello_world);
PHP_FUNCTION(hello_bool);
PHP_FUNCTION(mprint);
PHP_FUNCTION(makeLong);
PHP_FUNCTION(makeBool);
PHP_FUNCTION(getYig);

PHP_FUNCTION(eat);
PHP_FUNCTION(findName);
PHP_FUNCTION(cmpArray);

PHP_FUNCTION(makeObject);

PHP_MINIT_FUNCTION(hello);

void hello_old_one_pefree(zend_rsrc_list_entry *rsrc TSRMLS_DC);

typedef struct _old_one {
  char *name;
  int worshippers;
} old_one;

extern zend_module_entry hello_module_entry;
#define phpext_hello_ptr &hello_module_entry

#endif
