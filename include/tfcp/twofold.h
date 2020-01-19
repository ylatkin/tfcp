//======================================================================
// 2014-2015, 2019-2020 (c) Evgeny Latkin
// License: Apache 2.0 (http://www.apache.org/licenses/)
//======================================================================

#ifndef TWOFOLD_H
#define TWOFOLD_H
//======================================================================
//
//  C++ interface to TFCP library
//
//  - Defines `twofold` and `coupled` types
//  - Arithmetic functions like z = tadd(x, y)
//  - Math functions like sqrt(x), isnan(x), ...
//  - Operators over twofolds: -x, x + y, x < y, ...
//  - Converting twofold to string, and printing
//
//======================================================================

#include <iostream>
#include <string>
#include <type_traits>

#include <cmath>

namespace tfcp {

    // Assume T is scalar: float, double
    template<typename T> struct twofold;
    template<typename T> struct coupled;

    //------------------------------------------------------------------
    //
    // Define alias types:
    // - twofold: tfloat, tdouble
    // - coupled: pfloat, pdouble
    //
    // Twofold aliases degrade to standard float/double if Release build
    //
    //------------------------------------------------------------------

    using pfloat  = coupled<float>;
    using pdouble = coupled<double>;

#ifndef NDEBUG
    //
    // Debug: use twofolds to debug rounding errors
    //
    using tfloat  = twofold<float>;
    using tdouble = twofold<double>;
#else
    //
    // Release: twofold degrades to standard
    //
    using tfloat  = float;
    using tdouble = double;
#endif

    //------------------------------------------------------------------
    //
    //  Base class for twofold and coupled
    //
    //------------------------------------------------------------------

    // Assume T, S is float or double
    template<typename T> struct shaped {
        T value, error;

    public:
        shaped()                              { check_type(); }  // default undefined
        shaped(T v, T e) : value(v), error(e) { check_type(); }

    protected:
        void init(const shaped<T>& x) {
            value = x.value;
            error = x.error;
        }

    private:
        void check_type() {
            static_assert(std::is_same<T, float>::value ||
                          std::is_same<T, double>::value,
                          "unsupported base type");
        }
    };

    //------------------------------------------------------------------
    //
    //  Shape-agnostic access to value and error of twofold or coupled
    //
    //   Use like, e.g.:
    //     twofold<T> result;
    //     assert(|error_of(result)| <= threshold);
    //
    //   If NDEBUG, error_of(result) degrades to 0
    //
    //------------------------------------------------------------------

    // Access to components of twofold or coupled
    inline double value_of(const shaped<double>& x) { return x.value; }
    inline double error_of(const shaped<double>& x) { return x.error; }

    inline float value_of(const shaped<float>& x) { return x.value; }
    inline float error_of(const shaped<float>& x) { return x.error; }

    // If dotted x, e.g. if pdouble with NDEBUG
    inline double value_of(double x) { return x; }
    inline double error_of(double x) { return 0; }

    inline float value_of(float x) { return x; }
    inline float error_of(float x) { return 0; }

    //------------------------------------------------------------------
    //
    //  Arithmetic functions: tadd, tsub, tmul, tdiv, tsqrt, renormalize
    //
    //------------------------------------------------------------------

#define TFCP_RENORM(T) \
    shaped<T> renormalize(const shaped<T>& x); \
    shaped<T> fast_renorm(const shaped<T>& x); \
    inline T renormalize(T x) { return x; } \
    inline T fast_renorm(T x) { return x; }
    TFCP_RENORM(double);
    TFCP_RENORM(float);
#undef TFCP_RENORM

#define TFCP_SQRT(PREFIX, T) \
    shaped<T> PREFIX ## sqrt(const shaped<T>& x); \
    shaped<T> PREFIX ## sqrt(             T   x);
    TFCP_SQRT(t, double);
    TFCP_SQRT(p, double);
    TFCP_SQRT(t, float);
    TFCP_SQRT(p, float);
#undef TFCP_SQRT

#define TFCP_ARITHM_BASE(OP, PREFIX, T) \
    shaped<T> PREFIX ## OP(const shaped<T>& x, const shaped<T>& y); \
    shaped<T> PREFIX ## OP(const shaped<T>& x,              T   y); \
    shaped<T> PREFIX ## OP(             T   x, const shaped<T>& y); \
    shaped<T> PREFIX ## OP(             T   x,              T   y);
    TFCP_ARITHM_BASE(add, t, double);
    TFCP_ARITHM_BASE(sub, t, double);
    TFCP_ARITHM_BASE(mul, t, double);
    TFCP_ARITHM_BASE(div, t, double);
    TFCP_ARITHM_BASE(add, p, double);
    TFCP_ARITHM_BASE(sub, p, double);
    TFCP_ARITHM_BASE(mul, p, double);
    TFCP_ARITHM_BASE(div, p, double);
    TFCP_ARITHM_BASE(add, t, float);
    TFCP_ARITHM_BASE(sub, t, float);
    TFCP_ARITHM_BASE(mul, t, float);
    TFCP_ARITHM_BASE(div, t, float);
    TFCP_ARITHM_BASE(add, p, float);
    TFCP_ARITHM_BASE(sub, p, float);
    TFCP_ARITHM_BASE(mul, p, float);
    TFCP_ARITHM_BASE(div, p, float);
#undef TFCP_ARITHM_BASE

    //------------------------------------------------------------------
    //
    //  Type-and-shape conversions
    //
    //------------------------------------------------------------------

    //
    // Naming legend: d = dotted, t = twofold, p = coupled
    //
    // Types: D (destination), S (source) - float, double, or integral
    //

    // shape and type conversion: convert dotted value into...
//  template<typename D, typename S> inline        D  dbyd(S x);
    template<typename D, typename S> inline shaped<D> tbyd(S x);
    template<typename D, typename S> inline shaped<D> pbyd(S x);

    // shape and type conversion: convert assuming x is twofold...
    template<typename D, typename S> inline        D  dbyt(const shaped<S>& x);
    template<typename D, typename S> inline shaped<D> tbyt(const shaped<S>& x);
    template<typename D, typename S> inline shaped<D> pbyt(const shaped<S>& x);

    // shape and type conversion: convert assuming x is coupled...
    template<typename D, typename S> inline        D  dbyp(const shaped<S>& x);
    template<typename D, typename S> inline shaped<D> tbyp(const shaped<S>& x);
    template<typename D, typename S> inline shaped<D> pbyp(const shaped<S>& x);

    //------------------------------------------------------------------
    //
    //  Type-and-shape conversions: implementation
    //
    //------------------------------------------------------------------

    //
    // Get dotted by twofold/coupled
    //

    template<> inline double dbyt<double, double>(const shaped<double>& x) { return x.value; }
    template<> inline double dbyt<double, float >(const shaped<float> & x) { return x.value; }  // expand
    template<> inline float  dbyt<float , float >(const shaped<float> & x) { return x.value; }
    template<> inline float  dbyt<float , double>(const shaped<double>& x) { return x.value; }  // round

    template<> inline float  dbyp<float , double>(const shaped<double>& x) { return x.value; }  // round
    template<> inline float  dbyp<float , float >(const shaped<float> & x) { return x.value; }
    template<> inline double dbyp<double, double>(const shaped<double>& x) { return x.value; }
    template<> inline double dbyp<double, float> (const shaped<float> & x) {
        return static_cast<double>(x.value) + static_cast<double>(x.error);                     // expand
    }

    //
    // Get twofold/coupled by dotted
    //

    template<> inline shaped<double> pbyd<double, double>(double x) { return shaped<double>(x, 0); }
    template<> inline shaped<double> pbyd<double, float> (float  x) { return shaped<double>(x, 0); }
    template<> inline shaped<float>  pbyd<float,  float> (float  x) { return shaped<float >(x, 0); }
    template<> inline shaped<float>  pbyd<float,  double>(double x) {
        float value = x;          // round to nearest-even
        float error = x - value;  // exact, if enough bits
        return shaped<float>(value, error);
    }

    template<> inline shaped<double> tbyd<double, double>(double x) { return pbyd<double, double>(x); }
    template<> inline shaped<double> tbyd<double, float> (float  x) { return pbyd<double, float >(x); }
    template<> inline shaped<float>  tbyd<float,  double>(double x) { return pbyd<float , double>(x); }
    template<> inline shaped<float>  tbyd<float,  float> (float  x) { return pbyd<float , float >(x); }

    //
    // Get twofold by twofold/coupled
    //

    template<> inline shaped<double> tbyt(const shaped<double>& x) { return x; }
    template<> inline shaped<float>  tbyt(const shaped<float> & x) { return x; }
    template<> inline shaped<double> tbyt(const shaped<float> & x) {
        return shaped<double>(x.value, x.error);  // expand
    }
    template<> inline shaped<float>  tbyt(const shaped<double>& x) {
        float value  = x.value;          // round to nearest-even
        float error  = x.value - value;  // exact, if enough bits
              error += x.error;
        return shaped<float>(value, error);
    }

    template<> inline shaped<double> tbyp(const shaped<double>& x) { return x; }
    template<> inline shaped<float>  tbyp(const shaped<float> & x) { return x; }
    template<> inline shaped<float>  tbyp(const shaped<double>& x) { return tbyt<float, double>(x); }
    template<> inline shaped<double> tbyp(const shaped<float> & x) {
        shaped<double> t(x.value, x.error);  // expand
        return fast_renorm(t);
    }

    //
    // Get coupled by twofold/coupled
    //

    template<> inline shaped<double> pbyp(const shaped<double>& x) { return x; }
    template<> inline shaped<float>  pbyp(const shaped<float> & x) { return x; }
    template<> inline shaped<double> pbyp(const shaped<float> & x) {
        shaped<double> t(x.value, x.error);  // expand
        return fast_renorm(t);
    }
    template<> inline shaped<float>  pbyp(const shaped<double>& x) {
        float value  = x.value;          // round to nearest-even
        float error  = x.value - value;  // exact, if enough bits
              error += x.error;
        shaped<float> t(value, error);
        return fast_renorm(t);
    }

    template<> inline shaped<double> pbyt(const shaped<double>& x) { return renormalize(x); }
    template<> inline shaped<float>  pbyt(const shaped<float> & x) { return renormalize(x); }
    template<> inline shaped<double> pbyt(const shaped<float> & x) {
        shaped<double> t(x.value, x.error);  // expand
        return renormalize(t);
    }
    template<> inline shaped<float>  pbyt(const shaped<double>& x) {
        shaped<double> t = renormalize(x);
        return pbyp<float, double>(t);
    }

    //------------------------------------------------------------------
    //
    // Define twofold/coupled structures
    //
    //------------------------------------------------------------------

    //
    // Assume T is float or double
    // Assume S is float or double
    // Either S is integral if dotted param, i.e. if cast to/from S
    //

    template<typename T> struct twofold: public shaped<T> {
    public:
        template<typename S> twofold(              S   x) { init(tbyd<T, S>(x)); }
        template<typename S> twofold(const twofold<S>& x) { init(tbyt<T, S>(x)); }
        template<typename S> twofold(const coupled<S>& x) { init(tbyp<T, S>(x)); }
    public:
        template<typename S> operator         S () { return dbyt<S, T>(*this); }
        template<typename S> operator twofold<S>() { return tbyt<S, T>(*this); }
        template<typename S> operator coupled<S>() { return pbyt<S, T>(*this); }
    public:
        template<typename S> twofold& operator += (S x);
        template<typename S> twofold& operator -= (S x);
        template<typename S> twofold& operator *= (S x);
        template<typename S> twofold& operator /= (S x);
    public:
        template<typename S> twofold& operator += (const twofold<S>& x);
        template<typename S> twofold& operator -= (const twofold<S>& x);
        template<typename S> twofold& operator *= (const twofold<S>& x);
        template<typename S> twofold& operator /= (const twofold<S>& x);
    public:
        template<typename S> twofold& operator += (const coupled<S>& x);
        template<typename S> twofold& operator -= (const coupled<S>& x);
        template<typename S> twofold& operator *= (const coupled<S>& x);
        template<typename S> twofold& operator /= (const coupled<S>& x);
    };

    template<typename T> struct coupled: public shaped<T> {
    public:
        template<typename S> coupled(              S   x) { init(pbyd<T, S>(x));}
        template<typename S> coupled(const twofold<S>& x) { init(pbyt<T, S>(x));}
        template<typename S> coupled(const coupled<S>& x) { init(pbyp<T, S>(x));}
    public:
        template<typename S> operator         S () { return dbyp<S, T>(*this); }
        template<typename S> operator twofold<S>() { return tbyp<S, T>(*this); }
        template<typename S> operator coupled<S>() { return pbyp<S, T>(*this); }
    public:
        template<typename S> coupled& operator += (S x);
        template<typename S> coupled& operator -= (S x);
        template<typename S> coupled& operator *= (S x);
        template<typename S> coupled& operator /= (S x);
    public:
        template<typename S> coupled& operator += (const twofold<S>& x);
        template<typename S> coupled& operator -= (const twofold<S>& x);
        template<typename S> coupled& operator *= (const twofold<S>& x);
        template<typename S> coupled& operator /= (const twofold<S>& x);
    public:
        template<typename S> coupled& operator += (const coupled<S>& x);
        template<typename S> coupled& operator -= (const coupled<S>& x);
        template<typename S> coupled& operator *= (const coupled<S>& x);
        template<typename S> coupled& operator /= (const coupled<S>& x);
    };

    //------------------------------------------------------------------
    //
    //  Arithmetic operations: +, -, *, /, sqrt
    //
    //------------------------------------------------------------------

    inline twofold<double> sqrt(const twofold<double>& x) { return tsqrt(x); }
    inline coupled<double> sqrt(const coupled<double>& x) { return psqrt(x); }
    inline twofold<float>  sqrt(const twofold<float> & x) { return tsqrt(x); }
    inline coupled<float>  sqrt(const coupled<float> & x) { return psqrt(x); }
    inline double sqrt(double x) { return std::sqrt(x); }
    inline float  sqrt(float  x) { return std::sqrt(x); }

#define TFCP_ARITHM_SAME_TYPE(OP, F, PREFIX, SHAPE, T) \
    inline SHAPE<T> operator OP(const SHAPE<T>& x, const SHAPE<T>& y) { return PREFIX ## F(x, y); } \
    inline SHAPE<T> operator OP(const SHAPE<T>& x,             T   y) { return PREFIX ## F(x, y); } \
    inline SHAPE<T> operator OP(            T   x, const SHAPE<T>& y) { return PREFIX ## F(x, y); }
    TFCP_ARITHM_SAME_TYPE(+, add, t, twofold, double);
    TFCP_ARITHM_SAME_TYPE(-, sub, t, twofold, double);
    TFCP_ARITHM_SAME_TYPE(*, mul, t, twofold, double);
    TFCP_ARITHM_SAME_TYPE(/, div, t, twofold, double);
    TFCP_ARITHM_SAME_TYPE(+, add, p, coupled, double);
    TFCP_ARITHM_SAME_TYPE(-, sub, p, coupled, double);
    TFCP_ARITHM_SAME_TYPE(*, mul, p, coupled, double);
    TFCP_ARITHM_SAME_TYPE(/, div, p, coupled, double);
    TFCP_ARITHM_SAME_TYPE(+, add, t, twofold, float);
    TFCP_ARITHM_SAME_TYPE(-, sub, t, twofold, float);
    TFCP_ARITHM_SAME_TYPE(*, mul, t, twofold, float);
    TFCP_ARITHM_SAME_TYPE(/, div, t, twofold, float);
    TFCP_ARITHM_SAME_TYPE(+, add, p, coupled, float);
    TFCP_ARITHM_SAME_TYPE(-, sub, p, coupled, float);
    TFCP_ARITHM_SAME_TYPE(*, mul, p, coupled, float);
    TFCP_ARITHM_SAME_TYPE(/, div, p, coupled, float);
#undef TFCP_ARITHM_SAME_TYPE

    // Cast basic type to higher precision
#define TFCP_ARITHM_CROSS_TYPE_TWOFOLD(OP) \
    inline twofold<double> operator OP(const twofold<float>& x, const twofold<double>& y) { return     twofold<double>(x) OP y; } \
    inline twofold<double> operator OP(              float   x, const twofold<double>& y) { return static_cast<double>(x) OP y; } \
    inline twofold<double> operator OP(const twofold<float>& x,               double   y) { return     twofold<double>(x) OP y; } \
    inline twofold<double> operator OP(const twofold<double>& x, const twofold<float>& y) { return x OP     twofold<double>(y); } \
    inline twofold<double> operator OP(              double   x, const twofold<float>& y) { return x OP     twofold<double>(y); } \
    inline twofold<double> operator OP(const twofold<double>& x,               float   y) { return x OP static_cast<double>(y); }
    TFCP_ARITHM_CROSS_TYPE_TWOFOLD(+);
    TFCP_ARITHM_CROSS_TYPE_TWOFOLD(-);
    TFCP_ARITHM_CROSS_TYPE_TWOFOLD(*);
    TFCP_ARITHM_CROSS_TYPE_TWOFOLD(/);
#undef TFCP_ARITHM_CROSS_TYPE_TWOFOLD

    // Cast basic type to higher precision,
    // except cast coupled<float> to double
#define TFCP_ARITHM_CROSS_TYPE_COUPLED(OP) \
    inline coupled<double> operator OP(const coupled<float>& x, const coupled<double>& y) { return dbyp<double, float>(x) OP y; } \
    inline coupled<double> operator OP(const         float   x, const coupled<double>& y) { return static_cast<double>(x) OP y; } \
    inline         double  operator OP(const coupled<float>& x, const         double   y) { return dbyp<double, float>(x) OP y; } \
    inline coupled<double> operator OP(const coupled<double>& x, const coupled<float>& y) { return x OP dbyp<double, float>(y); } \
    inline coupled<double> operator OP(              double   x, const coupled<float>& y) { return x OP dbyp<double, float>(y); } \
    inline coupled<double> operator OP(const coupled<double>& x,               float   y) { return x OP static_cast<double>(y); }
    TFCP_ARITHM_CROSS_TYPE_COUPLED(+);
    TFCP_ARITHM_CROSS_TYPE_COUPLED(-);
    TFCP_ARITHM_CROSS_TYPE_COUPLED(*);
    TFCP_ARITHM_CROSS_TYPE_COUPLED(/);
#undef TFCP_ARITHM_CROSS_TYPE_COUPLED

    // Consider coupled is higher precision than twofold
    // Therefore, cast to coupled if coupled + twofold
    // NB: twofold loses error estimate in such cast
#define TFCP_ARITHM_CROSS_SHAPE(OP, T) \
    inline coupled<T> operator OP(const twofold<T>& x, const coupled<T>& y) { return value_of(x) OP y; } \
    inline coupled<T> operator OP(const coupled<T>& x, const twofold<T>& y) { return x OP value_of(y); }
    TFCP_ARITHM_CROSS_SHAPE(+, double);
    TFCP_ARITHM_CROSS_SHAPE(-, double);
    TFCP_ARITHM_CROSS_SHAPE(*, double);
    TFCP_ARITHM_CROSS_SHAPE(/, double);
    TFCP_ARITHM_CROSS_SHAPE(+, float);
    TFCP_ARITHM_CROSS_SHAPE(-, float);
    TFCP_ARITHM_CROSS_SHAPE(*, float);
    TFCP_ARITHM_CROSS_SHAPE(/, float);
#undef TFCP_ARITHM_CROSS_SHAPE

    // Cast shape according to higher-precision basic type
#define TFCP_ARITHM_CROSS_SHAPE_AND_TYPE(OP) \
    inline twofold<double> operator OP(const coupled<float> & x, const twofold<double>& y) { return dbyp<double, float>(x) OP y; } \
    inline coupled<double> operator OP(const twofold<float> & x, const coupled<double>& y) { return dbyt<double, float>(x) OP y; } \
    inline twofold<double> operator OP(const twofold<double>& x, const coupled<float> & y) { return x OP dbyp<double, float>(y); } \
    inline coupled<double> operator OP(const coupled<double>& x, const twofold<float> & y) { return x OP dbyt<double, float>(y); }
    TFCP_ARITHM_CROSS_SHAPE_AND_TYPE(+);
    TFCP_ARITHM_CROSS_SHAPE_AND_TYPE(-);
    TFCP_ARITHM_CROSS_SHAPE_AND_TYPE(*);
    TFCP_ARITHM_CROSS_SHAPE_AND_TYPE(/);
#undef TFCP_ARITHM_CROSS_SHAPE_AND_TYPE

    //------------------------------------------------------------------
    //
    //  Arithmetic operators: +=, -=, *=, /=
    //
    //------------------------------------------------------------------

#define TFCP_ARITHM_SELF(SHAPE, OP) \
    template<typename T> template<typename S> inline SHAPE<T>& SHAPE<T>::operator OP=(              S   x) { return *this = *this OP x; } \
    template<typename T> template<typename S> inline SHAPE<T>& SHAPE<T>::operator OP=(const twofold<S>& x) { return *this = *this OP x; } \
    template<typename T> template<typename S> inline SHAPE<T>& SHAPE<T>::operator OP=(const coupled<S>& x) { return *this = *this OP x; }
    TFCP_ARITHM_SELF(twofold, +);
    TFCP_ARITHM_SELF(twofold, -);
    TFCP_ARITHM_SELF(twofold, *);
    TFCP_ARITHM_SELF(twofold, /);
    TFCP_ARITHM_SELF(coupled, +);
    TFCP_ARITHM_SELF(coupled, -);
    TFCP_ARITHM_SELF(coupled, *);
    TFCP_ARITHM_SELF(coupled, /);
#undef TFCP_ARITHM_SELF

    //------------------------------------------------------------------
    //
    // Twofold/coupled auxiliary functions, and test for NaN and Inf
    //
    //------------------------------------------------------------------

    // Unary operator +
    template<typename T> twofold<T> operator + (const twofold<T>& x) { return x; }
    template<typename T> coupled<T> operator + (const coupled<T>& x) { return x; }

    // Unary operator -
    template<typename T> twofold<T> operator - (const twofold<T>& x) { return twofold<T>(-x.value, -x.error); }
    template<typename T> coupled<T> operator - (const coupled<T>& x) { return coupled<T>(-x.value, -x.error); }

    // NB: note that x.value and x.error may have opposite signs
    template<typename T> twofold<T> fabs(const twofold<T>& x) { return x.value < 0 ? -x : x; }
    template<typename T> coupled<T> fabs(const coupled<T>& x) { return x.value < 0 ? -x : x; }

    template<typename T> bool isnan(const shaped<T>& x) { return std::isnan(x.value) || std::isnan(x.error); }
    template<typename T> bool isinf(const shaped<T>& x) { return std::isinf(x.value) || std::isinf(x.error); }

    //------------------------------------------------------------------
    //
    // Comparing twofold/coupled
    //
    //------------------------------------------------------------------

    //
    // Comparing x vs y
    // Note that x.value or x.error or y.value or y.error may be NaN
    // In such case, x == y and x != y both should result in `false`
    //

#define TFCP_COMPARE_COUPLED(OP, T, S) \
    inline bool operator OP (const coupled<T>& x, const coupled<S>& y) { return value_of(x - y) OP 0; } \
    inline bool operator OP (const coupled<T>& x,               S   y) { return value_of(x - y) OP 0; } \
    inline bool operator OP (              T   x, const coupled<S>& y) { return value_of(x - y) OP 0; }
    TFCP_COMPARE_COUPLED(==, double, double);
    TFCP_COMPARE_COUPLED(!=, double, double);
    TFCP_COMPARE_COUPLED(>=, double, double);
    TFCP_COMPARE_COUPLED(<=, double, double);
    TFCP_COMPARE_COUPLED(> , double, double);
    TFCP_COMPARE_COUPLED(< , double, double);
    TFCP_COMPARE_COUPLED(==, double, float);
    TFCP_COMPARE_COUPLED(!=, double, float);
    TFCP_COMPARE_COUPLED(>=, double, float);
    TFCP_COMPARE_COUPLED(<=, double, float);
    TFCP_COMPARE_COUPLED(> , double, float);
    TFCP_COMPARE_COUPLED(< , double, float);
    TFCP_COMPARE_COUPLED(==, float, double);
    TFCP_COMPARE_COUPLED(!=, float, double);
    TFCP_COMPARE_COUPLED(>=, float, double);
    TFCP_COMPARE_COUPLED(<=, float, double);
    TFCP_COMPARE_COUPLED(> , float, double);
    TFCP_COMPARE_COUPLED(< , float, double);
    TFCP_COMPARE_COUPLED(==, float, float);
    TFCP_COMPARE_COUPLED(!=, float, float);
    TFCP_COMPARE_COUPLED(>=, float, float);
    TFCP_COMPARE_COUPLED(<=, float, float);
    TFCP_COMPARE_COUPLED(> , float, float);
    TFCP_COMPARE_COUPLED(< , float, float);
#undef TFCP_COMPARE_COUPLED

    //
    // Comparing twofolds may result in `undefined`
    // In such case, the comparing operation throws
    //

    struct twofold_exception {};

#define TFCP_COMPARE_TWOFOLD(OP, T, S)                                   \
    inline bool operator OP (const twofold<T>& x, const twofold<S>& y) { \
        if (x.value OP y.value) {                                        \
            coupled<T> px(x);                                            \
            coupled<S> py(y);                                            \
            if (px OP py) {                                              \
                return true;                                             \
            } else {                                                     \
                throw twofold_exception();                               \
            }                                                            \
        } else {                                                         \
            return false;                                                \
        }                                                                \
    }                                                                    \
    inline bool operator OP (const twofold<T>& x, S y) {                 \
        return x OP twofold<S>(y);                                       \
    }                                                                    \
    inline bool operator OP (T x, const twofold<S>& y) {                 \
        return twofold<T>(x) OP y;                                       \
    }
    TFCP_COMPARE_TWOFOLD(==, double, double);
    TFCP_COMPARE_TWOFOLD(!=, double, double);
    TFCP_COMPARE_TWOFOLD(>=, double, double);
    TFCP_COMPARE_TWOFOLD(<=, double, double);
    TFCP_COMPARE_TWOFOLD(> , double, double);
    TFCP_COMPARE_TWOFOLD(< , double, double);
    TFCP_COMPARE_TWOFOLD(==, double, float);
    TFCP_COMPARE_TWOFOLD(!=, double, float);
    TFCP_COMPARE_TWOFOLD(>=, double, float);
    TFCP_COMPARE_TWOFOLD(<=, double, float);
    TFCP_COMPARE_TWOFOLD(> , double, float);
    TFCP_COMPARE_TWOFOLD(< , double, float);
    TFCP_COMPARE_TWOFOLD(==, float, double);
    TFCP_COMPARE_TWOFOLD(!=, float, double);
    TFCP_COMPARE_TWOFOLD(>=, float, double);
    TFCP_COMPARE_TWOFOLD(<=, float, double);
    TFCP_COMPARE_TWOFOLD(> , float, double);
    TFCP_COMPARE_TWOFOLD(< , float, double);
    TFCP_COMPARE_TWOFOLD(==, float, float);
    TFCP_COMPARE_TWOFOLD(!=, float, float);
    TFCP_COMPARE_TWOFOLD(>=, float, float);
    TFCP_COMPARE_TWOFOLD(<=, float, float);
    TFCP_COMPARE_TWOFOLD(> , float, float);
    TFCP_COMPARE_TWOFOLD(< , float, float);
#undef TFCP_COMPARE_TWOFOLD

    //
    // Compare twofold vs coupled
    //

#define TFCP_COMPARE_CROSS_SHAPE_SAME_TYPE(OP, T)                        \
    inline bool operator OP (const twofold<T>& x, const coupled<T>& y) { \
        if (x.value OP y.value) {                                        \
            coupled<T> px(x);                                            \
            if (px OP y) {                                               \
                return true;                                             \
            } else {                                                     \
                throw twofold_exception();                               \
            }                                                            \
        } else {                                                         \
            return false;                                                \
        }                                                                \
    }                                                                    \
    inline bool operator OP (const coupled<T>& x, const twofold<T>& y) { \
        if (x.value OP y.value) {                                        \
            coupled<T> py(y);                                            \
            if (x OP py) {                                               \
                return true;                                             \
            } else {                                                     \
                throw twofold_exception();                               \
            }                                                            \
        } else {                                                         \
            return false;                                                \
        }                                                                \
    }
    TFCP_COMPARE_CROSS_SHAPE_SAME_TYPE(==, double);
    TFCP_COMPARE_CROSS_SHAPE_SAME_TYPE(!=, double);
    TFCP_COMPARE_CROSS_SHAPE_SAME_TYPE(>=, double);
    TFCP_COMPARE_CROSS_SHAPE_SAME_TYPE(<=, double);
    TFCP_COMPARE_CROSS_SHAPE_SAME_TYPE(> , double);
    TFCP_COMPARE_CROSS_SHAPE_SAME_TYPE(< , double);
    TFCP_COMPARE_CROSS_SHAPE_SAME_TYPE(==, float);
    TFCP_COMPARE_CROSS_SHAPE_SAME_TYPE(!=, float);
    TFCP_COMPARE_CROSS_SHAPE_SAME_TYPE(>=, float);
    TFCP_COMPARE_CROSS_SHAPE_SAME_TYPE(<=, float);
    TFCP_COMPARE_CROSS_SHAPE_SAME_TYPE(> , float);
    TFCP_COMPARE_CROSS_SHAPE_SAME_TYPE(< , float);
#undef TFCP_COMPARE_CROSS_SHAPE_SAME_TYPE

#define TFCP_COMPARE_CROSS_SHAPE_VAR_TYPES(OP)                                    \
    inline bool operator OP (const twofold<float>& x, const coupled<double>& y) { \
        twofold<double> dx(x);                                                    \
        return dx OP y;                                                           \
    }                                                                             \
    inline bool operator OP (const coupled<float>& x, const twofold<double>& y) { \
        coupled<double> dx(x);                                                    \
        return dx OP y;                                                           \
    }                                                                             \
    inline bool operator OP (const twofold<double>& x, const coupled<float>& y) { \
        coupled<double> dy(y);                                                    \
        return x OP dy;                                                           \
    }                                                                             \
    inline bool operator OP (const coupled<double>& x, const twofold<float>& y) { \
        twofold<double> dy(y);                                                    \
        return x OP dy;                                                           \
    }
    TFCP_COMPARE_CROSS_SHAPE_VAR_TYPES(==);
    TFCP_COMPARE_CROSS_SHAPE_VAR_TYPES(!=);
    TFCP_COMPARE_CROSS_SHAPE_VAR_TYPES(>=);
    TFCP_COMPARE_CROSS_SHAPE_VAR_TYPES(<=);
    TFCP_COMPARE_CROSS_SHAPE_VAR_TYPES(>);
    TFCP_COMPARE_CROSS_SHAPE_VAR_TYPES(<);
#undef TFCP_COMPARE_CROSS_SHAPE_VAR_TYPES

    //------------------------------------------------------------------
    //
    //  Printing, converting to string
    //
    //------------------------------------------------------------------

    template<typename T>
    inline std::string to_string(const shaped<T>& x) {
        return to_string(x.value) + "[" + to_string(x.error) + "]";
    }

    template<typename T>
    inline std::ostream& operator << (std::ostream& out, const shaped<T>& x) {
        return out << x.value << "[" << x.error << "]";
    }

}  // namespace tfcp

//======================================================================
#endif  // TWOFOLD_H
