def BlumBlumShub(p: int, q: int, seed: int):
    assert q % 4 == 3 and p % 4 == 3
    M = q * p
    xn = seed * q % M
    while True:
        xn = (xn * xn) % M
        yield xn
