# TFCP project strategy
Evgeny Latkin  
2019 Jul 13

This is my strategy about promoting the twofold/coupled arithmetic

## Goal

My goal is wide adoption of twofolds as "safe" floating-point numbers for substantially improving reliability of numeric computations.

Ideally, all numeric software could turn-on the safe twofold arithmetic despite of some performance impact, like Java controls array boundaries.

As the compromise, math software could use twofold numbers in DEBUG mode, and switch back to ordinary numbers for a production build.

If standard FP64 precision is found not enough for some numeric code, the TFCP project supplies "coupled" numbers with 2x wider mantissa.

## License

I would like to convince people to use twofolds, so license would be very permissive:

* My reference implementation would be _free_ for any purpose including commercial.

I hope to convince the hardware vendors supporting twofolds, if many people would adopt.

## Stages

(1) Reference library  
(2) Compilers support  
(3) Hardware support  

Reference library would provide twofold and coupled numbers for C++.
Enable standard use cases: I/O, copying, comparing, arithmetic operations, elementary functions, complex numbers, and value-arrays.
Using it would be as easy as replacing `double` type with `twofold`.

My reference implementation would demonstrate well enough performance.
It would provide OCL-like short vectors (e.g. `float4`) as the HAL for SIMD operations on AMD/Intel processors.
My code would vectorize `std::valaray` over `twofold` numbers (only for 1-thread).

If many people adopt the twofold arithmetic (adoption target TBD), then I hope to convince developers of C/C++ compilers to support twofold numbers.
I hope compilers would support automatic vectoring of twofold operations similarly to regular floating-point types.

Ultimately, I hope to convince vendors like Intel and AMD to support twofold arithmetic in hardware.
Instructions for taking the round-offs from the basic operations (+, -, *, /, sqrt, fma) must be enough.
Ideally, `twofold` could perform only 2-3x times slower than regular `double`.