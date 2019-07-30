# TFCP: twofold and coupled

Evgeny Latkin <<yevgeny.latkin@gmail.com>>  
2018 Dec 4  
2019 Jul 13-14, 16, 30  

Contents
* [Intro](#Intro)
* [Strategy](#Strategy)
* [Stages](#Stages)
* [Technique](#Technique)
* [Safe bool](#Safe-bool)

## Intro

This text overviews my TFCP project:  
* https://github.com/ylatkin/tfcp

This repo provides "twofold" and "coupled" arithmetic for C++.

Twofold numbers would help automatically debugging of floating-point errors like, e.g.:
```c++
twofold<double> result;
// …compute result with same formulas,
// as if it were ordinary double type…
assert(|error_of(result)| ≤ threshold);
```

Twofold numbers operate with both standard and extended precisions and track deviations caused by round-offs.

If result deviates too much in your view, you may consider increasing precision to possibly improve result's accuracy.

Coupled numbers is faster variant of [double-double](https://en.wikipedia.org/wiki/Quadruple-precision_floating-point_format#Double-double_arithmetic) arithmetic which duplicates floating-point precision, e.g.:
```c++
coupled<double> result;
// …compute result with same formulas…
// …use as if it were ordinary double…
cout << result;
```

Consider `tfcp::twofold` as _optimistic_ sort of intervals, which never over-estimates error, so can apply to legacy algorithms.

Consider `tfcp::coupled` as _surrogate_ of true 128-bit floats, less accurate than IEEE-754's `__float128` but much faster.

## Strategy

License is very permissive: interface and reference implementation are _free_ for any purpose including commercial. 
My strategy is convincing people to use twofolds, standardize de-facto.

Reference code includes hardware abstration level (HAL), so processor vendors can fine-tune their own implementations.
This may include support of short-vectors on SIMD processors.

Then I think about supporting twofold and coupled numbers in compilers for plain C and Fortran, as these languages are still widely used for math libraries, including legacy ones.

Twofolds anyway would not impact performance if applied only in DEBUG mode.
Production code may switch back to standard floating-point for 100% of original performance.

I expect increase of demand for expanding precision with fast "coupled" numbers, because intensive computations with faster computers would quicker accumulate rounding errors.

If future processors support fast FP128 arithmetic in hardware, TFCP would supply octal precision with `tfcp::coupled<quad>`.
It is not supported yet, but is easy to add if necessary.

## Stages

(1) Reference library  
(2) Compilers support  
(3) Hardware support  

Reference library would provide twofold and coupled numbers for C++.
Enable standard use cases: I/O, copying, comparing, arithmetic operations, elementary functions, complex numbers, and value-arrays.
Using it would be as easy as replacing `double` type with `twofold`.

Reference code would additionally include HAL for SIMD processors to support short-vector types like `twofold2`, similarly to `double4` on AVX2.
Several examples would demonstrate manually vectoring of twofolds, and test performance in linear algebra etc.

Assuming many enough people adopt twofolds, I think of adding twofold and coupled numeric types to plain C and Fortran.
I would add twofoldss to a fork of GCC compilers to standardize de-facto.
Ultimately, standardize twofolds in C++, C, and Fortran languages.

Twofold arithmetic is based on well-known _exact transforms_, like _a_ + _b_ --> _s_ + _t_ where _t_ is exactly the round-off of floating-point sum _a_ + _b_.
I presume hardware vendors would support such transforms in future processors, to boost extended precision math.

## Technique

Twofold approximates like _value_ + _error_, where _value_ is same as original floating-point value, and _error_ estimates its rounding error.
For example:

Number                                            | Comment
--------------------------------------------------|--------------------  
_e_ = 2.718281828459045…                          | an exact real value  
_value_ = 2.718282 = round(_e_)                   | with 7 decimal digits  
_error_ = -0.0000001715410 = round(_e_ - _value_) | with 7 digits  
_value_ + _error_ = 2.7182818284590               | with 14 digits  

Fast formulas for implementing arithmetic operations over twofold numbers and C++ interface is described in my article:  

* [Twofold fast arithmetic](https://sites.google.com/site/yevgenylatkin/twofold-arithmetic/twofold-fast-arithmetic)
* [Twofolds in C and C++](https://sites.google.com/site/yevgenylatkin/twofold-arithmetic/twofolds-in-c-and-c)

Both these articles are available at my Web page dedicated to the twofolds:

* https://sites.google.com/site/yevgenylatkin/twofold-arithmetic

## Safe bool

Comparing of twofold numbers may result in something undefined, not `true` neither `false`. E.g.:
```c++
using tfloat = twofold<float>;
tfloat e = 2.718281828459045…; // 7+7 decimal digits
float  m = (float) e;          // main part: 7 digits
tfloat d = e - m;              // equals 0, but error
if (d == 0) {                  // undefined!
    .   .   .
}
```

Here, we cannot decide if twofold `d` is zero: because `d` equals _value_ + _error_, where the _value_ part is zero but the _error_ part is not zero.

Comparison of twofolds would result in especial `tfcp::safe_bool` type, which allows the `undefined` value in addition to `true` and `false`.

Safe-bool `undefined` is sort of similar to floating-point NaN.
Techncally, you can check and process if the result is `undefined`, like this e.g.:
```c++
safe_bool b = (d == 0);
assert(!is_undefined(b));
```

The **_if_** statement examines the `safe_bool` by converting it into standard `bool`, and such conversion throws if the safe-bool value is undefined.
