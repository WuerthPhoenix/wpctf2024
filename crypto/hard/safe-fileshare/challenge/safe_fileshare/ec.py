from typing import Self, Optional
from copy import deepcopy
from dataclasses import dataclass
from hashlib import sha256

@dataclass(frozen=True)
class Point():
    x: int
    y: int

    def __eq__(self, other: Self) -> bool:
        return self.x == other.x and (self.y == other.y or self.y == -other.y)

@dataclass(frozen=True)
class EllipticCurve():
    """Represents the parameters and operations of an elliptic curve.

    Attributes:
        p (int): The prime order of the finite field.
        a (int): The coefficient 'a' in the curve equation y^2 = x^3 + ax + b.
        b (int): The coefficient 'b' in the curve equation.
        G (Point): The base point (generator) of the curve.
        n (int): The order of the base point.
        h (int): The cofactor.
        use_projective_coordinates (bool): If True, Jacobian coordinates will be used in curve operations.
    """

    p: int
    a: int
    b: int
    G: Point
    n: int
    h: int

    def is_point_on_curve(self, p: Optional[Point]):
        # We define None as the infinite point and the neutral element in the additive group.
        if p is None:
            return True

        y = (p.y * p.y) % self.p
        x = (p.x * p.x * p.x + self.a * p.x + self.b) % self.p
        return x == y

    def add_points(self, p: Optional[Point], q: Optional[Point]) -> Optional[Point]:
        assert self.is_point_on_curve(p), f"Point p: {p} not on curve."
        assert self.is_point_on_curve(q), f"Point q: {q} not on curve."
        
        if p is None or q is None:
            return p or q

        if p == q:
            p = q
            n = (3 * p.x * p.x + self.a) % self.p
            d = (2 * p.y) % self.p
        else:
            n = (q.y - p.y) % self.p
            d = (q.x - p.x) % self.p

        inv = pow(d, -1, self.p)
        s = (inv * n) % self.p

        x3 = (s * s - p.x - q.x) % self.p
        y3 = (s * (p.x - x3) - p.y)  % self.p

        p3 = Point(x3, y3)

        assert self.is_point_on_curve(p3), f"Point p3: {p3} not on curve"

        return p3

    def mul_point(self, k: int, p: Point) -> Point:
        kt = k
        assert self.is_point_on_curve(p), f"Point p: {p} not on curve."
        assert 0 < k and k < secp192k1.n, f"0 < {k} < {secp192k1.n}"

        if k == 0 or k.bit_length() > self.n.bit_length():
            raise ValueError("k is not in the range 0 < k < n")

        p = deepcopy(p)
        sum = None
        while k != 0:
            if k & 1 == 1:
                sum = self.add_points(sum, p)
            k >>= 1
            p = self.add_points(p, p)
        
        assert self.is_point_on_curve(sum), f"Point {sum} not on curve."

        return sum

    def sign(self, z: int, k: int, dA: int):
        new = secp192k1.mul_point(k, secp192k1.G)
        r = new.x % self.n
        k_inv = pow(k, -1, self.n)
        s = (k_inv * (z + r * dA)) % self.n

        return (r, s)

    def hash_bits(self, input: bytes):
        hasher = sha256()
        hasher.update(input)
        e = int(hasher.hexdigest(), 16)
        outlen = self.n.bit_length()
        z = e | ((1 << outlen) - 1)
        return z


secp192k1 = EllipticCurve(
    p=0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFEE37,
    a=0x0,
    b=0x3,
    G=Point(
        x=0xDB4FF10EC057E9AE26B07D0280B7F4341DA5D1B1EAE06C7D,
        y=0x9B2F2F6D9C5628A7844163D015BE86344082AA88D95E2F9D,
    ),
    n=0xFFFFFFFFFFFFFFFFFFFFFFFE26F2FC170F69466A74DEFD8D,
    h=0x1,
)
