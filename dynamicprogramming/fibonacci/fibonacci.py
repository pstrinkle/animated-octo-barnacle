
# plenty of ways to do this, let's do it with fibonacci.
# https://www.youtube.com/watch?v=OQ5jsbhAv_M

# exponential T(n) = T(n-1) + T(n-2) + O(1)
def naive_fib(n):
    """Compute the nth fibonaci number"""

    if n <= 2:
        f = 1
    else:
        f = naive_fib(n - 1) + naive_fib(n - 2)
    return f

stor = {}

def dp_recurse_fib(n):
    """Compute the nth fibonaci number"""
    global stor
    if n in stor:
        return stor[n]
    if n <= 2:
        f = 1
    else:
        f = dp_recurse_fib(n - 1) + dp_recurse_fib(n - 2)
    
    stor[n] = f
    return f

def bottom_up_fib(n):
    """Compute the nth fibonaci number; bottom-up dynamic programming approach"""
    memo = {}

    for k in range(1, n+1):
        if k <= 2:
            f = 1
        else:
            f = memo[k-1] + memo[k-2]
        memo[k] = f

    return memo[n]

def main():
    global stor

    stor = {}

    # 1 1 2 3 5 8 13 21 34
    for i in range(1, 10):
        print naive_fib(i)

    for i in range(1, 10):
        # because we don't clear stor between calls it saves more computations.
        # but it _is_ cheating.
        print dp_recurse_fib(i)

    for i in range(1, 10):
        print bottom_up_fib(i)

    # print dp_recurse_fib(100)

    return

if __name__ == "__main__":
    main()
