#ifndef CULTIST_H
#define CULTIST_H

void hello_init_cultist(TSRMLS_D);

PHP_METHOD(Cultist, sacrifice);
PHP_METHOD(Cultist, createCultist);
PHP_METHOD(Cultist, __construct);

#endif
