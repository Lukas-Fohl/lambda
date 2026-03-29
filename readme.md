# Lambda Calculus

## Description
> Simple program to perform alpha and beta conversions for lambda expressions.

## How to Run
- Assumes you have a GCC version supporting C99 or later.
```bash
make clean && make all
./main
```

## Usage
### Function Definitions (Lambdas)
```
\<variable-name>.<body>
```
### Application
```
<lhs> <rhs>
```
### Variables
- Names can contain `[a-z]`, `[A-Z]`, and `[0-9]`.
```
<variable>
```

## Facts
- Use parentheses to change the order of operations.
- Application is left-associative: `A B C` is equivalent to `(A B) C`.
- Includes built-in definitions: `True`, `False`, `And`, `Or`, `Not`.
