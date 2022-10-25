/* pow() replacement for libmbus
 *
 * Based on fs_math.c by Pete Filandr <pfiland@mindspring.com>, taken
 * from https://github.com/addiva-elektronik/pd
 *
 * This is free and unencumbered software released into the public domain.
 */
#include <float.h>

double fs_sqrt(double x)
{
    double a, b;
    int n;

    if (x > 0 && DBL_MAX >= x) {
        for (n = 0; x > 2; x /= 4) {
            ++n;
        }

        while (0.5 > x) {
            --n;
            x *= 4;
        }

        a = x;
        b = (1 + x) / 2;
        do {
            x = b;
            b = (a / x + x) / 2;
        } while (x > b);

        while (n > 0) {
            x *= 2;
            --n;
        }

        while (0 > n) {
            x /= 2;
            ++n;
        }
    } else {
        if (x != 0) {
            x = DBL_MAX;
        }
    }

    return x;
}

double fs_log(double x)
{
    static int initialized = 0;
    static double A, B, C;
    double a, b, c;
    double epsilon;
    int n;

    if (x > 0 && DBL_MAX >= x) {
        if (!initialized) {
            initialized = 1;
            A = fs_sqrt(2);
            B = A / 2;
            C = fs_log(A);
        }

        for (n = 0; x > A; x /= 2) {
            ++n;
        }

        while (B > x) {
            --n;
            x *= 2;
        }

        a = (x - 1) / (x + 1);
        x = C * n + a;
        c = a * a;
        n = 1;
        epsilon = DBL_EPSILON * x;
        if (0 > a) {
            if (epsilon > 0) {
                epsilon = -epsilon;
            }

            do {
                n += 2;
                a *= c;
                b = a / n;
                x += b;
            } while (epsilon > b);
        } else {
            if (0 > epsilon) {
                epsilon = -epsilon;
            }

            do {
                n += 2;
                a *= c;
                b = a / n;
                x += b;
            } while (b > epsilon);
        }
        x *= 2;
    } else {
        x = -DBL_MAX;
    }

    return x;
}

double fs_exp(double x)
{
    static int initialized = 0;
    static double x_max, x_min;
    static double epsilon;
    unsigned n, square;
    double b, e;

    if (!initialized) {
        initialized = 1;
        x_max = fs_log(DBL_MAX);
        x_min = fs_log(DBL_MIN);
        epsilon = DBL_EPSILON / 4;
    }

    if (x_max >= x && x >= x_min) {
        for (square = 0; x > 1; x /= 2) {
            ++square;
        }

        while (-1 > x) {
            ++square;
            x /= 2;
        }

        e = b = n = 1;
        do {
            b /= n++;
            b *= x;
            e += b;
            b /= n++;
            b *= x;
            e += b;
        } while (b > epsilon);

        while (square-- != 0) {
            e *= e;
        }
    } else {
        e = x > 0 ? DBL_MAX : 0;
    }

    return e;
}

double fs_fmod(double x, double y)
{
    const double c = x;
    double a, b;

    if (0 > c) {
        x = -x;
    }

    if (0 > y) {
        y = -y;
    }

    if (y != 0 && DBL_MAX >= y && DBL_MAX >= x) {
        while (x >= y) {
            a = x / 2;
            b = y;

            while (a >= b) {
                b *= 2;
            }
            x -= b;
        }
    } else {
        x = 0;
    }

    return 0 > c ? -x : x;
}

double fs_pow(double x, double y)
{
    double p = 0;

    if (0 > x && fs_fmod(y, 1) == 0) {
        if (fs_fmod(y, 2) == 0) {
            p =  fs_exp(fs_log(-x) * y);
        } else {
            p = -fs_exp(fs_log(-x) * y);
        }
    } else {
        if (x != 0 || 0 >= y) {
            p =  fs_exp(fs_log( x) * y);
        }
    }

    return p;
}

#ifdef weak_alias
weak_alias (fs_pow, pow)
#endif
