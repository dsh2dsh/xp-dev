// Copyright David Abrahams 2002. Permission to copy, use,
// modify, sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.

#if NOCOMPILE
1 = 1;
#endif 

void lib();

void f()
{
    lib();
}

#ifndef NOLINK
int main()
{
}
#endif
