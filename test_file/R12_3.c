#include "R12_3.h"

void f(int a, int b)
{
    int c = a+b;
}

void func(int p1, int p2)
{
    int a1,a2;
    f(1,0);
	f ( ( 1, 2 ), 3 ); /* Non-compliant - how many parameters? */
	f ( COMMA( 1, 2 ), 3 ); /* Non-compliant - how many parameters? */
	f ( 3, COMMA( 1, 2 ) ); /* Non-compliant - how many parameters? */
	//The following example is non-compliant with this rule and other rules:
	for ( a1 = 0,  a2 = 10; a1 < a2; ++a1, --a2 )
	{
	}
    a1 = COMMAOP;
}


