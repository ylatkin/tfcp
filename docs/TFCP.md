# TFCP: twofold and coupled
Evgeny Latkin  
2018 Dec 4  
2019 Jul 13  

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

Coupled numbers duplicate precision in case if standard double appears not enough, e.g.:
```c++
coupled<double> result;
// …compute result with same formulas…
// …use as if it were ordinary double…
cout << result;
```

License is very permissive:  

* My reference implementation is _free_ for any purpose including commercial.

Actually, my strategy is encouraging people to use twofolds:

* [STRATEGY.md](./STRATEGY.md)

Then I hope to convince hardware vendors to support twofolds in future processors.

## Technique

Twofold approximates like _value_ + _error_, where value is same as original floating-point value, and error estimates its rounding _error_.  

E.g.:

    e = 2.718281828459045…				-- be an exact real value  
    value = 2.718282 = round(e)			-- if with 7 decimal digits  
    error = -0.0000001715410 = round(e - value)	-- with 7 digits  
    value + error = 2.7182818284590			-- with 14 digits  

Fast formulas for implementing arithmetic operations over twofold numbers and C++ interface is described in my article:  

* [Twofold fast arithmetic](https://sites.google.com/site/yevgenylatkin/twofold-arithmetic/twofold-fast-arithmetic)
* [Twofolds in C and C++](https://sites.google.com/site/yevgenylatkin/twofold-arithmetic/twofolds-in-c-and-c)

Both these articles are available at my Web page dedicated to the twofolds:

* https://sites.google.com/site/yevgenylatkin/twofold-arithmetic

Even reference implementation is quite fast: your code with `tfcp::coupled<double>` numbers would run much faster than with `__float128` arithmetic.

You can additionally boost your code with OCL-like short vectors (e.g. `float4`, `double4`). Reference would support vectored types like `std::coupled<double4>`, etc.

You can also leverage from `std::valarray<coupled>`, it is vectored if processor supports SIMD.
