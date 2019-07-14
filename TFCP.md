# TFCP: twofold and coupled

Evgeny Latkin <<yevgeny.latkin@gmail.com>>  
2018 Dec 4  
2019 Jul 13-14  

Contents
* [Intro](#Intro)
* [Strategy](#Strategy)
* [Stages](#Stages)
* [Technique](#Technique)
* [Safe bool](#Safe-bool)

---

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

---

## Strategy

License is very permissive: interface and reference implementation are _free_ for any purpose including commercial. 
Actually, my strategy is convincing people to use twofolds.

Ideally, all numeric software could use twofolds as sort of "safe" floating-point numbers, at least in DEBUG mode, and switching back to ordinary floats for production build.

Then, if you find the standard FP64 precision is not enough for some calculations in your code, the TFCP project would supply "coupled" numbers with 2x wider mantissa.

In future, if processors support fast FP128 arithmetic in hardware, I think you would value simulating octal precision with `tfcp::coupled<quad>` where necessary.

---

## Stages

(1) Reference library  
(2) Compilers support  
(3) Hardware support  

Reference library would provide twofold and coupled numbers for C++.
Enable standard use cases: I/O, copying, comparing, arithmetic operations, elementary functions, complex numbers, and value-arrays.
Using it would be as easy as replacing `double` type with `twofold`.

My reference implementation would demonstrate well enough performance.
It would provide OCL-like short vectors (e.g. `float4`) as the HAL for SIMD operations on AMD/Intel processors.
My code would vectorize `std::valaray` over `twofold` and `coupled` numbers.

Assuming many people adopt the twofold arithmetic (adoption target TBD), I hope to convince developers of C/C++ compilers to support twofold numbers.
I hope compilers would support automatic vectoring of twofold operations similarly to regular floating-point types.

Then I hope to convince hardware vendors like Intel and AMD to support twofold arithmetic in future processors.
New instructions for taking the round-off of the basic operations (+, -, *, /, sqrt, fma) must be enough.
Ideally, `twofold` could be only 3-4x times slower than regular `double`.

---

## Technique

Twofold approximates like _value_ + _error_, where _value_ is same as original floating-point value, and _error_ estimates its rounding error.
For example:

    e = 2.718281828459045…				-- an exact real value  
    value = 2.718282 = round(e)			-- with 7 decimal digits  
    error = -0.0000001715410 = round(e - value)	-- with 7 digits  
    value + error = 2.7182818284590			-- with 14 digits  

Fast formulas for implementing arithmetic operations over twofold numbers and C++ interface is described in my article:  

* [Twofold fast arithmetic](https://sites.google.com/site/yevgenylatkin/twofold-arithmetic/twofold-fast-arithmetic)
* [Twofolds in C and C++](https://sites.google.com/site/yevgenylatkin/twofold-arithmetic/twofolds-in-c-and-c)

Both these articles are available at my Web page dedicated to the twofolds:

* https://sites.google.com/site/yevgenylatkin/twofold-arithmetic

Even reference implementation is quite fast: your code with `tfcp::coupled<double>` numbers would run much faster than with `__float128` arithmetic.

You can additionally boost your code with OCL-like short vectors (e.g. `float4`, `double2`). My code would support vectored types like `std::coupled<double2>`, etc.

Finally, you can test performance of `std::valarray<coupled>`, which is vectored if processor supports SIMD.

## Safe bool

Comparing of twofold numbers may result in something undefined, not `true` neither `false`. E.g.:
```c++
using tfloat = twofold<float>;
tfloat e = 2.718281828459045…; // 7+7 decimal digits
float  m = (float) e;          // main part, 7 digits
tfloat d = e - m;              // equals 0, but error
if (d == 0) {                  // undefined behavior!
    .   .   .
}
```

Here, we cannot decide if twofold `d` is zero: because `d` equals _value_ + _error_, where the _value_ part is zero but the _error_ part is not zero.

Comparison of twofolds would result in especial `tfcp::safe_bool` type, which allows the _undefined_ value in addition to `true` and `false`.

Safe-bool `undefined` is sort of similar to floating-point NaN.
Computations may continue instead of throwing from the `d == 0` test.

Techncally, you can check if the result is `undefined` and process, like this e.g.:
```c++
safe_bool b = (d == 0);
assert(!is_undefined(b));
```

The `if` statement examines the `safe_bool` by converting it into standard `bool`, and such conversion throws if the safe-bool value is undefined.