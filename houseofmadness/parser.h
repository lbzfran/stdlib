#ifndef PARSER_H
#define PARSER_H

inline int
CharIsSpace(char c)
{
    if (c == ' '  || c == '\t' || \
        c == '\n' || c == '\r') return 1;
    return 0;
}

inline int
CharIsAlpha(char c)
{
    if ((c >= 'A' && c <= 'Z') || \
        (c >= 'a' && c <= 'z')) return 1;
    return 0;
}

inline int
CharIsDigit(char c)
{
    if ( c >= '0' && c <= '9' ) return 1;
    return 0;
}

inline int
CharIsSymbol(char c)
{
    if ((c >= '!' && c <= '/') || \
        (c >= ':' && c <= '@') || \
        (c >= '[' && c <= '`') || \
        (c >= '{' && c <= '~')) return 1;
    return 0;
}

#endif //PARSER_H
