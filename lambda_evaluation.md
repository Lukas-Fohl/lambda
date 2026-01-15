# How to Evaluate Lambda Calculus

Evaluation in lambda calculus is the process of reducing an expression until it can't be reduced any further. This final, irreducible form is called the "normal form." An expression that can be reduced is called a **redex** (short for "reducible expression").

The key to evaluation is the **reduction strategy**: the rule that decides *which* redex to evaluate next when there are multiple options. The most common strategies are Applicative-Order, Normal-Order, and Lazy Evaluation.

---

### 1. Applicative-Order Evaluation (Call-by-Value)

This is the strategy used by most mainstream programming languages (C, Java, Python, etc.).

- **Rule:** Always evaluate the **innermost** redex first.
- **Analogy:** Fully evaluate the arguments to a function *before* applying the function.

**Example:**
Consider the expression `(λx.x+x) (2*3)`.

1.  Applicative-order first looks at the innermost expression, the argument `(2*3)`.
2.  It reduces `2*3` to `6`.
3.  Now the expression is `(λx.x+x) 6`.
4.  It applies the function, substituting `x` with `6`.
5.  The expression becomes `6+6`, which reduces to `12`.

- **Pro:** Efficient, as each argument is evaluated only once.
- **Con:** May fail to terminate if an argument contains an infinite loop, even if the argument is never used by the function.

---

### 2. Normal-Order Evaluation (Call-by-Name)

This strategy is "lazy" in that it doesn't evaluate anything until it has to.

- **Rule:** Always evaluate the **leftmost, outermost** redex first.
- **Analogy:** Pass the unevaluated argument directly into the function's body. The argument is only evaluated when it's actually used.

**Example:**
Consider `(λy.1) ((λx.xx)(λx.xx))`. The second argument is an infinite loop.

1.  Normal-order evaluates the outermost redex first, which is the whole expression.
2.  It applies the function `(λy.1)`, substituting the variable `y` with the (unevaluated) infinite loop `((λx.xx)(λx.xx))`.
3.  The body of the function is just `1`, and it doesn't use `y`. So the argument is discarded.
4.  The result is simply `1`.

- **Pro:** Guaranteed to find a normal form (a final answer) if one exists.
- **Con:** Can be very inefficient. If an argument is used multiple times inside a function, it gets re-evaluated every single time.

---

### 3. Lazy Evaluation (Call-by-Need)

This strategy combines the benefits of the previous two. It's famously used by the Haskell programming language.

- **Rule:** Evaluate the **leftmost, outermost** redex (like Normal-Order), but the first time an argument is evaluated, **save the result**.
- **Analogy:** Be lazy like Normal-Order, but be smart about it. If you ever need to evaluate an argument, write down the answer so you don't have to calculate it again.

This gives you the best of both worlds: the termination guarantee of Normal-Order and the efficiency of not re-evaluating arguments.

### Summary

| Strategy | Rule | Analogy | Pros & Cons |
| :--- | :--- | :--- | :--- |
| **Applicative-Order** | Innermost First | Call-by-Value | Efficient, but may not terminate. |
| **Normal-Order** | Outermost First | Call-by-Name | Guaranteed to terminate, but can be slow. |
| **Lazy Evaluation** | Outermost First + Memoization| Call-by-Need | The best of both: terminates and is efficient. |
