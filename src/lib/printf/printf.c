#include "global.h"
#include "lib/printf/printf.h"
#include "sys/util.h"


#define BYTES_TO_DIGITS(b) ((5 * (b) + 1) / 2)
#ifdef PRINTF_SUPPORT_LONGLONG
#define BUFSIZE BYTES_TO_DIGITS(sizeof(long long int)) + 2
#else
#define BUFSIZE BYTES_TO_DIGITS(sizeof(int)) + 2
#endif


static const char printf_hexdigit[] = "0123456789ABCDEF";


int printf_format(int (*push)(void *userp, unsigned char data), void *userp, const char *fmt, va_list args)
{
    int printed = 0;
    bool parsing = false;
    bool align_left = false;
    bool zero_pad = false;
    char lowercase;
    unsigned int param = 0;
    unsigned int width = 0;
    unsigned int limit = 0;
    int8_t fixedpoint;
    bool have_param = false;
    bool have_precision = false;
#ifdef PRINTF_ENABLE_PRECISION
    bool pad_precision = false;
#endif
    bool is_long = false;
#ifdef PRINTF_SUPPORT_LONGLONG
    bool is_longlong = false;
#endif
    bool is_negative;
    bool is_signed;
    char sign = 0;
    const char* begin = fmt;
    char* ptr;
    char c;
    int radix;
    union
    {
        int i;
#ifdef PRINTF_SUPPORT_LONGLONG
        long long int lli;
#endif
    } number;
    char buf[BUFSIZE];

    while ((c = *fmt++))
    {
        if (!parsing)
        {
            if (c == '%')
            {
                parsing = true;
                align_left = false;
                zero_pad = false;
                param = 0;
                width = 0;
                limit = (unsigned)-1;
                have_param = false;
#if defined(PRINTF_ENABLE_PRECISION) || defined(PRINTF_ENABLE_FIXEDPOINT)
                have_precision = false;
#endif
#ifdef PRINTF_ENABLE_PRECISION
                pad_precision = false;
#endif
                is_long = false;
#ifdef PRINTF_SUPPORT_LONGLONG
                is_longlong = false;
#endif
                is_signed = false;
                sign = 0;
                begin = fmt - 1;
            }
            else
            {
print_char:
                if (push(userp, c)) printed++;
                else return printed;
                parsing = false;
            }
        }
        else
            switch (c)
            {
            case '%':  // literal '%'
                goto print_char;

            case '+':  // explicit + sign flag
                sign = '+';
                break;

            case ' ':  // sign padding flag
                if (sign != '+') sign = ' ';
                break;

            case '-':  // left align flag
                align_left = true;
                break;

#if defined(PRINTF_ENABLE_PRECISION) || defined(PRINTF_ENABLE_FIXEDPOINT)
            case '.':  // width/precision delimiter
                have_precision = true;
                if (have_param)
                {
                    have_param = false;
                    width = param;
                    param = 0;
                }
                break;
#endif

            case '*':  // param supplied as arg flag
            {
                have_param = true;
                int val = va_arg(args, int);
                if (val >= 0) param = val;
#if defined(PRINTF_ENABLE_PRECISION) || defined(PRINTF_ENABLE_FIXEDPOINT)
                else if (have_precision);
#endif
                else
                {
                    align_left = true;
                    param = -val;
                }
                break;
            }

            case '0':  // could be a zero padding flag or part of a param
                if (!have_param && !have_precision && !align_left)
                {
                    zero_pad = true;
                    break;
                }
                // fallthrough for the param case

            case '1' ... '9':
                have_param = true;
                param = param * 10 + c - '0';
                break;

            case 'c':  // token for char
                c = va_arg(args, int);
                if (!have_param && !have_precision) goto print_char;
                ptr = buf;
                buf[0] = c;
                buf[1] = 0;
                goto print_string;

            case 's':  // token for ASCIIZ string
                ptr = va_arg(args, char*);
                if (!ptr) ptr = "(null)";
#ifdef PRINTF_ENABLE_PRECISION
              if (have_precision) limit = param;
              else
#endif
                  if (have_param) width = param;
print_string:
              parsing = false;
              if (!align_left)
              {
                  if (width) width -= !!sign;
                  unsigned int len = strlen(ptr);
                  char pad_char = ' ';
#ifdef PRINTF_ENABLE_PRECISION
                  if (pad_precision)
                  {
                      if (param <= width) width -= param;
                      else width = 0;
                  }
                  if (limit < len) len = limit;
#endif
                  if (zero_pad)
                  {
                      pad_char = '0';
                      if (sign)
                      {
                          if (push(userp, sign)) printed++;
                          else return printed;
                          sign = 0;
                      }
                  }
                  while (len++ < width)
                  {
                      if (push(userp, pad_char)) printed++;
                      else return printed;
                  }
              }
              width += printed;
              if (sign)
              {
                  if (push(userp, sign)) printed++;
                  else return printed;
              }
#ifdef PRINTF_ENABLE_PRECISION
              if (pad_precision)
                  while (param--)
                  {
                      if (push(userp, '0')) printed++;
                      else return printed;
                  }
#endif
              while (*ptr && limit--)
              {
                  if (push(userp, *ptr++)) printed++;
                  else return printed;
              }
              if (align_left)
                  while (printed < width)
                  {
                      if (push(userp, ' ')) printed++;
                      else return printed;
                  }
              break;

        case 'l':  // long flag
#ifdef PRINTF_SUPPORT_LONGLONG
            if (is_longlong) goto bad_fmt;
            else if (is_long)
            {
                is_longlong = true;
                is_long = false;
            }
            else
#endif
            if (is_long) goto bad_fmt;
            is_long = true;
            break;

        case 'X':  // token for hexadecimal integer
        case 'x':
            radix = 16;
            goto handle_number;

#ifdef PRINTF_ENABLE_FIXEDPOINT
        case 'D':  // token for signed decimal fixed point number
#endif
        case 'd':  // token for signed decimal integer
        case 'i':
            is_signed = true;
            // fallthrough

#ifdef PRINTF_ENABLE_FIXEDPOINT
        case 'U':  // token for unsigned decimal fixed point number
#endif
        case 'u':  // token for unsigned decimal integer
            radix = 10;
handle_number:
            lowercase = c & 0x20;
#ifdef PRINTF_SUPPORT_LONGLONG
            if (is_longlong)
            {
                number.lli = va_arg(args, long long int);
                is_negative = number.lli < 0;
            }
            else
#endif
            {
                number.i = va_arg(args, int);
                is_negative = number.i < 0;
            }
            if (radix != 10) sign = 0;
            else if (is_signed && is_negative)
            {
                sign = '-';
#ifdef PRINTF_SUPPORT_LONGLONG
                if (is_longlong) number.lli = -number.lli;
                else
#endif
                number.i = -number.i;
            }
#ifdef PRINTF_ENABLE_FIXEDPOINT
            fixedpoint = (radix == 10 && !lowercase && param) ? param + 1 : 0;
#else
            fixedpoint = 0;
#endif
            ptr = buf + sizeof(buf);
            *--ptr = 0;
#ifdef PRINTF_SUPPORT_LONGLONG
            if (is_longlong)
                do
                {
#ifdef PRINTF_ENABLE_FIXEDPOINT
                    if (!--fixedpoint) *--ptr = '.';
#endif
                    *--ptr = printf_hexdigit[(unsigned)number.lli % radix] | lowercase;
                    number.lli = ((unsigned)number.lli) / radix;
                }
                while (number.lli || fixedpoint > 0);
            else
#endif
                do
                {
#ifdef PRINTF_ENABLE_FIXEDPOINT
                    if (!--fixedpoint) *--ptr = '.';
#endif
                    *--ptr = printf_hexdigit[(unsigned)number.i % radix] | lowercase;
                    number.i = ((unsigned)number.i) / radix;
                }
                while (number.i || fixedpoint > 0);
#ifdef PRINTF_ENABLE_PRECISION
            if (have_precision)
            {
                int len = buffer + sizeof(buffer) - ptr - 1;
                if (len < param)
                {
                    pad_precision = true;
                    param -= len;
                }
            }
            else
#endif
                if (have_param) width = param;
            goto print_string;

        default:
bad_fmt:
            parsing = false;
            while (begin < fmt)
                if (push(userp, *begin++)) printed++;
                else return printed;
            break;
        }
    }
    return printed;
}
