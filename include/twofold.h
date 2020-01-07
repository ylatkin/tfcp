//======================================================================
// 2014-2015, 2020 (c) Evgeny Latkin
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

#include <tfcp/basic.h>

#include <type_traits>

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
    // Define twofold/coupled as similar struct containing value + error
    //
    //------------------------------------------------------------------

    // Type and shape conversions: t=twofold, p=coupled, d=dotted
    // Only for scalar types T and S: double, float, and integral
    template<typename T, typename S> inline         T  dbyd(S x);
    template<typename T, typename S> inline twofold<T> tbyd(S x);
    template<typename T, typename S> inline coupled<T> pbyd(S x);
    template<typename T, typename S> inline         T  dbyt(const twofold<S>& x);
    template<typename T, typename S> inline twofold<T> tbyt(const twofold<S>& x);
    template<typename T, typename S> inline coupled<T> pbyt(const twofold<S>& x);
    template<typename T, typename S> inline         T  dbyp(const coupled<S>& x);
    template<typename T, typename S> inline twofold<T> tbyp(const coupled<S>& x);
    template<typename T, typename S> inline coupled<T> pbyp(const coupled<S>& x);

    // Assume T is float or double
    // Assume S is float or double or integral
    template<typename T> struct twofold {
    public:
        T value;
        T error;
        twofold() {}  // default behavior if undefined
    public:
        twofold(T v, T e) : value(v), error(e) {}
    private:
        void init(const twofold<T>& t) { value = t.value;  error = t.error; }
    public:
        template<typename S> twofold(S   x)               { init(tbyd<T, S>(x)); }
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

    // Assume T is float or double
    // Assume S is float or double or integral
    template<typename T> struct coupled {
    public:
        T value;
        T error;
        coupled() {}  // default behavior if undefined
    public:
        coupled(T v, T e) : value(v), error(e) {}
    private:
        void init(const coupled<T>& p) { value = p.value;  error = p.error; }
    public:
        template<typename S> coupled(              S   x) { init(pbyd<T, S>(x)); }
        template<typename S> coupled(const twofold<S>& x) { init(pbyt<T, S>(x)); }
        template<typename S> coupled(const coupled<S>& x) { init(pbyp<T, S>(x)); }
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
    //  Type-and-shape conversions implementation
    //
    //------------------------------------------------------------------

    template<typename T, typename S> inline T dbyd(S x) {
        static_assert(std::is_same<T, float>::value ||
                      std::is_same<T, double>::value,
                      "type T must be float or double");
        static_assert(std::is_same<S, float>::value ||
                      std::is_same<S, double>::value ||
                      std::is_integral<S>::value,
                      "type S must be float or double or integral");
        T t = x;  // may provoke compiler warning if S is wider than T
        return t;
    }

    template<typename T, typename S> inline twofold<T> tbyd(S x) {
        static_assert(std::is_same<T, float>::value ||
                      std::is_same<T, double>::value,
                      "type T must be float or double");
        static_assert(std::is_same<S, float>::value ||
                      std::is_same<S, double>::value ||
                      std::is_integral<S>::value,
                      "type S must be float or double or integral");
        if (std::is_integral<S>::value || sizeof(T) < sizeof(S))
        {
            T value = x;          // may provoke compiler warning
            T error = x - value;  // maybe inexact
            return twofold<T>(value, error);
        }
        else  // if S fits into T
        {
            T value = x;  // exact, no compiler warning
            return twofold<T>(value, 0);
        }
    }

    template<typename T, typename S> inline coupled<T> pbyd(S x) {
        static_assert(std::is_same<T, float>::value ||
                      std::is_same<T, double>::value,
                      "type T must be float or double");
        static_assert(std::is_same<S, float>::value ||
                      std::is_same<S, double>::value ||
                      std::is_integral<S>::value,
                      "type S must be float or double or integral");
        if (std::is_integral<S>::value || sizeof(T) < sizeof(S))
        {
            T value = x;                      // may provoke warning
            T error = x - value;              // maybe inexact
            return coupled<T>(value, error);  // no need to renormalize
        }
        else  // if S fits into T
        {
            T value = x;  // exact, no compiler warning
            return coupled<T>(value, 0);
        }
    }

    template<typename T, typename S> inline T dbyt(const twofold<S>& x) {
        static_assert(std::is_same<T, float>::value ||
                      std::is_same<T, double>::value,
                      "type T must be float or double");
        static_assert(std::is_same<S, float>::value ||
                      std::is_same<S, double>::value,
                      "type S must be float or double");
        T t = x.value;  // may provoke compiler warning
        return t;
    }

    template<typename T, typename S> inline twofold<T> tbyt(const twofold<S>& x) {
        static_assert(std::is_same<T, float>::value ||
                      std::is_same<T, double>::value,
                      "type T must be float or double");
        static_assert(std::is_same<S, float>::value ||
                      std::is_same<S, double>::value,
                      "type S must be float or double");
        if (sizeof(T) < sizeof(S))
        {
            T value = x.value;  // may provoke compiler warning
            T error = x.error + (x.value - value);
            return twofold<T>(value, error);
        }
        else // if S fits into T
        {
            return twofold<T>(x.value, x.error);
        }
    }

    template<typename T, typename S> inline coupled<T> pbyt(const twofold<S>& x) {
        static_assert(std::is_same<T, float>::value ||
                      std::is_same<T, double>::value,
                      "type T must be float or double");
        static_assert(std::is_same<S, float>::value ||
                      std::is_same<S, double>::value,
                      "type S must be float or double");
        if (sizeof(T) < sizeof(S))
        {
            T value = x.value;
            T error = x.value - value;        // ignore x.error
            return coupled<T>(value, error);  // no need to renormalize
        }
        else // if S fits into T
        {
            return coupled<T>(x.value, 0);    // ignore x.error
        }
    }

    template<typename T, typename S> inline T dbyp(const coupled<S>& x) {
        static_assert(std::is_same<T, float>::value ||
                      std::is_same<T, double>::value ||
                      std::is_integral<T>::value,
                      "type T must be float or double or integral");
        static_assert(std::is_same<S, float>::value ||
                      std::is_same<S, double>::value,
                      "type S must be float or double");
        if (std::is_integral<T>::value)
        {
            T t  = x.value;      // round towards 0
            S e  = x.value - t;  // rounding error
              t += x.error + e;
            return t;
        }
        else
        if (sizeof(T) > sizeof(S))
        {
            T t  = x.value;  // exactly
              t += x.error;
            return t;
        }
        else // if T is narrower than S
        {
            T t = x.value;  // may provoke compiler warning
            return t;
        }
    }

    template<typename T, typename S> inline twofold<T> tbyp(const coupled<S>& x) {
        static_assert(std::is_same<T, float>::value ||
                      std::is_same<T, double>::value,
                      "type T must be float or double");
        static_assert(std::is_same<S, float>::value ||
                      std::is_same<S, double>::value,
                      "type S must be float or double");
        if (sizeof(T) < sizeof(S))
        {
            T value  = x.value;          // best fit
            T error  = x.value - value;  // rounding error
              error += x.error;
            return twofold<T>(value, error);
        }
        else
        if (sizeof(T) > sizeof(S))
        {
            T value = x.value;
            T error = x.error;
            value = fast_renorm(value, error, error);
            return twofold<T>(value, error);
        }
        else  // if S equals T
        {
            return twofold<T>(x.value, x.error);
        }
    }

    template<typename T, typename S> inline coupled<T> pbyp(const coupled<S>& x) {
        static_assert(std::is_same<T, float>::value ||
                      std::is_same<T, double>::value,
                      "type T must be float or double");
        static_assert(std::is_same<S, float>::value ||
                      std::is_same<S, double>::value,
                      "type S must be float or double");
        if (sizeof(T) < sizeof(S))
        {
            T value  = x.value;          // best fit
            T error  = x.value - value;  // rounding error
              error += x.error;
            value = fast_renorm(value, error, error);
            return coupled<T>(value, error);
        }
        else
        if (sizeof(T) > sizeof(S))
        {
            T value = x.value;
            T error = x.error;
            value = fast_renorm(value, error, error);
            return coupled<T>(value, error);
        }
        else  // if S equals T
        {
            return coupled<T>(x.value, x.error);
        }
    }

}  // namespace tfcp

//======================================================================
#endif  // TWOFOLD_H
