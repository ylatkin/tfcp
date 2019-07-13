# TFCP project strategy
Evgeny Latkin  
2019 Jul 13

This is my strategy about promoting the twofold/coupled arithmetic

## Goal

My goal is wide adoption of twofolds as "safe" floating-point numbers for substantially improving reliability of numeric computations.

Ideally, all numeric software could turn-on the safe twofold arithmetic despite of some performance impact, like Java controls array boundaries.

As the compromise, math software could use twofold numbers in DEBUG mode, and switch back to ordinary numbers for a production build.

If standard FP64 precision is found not enough for some numeric code, the TFCP project supplies "coupled" numbers with 2x wider mantissa.

## Stages

(1) Reference library  
(2) Performance demo  
(3) Compilers support  
(4) Hardware support  

Reference library would provide twofold numbers for C++.
Enable standard number use cases: I/O, copying, comparing, arithmetic operations, elementary functions, complex numbers, and value-arrays.
Implementation would be hardware-agnostic with moderate optimization.

Performance demo would exhibit optimization for AMD/Intel processors.
It would additionally provide twofold short-vectors for SSE/AVX, and optimized implementation of `std::valarray` (for 1 thread).
The demo would perform at 70-90% of CPU theoretical peak.

If many people adopt the twofold arithmetic (adoption target TBD), then I hope to encourage providers of C/C++ compilers to support twofold numbers.
Compilers should support automatic vectoring of numeric algorithms similarly to regular floating-point data.

Ultimately, I hope to encourage hardware vendors (like Intel and AMD) to support the instructions for taking the round-offs from the basic operations: +, -, *, /, sqrt, fma.
Ideally, this would allow twofold operations to performa at 1/2 of regular douple-precision arithmetic.