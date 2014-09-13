#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "php.h"
#include "php_hello.h"


int le_old_one; // "list entry" old_one

// Defines the functions we want to add
static zend_function_entry hello_functions[] = {
    PHP_FE(hello_world, NULL)
    PHP_FE(hello_bool, NULL)
    PHP_FE(mprint, NULL)
    PHP_FE(makeBool, NULL)
    PHP_FE(makeLong, NULL)
    PHP_FE(getYig, NULL)
    PHP_FE(eat, NULL)
    PHP_FE(findName, NULL)
    PHP_FE(cmpArray, NULL)
    {NULL, NULL, NULL}
};

zend_module_entry hello_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    PHP_HELLO_WORLD_EXTNAME,
    hello_functions,
    PHP_MINIT(hello),
    NULL,
    NULL,
    NULL,
    NULL,
#if ZEND_MODULE_API_NO >= 20010901
    PHP_HELLO_WORLD_VERSION,
#endif
    STANDARD_MODULE_PROPERTIES
};


// -------------------------------- Install the module
#ifdef COMPILE_DL_HELLO
ZEND_GET_MODULE(hello)
#endif

// -------------------------------- Persistent memory
PHP_MINIT_FUNCTION(hello)
{
    le_old_one = zend_register_list_destructors_ex(NULL, hello_old_one_pefree, "Great Old Ones", module_number);
}

// Destructor, called automatically module_type_pefree
void hello_old_one_pefree(zend_rsrc_list_entry* rsrc TSRMLS_DC)
{
    old_one *god = rsrc->ptr;

    if (god->name) {
        pefree(god->name, 1);
    }

    pefree(god, 1);
}

PHP_FUNCTION(getYig)
{
    zend_rsrc_list_entry *le;
    char *key = "yig";

    // If not found
    if (zend_hash_find(&EG(persistent_list), key, strlen(key)+1, (void**)&le) == SUCCESS) {
        old_one *god = le->ptr;

        php_printf("Fetched %s with %d worshippers\n", god->name, god->worshippers);
        god->worshippers--;

        if (god->worshippers < 0) {
            php_printf("Killed !\n");
            zend_hash_del(&EG(persistent_list), key, strlen(key)+1);
        }
    }

    if (zend_hash_find(&EG(persistent_list), key, strlen(key)+1, (void**)&le) == FAILURE) {
        zend_rsrc_list_entry nle;
        old_one *yig;

        yig = (old_one*)pemalloc(sizeof(old_one), 1);
        yig->name = pestrdup("Yig", 1);
        yig->worshippers = 4;

        php_printf("New god created\n");

        nle.ptr = yig;
        nle.type = le_old_one;
        nle.refcount = 1;

        zend_hash_update(&EG(persistent_list), key, strlen(key)+1, (void*)&nle, sizeof(zend_rsrc_list_entry), NULL);
    }
}

// -------------------------------- Arrays
/**
add_assoc_long(zval *zval_p, char *key, long n)
add_assoc_null(zval *zval_p, char *key)
add_assoc_bool(zval *zval_p, char *key, zend_bool b)
add_assoc_double(zval *zval_p, char *key, double d)
add_assoc_string(zval *zval_p, char *key, char *str, int duplicate)
add_assoc_stringl(zval *zval_p, char *key, char *str, int length, int duplicate)
add_assoc_zval(zval *zval_p, char *key, zval *value)

add_next_index_long(zval *zval_p, long n);
add_next_index_null(zval *zval_p);
add_next_index_bool(zval *zval_p, int b);
add_next_index_resource(zval *zval_p, int r);
add_next_index_double(zval *zval_p, double d);
add_next_index_string(zval *zval_p, const char *str, int duplicate);
add_next_index_stringl(zval *zval_, const char *str, uint length, int duplicate);
add_next_index_zval(zval *zval_p, zval *value);
**/
PHP_FUNCTION(eat)
{
    int str_len, i;
    char* str;
    long num;
    zval* arr;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sla", &str, &str_len, &num, &arr) == FAILURE) {
        return;
    }

    for (i=0; i<num; i++) {
        add_next_index_string(arr, str, 1);
    }
}

/**
int zend_hash_find(const HashTable *ht, const char *arKey, uint nKeyLength, void **pData);
int zend_hash_add(const HashTable *ht, const char *arKey, uint nKeyLength, void *pData, int pDataSize, void **pDest);
int zend_hash_add(const HashTable *ht, const char *arKey, uint nKeyLength, void *pData, int pDataSize, void **pDest);
int zend_hash_num_elements(const HashTable *ht);
int zend_hash_exists(const HashTable *ht, const char *arKey, uint nKeyLength);
**/
PHP_FUNCTION(findName)
{
    zval *arr, **eleFound;
    char* name;
    int name_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sa", &name, &name_len, &arr) == FAILURE) {
        return;
    }

    if (zend_hash_find(Z_ARRVAL_P(arr), name, name_len, (void**)&eleFound) == FAILURE) {
        return;
    }

    RETURN_STRINGL(Z_STRVAL_PP(eleFound), Z_STRLEN_PP(eleFound), 1);
}

/**
array_init
loop

Creates an array, loop on it, and returns a new array with each values doubled
**/
PHP_FUNCTION(cmpArray)
{
    zval *arr;
    MAKE_STD_ZVAL(arr);
    array_init(arr);

    array_init(return_value);

    char* value = estrdup("value");
    char* valuebis = estrdup("valuebis");

    add_next_index_string(arr, value, 0);
    add_next_index_string(arr, valuebis, 0);

    // Adds all elements twice
    HashTable *hindex = Z_ARRVAL_P(arr);
    HashPosition ptr;
    zval** data;

    for(zend_hash_internal_pointer_reset_ex(hindex, &ptr);
        zend_hash_get_current_data_ex(hindex, (void**)&data, &ptr) == SUCCESS;
        zend_hash_move_forward_ex(hindex, &ptr)) {

        char* key;
        ulong ikey;
        uint key_len, key_type;

        key_type = zend_hash_get_current_key_ex(hindex, &key, &key_len, &ikey, 0, &ptr);

        switch (key_type) {
            case HASH_KEY_IS_STRING:
                break;
            case HASH_KEY_IS_LONG:
                add_next_index_string(return_value, Z_STRVAL_PP(data), 0);
                add_next_index_string(return_value, Z_STRVAL_PP(data), 0);
                break;
            default:
                return;
        }
    }
}

// -------------------------------- Prints something
PHP_FUNCTION(mprint) {
    char* str;
    long num;
    int str_len;

    /**
    Gets parameters
    b => boolean  => zend_bool
    s => string   => char* | int
    l => long     => long
    d => double   => double
    a => array    => zval*
    o => object   => zval*
    z => any      => zval*
    r => resource => zval*
    */
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &num, &str, &str_len) == FAILURE) {
        return;
    }

    php_printf("Hello world ! I'm the best module ever %d => %s!\n", num, str);
}

// Returns string
PHP_FUNCTION(hello_world)
{
    RETURN_STRING("Hello World", 1);
}

// Returns bool
PHP_FUNCTION(hello_bool)
{
    RETURN_BOOL(1);
}

/**
typedef union _zvalue_value {
    long lval;
    double dval;
    struct {
        char *val;
        int len;
    } str;
    HashTable *ht;
    zend_object_value obj;
} zvalue_value;

return_value is passed into PHP_FUNCTION and holds values that is returned (default to null)
**/

/**
=> macros to get values (_p is for the number of pointers)
zval *zval_p;
=> eg Z_LVAL_P(zval)
zval zval
=> eg Z_LVAL(zval)
zval **zval
=> eg Z_VAL_PP(zval)


long l        = Z_LVAL_P(zval_p)
zend_bool b   = Z_BVAL_P(zval_p)
double d      = Z_DVAL_P(zval_p)
char *str     = Z_STRVAL_P(zval_p)
int str_len   = Z_STRLEN_P(zval_p)
HashTable *ht = Z_ARRVAL_P(zval_p)

// objects are a bit complicated... suffice to know that these exist:
Z_OBJPROP_P(zval_p)
Z_OBJCE_P(zval_p)
Z_OBJVAL_P(zval_p)
Z_OBJ_HANDLE_P(zval_p)
Z_OBJ_HT_P(zval_p)
Z_OBJ_HANDLER_P(zval_p, h)
Z_OBJDEBUG_P(zval_p,is_tmp)
**/

// Returns true
PHP_FUNCTION(makeBool) {
    Z_TYPE_P(return_value) = IS_BOOL;
    Z_LVAL_P(return_value) = 1;
}

// Returns 1
PHP_FUNCTION(makeLong) {
    Z_TYPE_P(return_value) = IS_LONG;
    Z_LVAL_P(return_value) = 1;
}

/**
ZVAL CREATION
ZVAL_type :
-----
ZVAL_NULL(zval_p);
ZVAL_BOOL(zval_p, 0);
ZVAL_LONG(zval_p, 123);
ZVAL_DOUBLE(zval_p, 12.3);
-----
----> strings
// "bar" is on the stack and zval_p is on the heap, so we
// want to make a copy of "bar" on the heap
ZVAL_STRING(zval_p, "bar", 1);
// this means "copy" ------^

// copy "bar" to the heap
char *str = estrdup("bar");
ZVAL_STRING(zval_p, str, 0);
// "don't copy" ---------^

zval *zval_p
MAKE_STD_ZVAL(zval_p)  // Creates zval
zval_ptr_dtor(&zval_p) // destroy a **zval (refcount --)
**/
