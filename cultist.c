#include <php.h>
#include "cultist.h"

// Class entry
zend_class_entry *hello_ce_cultist;

static zend_function_entry cultist_methods[] = {
    PHP_ME(Cultist, sacrifice, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Cultist, createCultist, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
    PHP_ME(Cultist, __construct, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    {NULL, NULL, NULL}
};

/**
ZEND_ACC_STATIC
ZEND_ACC_PUBLIC
ZEND_ACC_PROTECTED
ZEND_ACC_PRIVATE
ZEND_ACC_CTOR
ZEND_ACC_DTOR
ZEND_ACC_DEPRECATED eg ZEND_ACC_PUBLIC|ZEND_ACC_DEPRECATED
**/
void hello_init_cultist(TSRMLS_D) {
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "Cultist", cultist_methods);
    hello_ce_cultist = zend_register_internal_class(&ce TSRMLS_CC);

    // Fields
    zend_declare_property_bool(hello_ce_cultist, "alive", strlen("alive"), 1, ZEND_ACC_PUBLIC TSRMLS_CC);
}

PHP_METHOD(Cultist, sacrifice) {
    php_printf("Sacrifice !");
}

PHP_METHOD(Cultist, createCultist) {
    object_init_ex(return_value, hello_ce_cultist);
}

PHP_METHOD(Cultist, __construct) {
    php_printf("New !");
}

