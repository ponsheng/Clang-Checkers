
void test(int a1, double a2, char a3, int a4[5], char a5[10]) {
    int v1 = sizeof a1;  // Compliant
    int v2 = sizeof(a2); // Compliant
    int v3 = sizeof(a3); // Compliant
    int v4 = sizeof(a4); // Non-compliant
    int v5 = sizeof a5; // Non-compliant
    int v6 = sizeof(int); // Compliant
    int v7 = sizeof(int [10]); // Compliant
/*
    int b2 =  sizeof C;
    int b3 = (int) sizeof(D);
    int b4 = (int) sizeof(b);
    int z[100];
    int b5 = (int) sizeof(z);
    int b7 = sizeof(int);
    int b6 = a;
*/
}
/*
void R7_2(int p1) {
    int a = 100;
    int b = 10u;
    int c = 1000U;
    long d = 0x0LL;
    int x = sizeof(p1);
}
*/

/*
int x = 0;
int y = 0;
x = y;
x= y= z;
if (x != 0)
{
}
bool b1 = x != y;
bool b2;
b2 = x != y;

if ( ( x=y) != 0)
{
    if (x=y)
    {}
}
*/
/*
int f(int xx) {
 int x = 0;
int y = 0;
int z = 0;
x = y;
x= y= z;
if (x != 0)
{
}
bool b1 = x != y;
bool b2;
b2 = x != y;

if ( ( x=y) != 0)
{
    if (x=y)
    {}
}
     int result = (x / 42);
        return result;
}*/
