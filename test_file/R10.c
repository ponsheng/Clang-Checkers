void f() {

    _Bool _bool;
    char _char;
    signed char signed_char;
        signed short signed_short;
        signed int signed_int;
        signed long signed_long;
        signed long long signed_long_long;
    unsigned char unsigned_char;
        unsigned short unsigned_short;
        unsigned int unsigned_int;
        unsigned long unsigned_long;
        unsigned long long unsigned_long_long;

        enum name { aa,bb,cc,dd,ee,ff} _enum;

        float _float;
            double _double;
            long double long_double;

    int array[10];
    int B;
#define ARRAY(idx) B = array[idx];
#define PLUS(var) B = (+ var );
#define MINUS(var) B = (- var );
#define OP(var) B = var + var ;
    OP(_bool)
    OP(_char)
    OP(_enum)
    OP(signed_char)
    OP(signed_short)
    OP(signed_int)
    OP(signed_long)
    OP(signed_long_long)
    OP(unsigned_char)
    OP(unsigned_short)
    OP(unsigned_int)
    OP(unsigned_long)
    OP(unsigned_long_long)
    OP(_float)
    OP(_double)
    OP(long_double)
}
