# TFCP: twofold and coupled
Evgeny Latkin  
2018 Dec 4  
2019 Jul 13  

## Intro

This text overviews my TFCP project:

    https://github.com/ylatkin/tfcp

This repo provides "twofold" and "coupled" arithmetic for C++.

Twofold helps automatically debugging of floating-point errors like, e.g.:
```c++
twofold<double> result;
// …compute result with same formulas,
// as if it were ordinary double type…
assert(|error_of(result)| ≤ threshold);
```

Coupled duplicates precision in case if standard double appears not enough, e.g.:
```c++
coupled<double> result;
// …compute result with same formulas…
// …use as if it were ordinary double…
cout << result;
```

License is very permissive:  

    My reference implementation of twofold/coupled arithmetic is free for any purpose including commercial.

Actually, my strategy is encouraging people to use twofolds:

    [STRATEGY.md](./STRATEGY.md)

Then I hope to convince hardware vendors to support twofolds in future processors.

## Technique

Twofold approximates like _value_ + _error_, where value is same as original floating-point variable, and error estimates its rounding error.  

E.g.:

    _e_ = 2.718281828459045…				-- be an exact real value  
    _value_ = 2.718282 = round(_e_)			-- if with 7 decimal digits  
    _error_ = -0.0000001715410 = round(_e_ - _value_)	-- with 7 digits  
    _value_ + _error_ = 2.7182818284590			-- with 14 digits  

Fast formulas for implementing arithmetic operations over twofold numbers and C++ interface is described in my article:  
* [Twofold fast arithmetic](https://sites.google.com/site/yevgenylatkin/twofold-arithmetic/twofold-fast-arithmetic)
* [Twofolds in C and C++](https://sites.google.com/site/yevgenylatkin/twofold-arithmetic/twofolds-in-c-and-c)

Both these articles are available at my Web page dedicated to twofolds:

    https://sites.google.com/site/yevgenylatkin/twofold-arithmetic

Even reference implementation is quite fast: your code with `tfcp::coupled<double>` numbers would run much faster than with `__float128` arithmetic.

You can additionally boost your code with OpenCL-like short vectors. My reference code would support vectored types like `std::coupled<double4>` etc.

You can also leverage from `std::valarray` of coupled, it is vectored if processor supports SIMD.
