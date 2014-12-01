#include "global.h"
#include "lib/printf/printf.h"

static const char printf_hexdigit[] = "0123456789ABCDEF";

int printf_format(int (*push)(void *userp, unsigned char data), void *userp, const char *fmt, va_list ap)
{
    char *str;
    char tmpbuf[12], pad;
    int ch, width, val, sign, precision;
    long lval, lsign;
    unsigned int uval;
    unsigned long ulval;
    bool ok = true;

    tmpbuf[sizeof tmpbuf - 1] = '\0';

    while ((ch = *fmt++) != '\0' && ok)
    {
        if (ch == '%')
        {
            ch = *fmt++;
            pad = ' ';
            if (ch == '0')
            pad = '0';

            width = 0;
            while (ch >= '0' && ch <= '9')
            {
                width = 10 * width + ch - '0';
                ch = *fmt++;
            }

            precision = 0;
            if (ch == '.')
            {
                ch = *fmt++;
                while (ch >= '0' && ch <= '9')
                {
                    precision = 10 * precision + ch - '0';
                    ch = *fmt++;
                }
            }
            else precision = 12;

            str = tmpbuf + sizeof tmpbuf - 1;
            switch (ch)
            {
            case 'c':
                *--str = va_arg(ap, int);
                break;

            case 's':
                str = va_arg(ap, char*);
                break;

            case 'd':
                val = sign = va_arg(ap, int);
                if (val < 0) val = -val;
                do
                {
                    *--str = (val % 10) + '0';
                    val /= 10;
                }
                while (val > 0);
                if (sign < 0) *--str = '-';
                break;

            case 'u':
                uval = va_arg(ap, unsigned int);
                do
                {
                    *--str = (uval % 10) + '0';
                    uval /= 10;
                }
                while (uval > 0);
                break;

            case 'x':
            case 'X':
                pad='0';
                uval = va_arg(ap, int);
                do
                {
                    *--str = printf_hexdigit[uval & 0xf];
                    uval >>= 4;
                }
                while (uval);
                break;

            case 'l':
            case 'z':
                ch = *fmt++;
                switch(ch)
                {
                case 'x':
                case 'X':
                    pad='0';
                    ulval = va_arg(ap, long);
                    do
                    {
                        *--str = printf_hexdigit[ulval & 0xf];
                        ulval >>= 4;
                    }
                    while (ulval);
                    break;
                case 'd':
                    lval = lsign = va_arg(ap, long);
                    if (lval < 0) lval = -lval;
                    do
                    {
                        *--str = (lval % 10) + '0';
                        lval /= 10;
                    }
                    while (lval > 0);
                    if (lsign < 0) *--str = '-';
                    break;

                case 'u':
                    ulval = va_arg(ap, unsigned long);
                    do
                    {
                        *--str = (ulval % 10) + '0';
                        ulval /= 10;
                    }
                    while (ulval > 0);
                    break;

                default:
                    *--str = 'l';
                    *--str = ch;
                    break;
                }
                break;

            default:
                *--str = ch;
                break;
            }

            if (width > 0)
            {
                const char* p = str;
                while (*p++) width--;
                while (width-- > 0 && ok) ok = push(userp, pad);
            }
            while (*str != '\0' && ok && precision--) ok = push(userp, *str++);
    }
    else
        ok = push(userp, ch);
    }
    return ok;
}
