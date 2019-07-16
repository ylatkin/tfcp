# TFCP: twofold and coupled

Evgeny Latkin <<yevgeny.latkin@gmail.com>>  
2018 Dec 4  
2019 Jul 13-14, 16  

Contents
* [Intro](#Intro)
* [Strategy](#Strategy)
* [Stages](#Stages)
* [Performance](#Performance)
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

Coupled numbers duplicate precision, in case if standard `double` appears not enough, e.g.:
```c++
coupled<double> result;
// …compute result with same formulas…
// …use as if it were ordinary double…
cout << result;
```

Consider `tfcp::twofold` as the _optimistic_ sort of intervals, which never over-estimates the error although may under-estimate sometimes.

Consider `tfcp::coupled` as the _surrogate_ of 128-bit floats, less accurate than the true `__float128` but much faster on modern processors.

## Strategy

License is very permissive: interface and reference implementation are _free_ for any purpose including commercial. 
Actually, my strategy is convincing people to use twofolds.

Ideally, all numeric software could use twofolds as sort of "safe" floating-point numbers, at least in DEBUG mode, and switching back to ordinary floats for production build.

Then, if you find the standard FP64 precision is not enough for some calculations in your code, the TFCP project would supply "coupled" numbers with 2x wider mantissa.

In future, if processors support fast FP128 arithmetic in hardware, I think you would value simulating octal precision with `tfcp::coupled<quad>` where necessary.

## Stages

(1) Reference library  
(2) Performance demo  
(3) Compilers support  
(4) Hardware support  

Reference library would provide twofold and coupled numbers for C++.
Enable standard use cases: I/O, copying, comparing, arithmetic operations, elementary functions, complex numbers, and value-arrays.
Using it would be as easy as replacing `double` type with `twofold`.

My implementation would support twofolds over short-vector types like `double4` on SIMD processors, and vectorize `std::valarray` over twofold numbers. This would fuel
several performance demo/tests, show that twofolds can perform only 2x slower than regular `double` sometimes.

Assuming many enough people adopt the twofold numbers, then 
I hope C/C++ compilers would support automatic vectoring of twofold operations.
Not sure if I can implement this myself, I would rather convince developers of C/C++ compilers to support twofold numbers.

Then I hope to convince hardware vendors like Intel and AMD to support twofold arithmetic in future processors.
New instructions for taking the round-off of the basic operations (+, -, *, /, sqrt, fma) must be enough.
Ideally, twofolds could be only 2x times slower than regular `double`.

## Performance

My reference library would be quite fast: your code with `tfcp::coupled<double>` would run up to 10x times faster than with `__float128` numbers.

On a SIMD processor, you can manually boost your code by 2-4x times with short vectors, like `tfcp::coupled4`, similarly to vectoring with `double4`.

Although, you may find it more convenient leveraging performance boost from the vectored `std::valarray<coupled>` operations.

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
