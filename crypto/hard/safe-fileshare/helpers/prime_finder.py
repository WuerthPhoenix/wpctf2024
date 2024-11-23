def Primes():
    seen = [2]
    current = 1

    def is_prime(n):
        for prime in seen:
            if n % prime == 0:
                return False
            if prime * prime > n:
                return True
        return True

    while True:
        current += 2
        if is_prime(current):
            seen.append(current)
            yield current

def BlumBlumShub(p: int, q: int, seed: int):
    M = q * p
    xn = seed * q % M
    while True:
        xn = (xn * xn) % M
        yield xn


def find_rng_period(p, q):
    bbs = BlumBlumShub(p, q, 123456)
    seen = []
    while True:
        current = next(bbs)
        if current in seen:
            return len(seen)
        seen.append(current)
        if len(seen) > 10000:
            return -1

primes = Primes()
next(primes) # Skip 2
next(primes) # Skip 3

seen = [3]

while True:
    for _ in range(500):
        current = next(primes)
        while current % 4 != 3:
            current = next(primes)

    for prev in seen:
        period = find_rng_period(current, prev)
        print(f"Period {current}, {prev}: {period}")

        period = find_rng_period(prev, current)
        print(f"Period {prev}, {current}: {period}")
    
    seen.append(current)
    if len(seen) > 50:
        seen = seen[1:]