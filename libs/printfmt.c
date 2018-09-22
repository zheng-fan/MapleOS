#include <defs.h>
#include <stdio.h>
#include <string.h>
#include <x86.h>

/* *
 * printnum - print a number (base <= 16) in reverse order
 * @putch:        specified putch function, print a single character
 * @putdat:        used by @putch function
 * @num:        the number will be printed
 * @base:        base for print, must be in [1, 16]
 * @width:         maximum number of digits, if the actual width is less than @width, use @padc instead
 * @padc:        character that padded on the left if the actual width is less than @width
 * */
static void
printnum(void (*putch)(int, void *), void *putdat,
         unsigned long long num, unsigned base, int width, int padc) {
    unsigned long long result = num;
    //long long的乘除法需要用到编译器的一些库，因此需要手动编写汇编代码。但由于我们是64位的，可以直接用long
    unsigned mod = (unsigned long)result % base;
    result = (unsigned long)result / base;

    // first recursively print all preceding (more significant) digits
    if (num >= base) {
        printnum(putch, putdat, result, base, width - 1, padc);
    } else {
        // print any needed pad characters before first digit
        while (--width > 0)
            putch(padc, putdat);
    }
    // then print this (the least significant) digit
    putch("0123456789abcdef"[mod], putdat);
}

/* *
 * getuint - get an unsigned int of various possible sizes from a varargs list
 * @ap:            a varargs list pointer
 * @lflag:        determines the size of the vararg that @ap points to
 * */
static unsigned long long
getuint(va_list *ap, int lflag) {
    if (lflag >= 2) {
        return va_arg(*ap, unsigned long long);
    } else if (lflag) {
        return va_arg(*ap, unsigned long);
    } else {
        return va_arg(*ap, unsigned int);
    }
}

/* *
 * getint - same as getuint but signed, we can't use getuint because of sign extension
 * @ap:            a varargs list pointer
 * @lflag:        determines the size of the vararg that @ap points to
 * */
// AMD64的va_list实现有所变化，从指针变成了数组，因此直接传参即可，无需传地址
static long long
getint(va_list ap, int lflag) {
    if (lflag >= 2) {
        return va_arg(ap, long long);
    } else if (lflag) {
        return va_arg(ap, long);
    } else {
        return va_arg(ap, int);
    }
}

/* *
 * printfmt - format a string and print it by using putch
 * @putch:        specified putch function, print a single character
 * @putdat:        used by @putch function
 * @fmt:        the format string to use
 * */
void printfmt(void (*putch)(int, void *), void *putdat, const char *fmt, ...) {
    va_list ap;

    va_start(ap, fmt);
    vprintfmt(putch, putdat, fmt, ap);
    va_end(ap);
}

/* *
 * vprintfmt - format a string and print it by using putch, it's called with a va_list
 * instead of a variable number of arguments
 * @putch:        specified putch function, print a single character
 * @putdat:        used by @putch function
 * @fmt:        the format string to use
 * @ap:            arguments for the format string
 *
 * Call this function if you are already dealing with a va_list.
 * Or you probably want printfmt() instead.
 * */
void vprintfmt(void (*putch)(int, void *), void *putdat, const char *fmt, va_list ap) {
    register const char *p;
    register int ch, err;
    unsigned long long num;
    int base, width, precision, lflag, altflag;

    while (1) {
        while ((ch = *(unsigned char *)fmt++) != '%') {
            if (ch == '\0') {
                return;
            }
            putch(ch, putdat);
        }

        // Process a %-escape sequence
        char padc = ' ';
        width = precision = -1;
        lflag = altflag = 0;

    reswitch:
        switch (ch = *(unsigned char *)fmt++) {

        // flag to pad on the right
        case '-':
            padc = '-';
            goto reswitch;

        // flag to pad with 0's instead of spaces
        case '0':
            padc = '0';
            goto reswitch;

        // width field
        case '1' ... '9':
            for (precision = 0;; ++fmt) {
                precision = precision * 10 + ch - '0';
                ch = *fmt;
                if (ch < '0' || ch > '9') {
                    break;
                }
            }
            goto process_precision;

        case '*':
            precision = va_arg(ap, int);
            goto process_precision;

        case '.':
            if (width < 0)
                width = 0;
            goto reswitch;

        case '#':
            altflag = 1;
            goto reswitch;

        process_precision:
            if (width < 0)
                width = precision, precision = -1;
            goto reswitch;

        // long flag (doubled for long long)
        case 'l':
            lflag++;
            goto reswitch;

        // character
        case 'c':
            putch(va_arg(ap, int), putdat);
            break;

        // string
        case 's':
            if ((p = va_arg(ap, char *)) == NULL) {
                p = "(null)";
            }
            if (width > 0 && padc != '-') {
                for (width -= strnlen(p, precision); width > 0; width--) {
                    putch(padc, putdat);
                }
            }
            for (; (ch = *p++) != '\0' && (precision < 0 || --precision >= 0); width--) {
                if (altflag && (ch < ' ' || ch > '~')) {
                    putch('?', putdat);
                } else {
                    putch(ch, putdat);
                }
            }
            for (; width > 0; width--) {
                putch(' ', putdat);
            }
            break;

        // (signed) decimal
        case 'd':
            num = getint(ap, lflag);
            if ((long long)num < 0) {
                putch('-', putdat);
                num = -(long long)num;
            }
            base = 10;
            goto number;

        // unsigned decimal
        case 'u':
            num = getuint(ap, lflag);
            base = 10;
            goto number;

        // (unsigned) octal
        case 'o':
            num = getuint(ap, lflag);
            base = 8;
            goto number;

        // pointer
        case 'p':
            putch('0', putdat);
            putch('x', putdat);
            num = (unsigned long long)(ulongptr_t)va_arg(ap, void *);
            base = 16;
            goto number;

        // (unsigned) hexadecimal
        case 'x':
            num = getuint(ap, lflag);
            base = 16;
        number:
            printnum(putch, putdat, num, base, width, padc);
            break;

        // escaped '%' character
        case '%':
            putch(ch, putdat);
            break;

        // unrecognized escape sequence - just print it literally
        default:
            putch('%', putdat);
            for (fmt--; fmt[-1] != '%'; fmt--)
                /* do nothing */;
            break;
        }
    }
}
