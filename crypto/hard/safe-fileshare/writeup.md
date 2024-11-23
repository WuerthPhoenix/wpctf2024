# Crypto Hard: Safe File-share

Our infiltrator has managed to gain access to some secret internal documents.
He managed to upload it to a file-sharing service under his account `intruder`,
but vanished before he could share the link with you. Your mission, should you
choose to accept it, is to analyze the service and download the file `flag.txt`?

## Scouting out the endpoints.

The player starts with a link to a file-sharing service. Once they have created
a user, they can start uploading a file. After uploading the first file, they
can copy a download link to that file. It consists of one parameter in the path
and a key as a query parameter.

Analyzing the source code, from where the parameters were generated, the player
will notice that the parameters are the two parts of a signature over an
elliptic curve.


``` python
import requests
 
session = requests.session()
credentials = {'username': 'pippo', 'password': 'pippo'}
requests.post('http://localhost:5000/auth/register.html', data=credentials)
resp = session.post('http://localhost:5000/auth/login.html', data=credentials)
assert resp.status_code == 200
```

## Analyzing the random number generator

Uploading the same file twice reveals that the url changes each upload, even
for the exact same file. The reason for this is a random function generator,
that generates a unique signature for each upload.

The random number generator relies on the [Blum Blum Shub](https://en.wikipedia.org/wiki/Blum_Blum_Shub)
algorithm. Reading over the wikipedia page will reveal that $x_0$ should be co-prime 
to `M`. Looking at the source-code, this is clearly not the case, as the seed is
multiplied with `q` to generate the new seed $x_0$.

As all following values $x_n$ are generated from $x_0$ all values $x_n$ are
co-prime to `M`. The player can easily intuit, that on a poor choice of the
initial values `p` and `q`, the period of the random numbers is compromised.
And since each random number $x_n$ is computed only from the value $x_n - 1$,
The same random numbers will repeat predictably.

Taking a look at the signature algorithm, the player will notice, that the key
`r` is only dependent on the private key, and the random number, but not the
input, so the key will also repeat at the same rate as the random number. With
this information, we can make requests until a key repeats. With this, the
player can makes requests until a key repeats, to get the period of the random
numbers.

Testing this assumption against a local container will reveal that the random
numbers do indeed repeat every 203 cycles, on the challenge instance every 924.

``` python
import re

SIG_RE = re.compile('^/files/(.*)\\?')
KEY_RE = re.compile('\\?key=(.*)$')


def get_request_data(i: int):
    filename = f'notes{i}.md'
    files = {"file": (filename, open('../notes.md', 'r'))}
    resp = session.post('http://localhost:5000/upload', files=files)
    response_json = resp.json()
    url = response_json['download_urls'][0]
    sig = SIG_RE.findall(url)[0]
    key = KEY_RE.findall(url)[0]
    return (filename, sig, key)

known_requests={}
for i in range(10000):
    filename, sig, key = get_request_data(i)
    if key in known_requests.keys():
        break

    known_requests[key] = {
        'z1': f"pippo/{filename}",
        's1': sig,
        'r': key
    }

print(f"Random number period: {len(known_requests)}")
```

## Retrieving the private key

Either by analyzing the underlying math, or by identifying the signing
algorithm and reading the wikipedia article, the player will notice, that they
can calculate first the random number `k` and then the private key $d_A$, if
they know two plaintext/signature pairs `(z, s)` with the same key. Important
to know here is that the plaintext `z` is the hash of the original plaintext,
with a bit-length not greater than the order of the base point `G`.

Here the player must also notice, that the plaintext $z_n$ is calculated as
follows: the name of the file gets the prefix `<user_name>/` to protect from
generating keys for the same file. Then the hash of the filename is calculated
and truncated to the bit-length of the order `n` of the base-point `G` of the
curve.

$$
    k = \frac{s2 - s1}{z2 - z1} mod\ n \\
    d_A = \frac{s1 * k - z1}{r} mod\ n
$$

The same [Elliptic curve digital signing algorithm (ecdsa)](https://en.wikipedia.org/wiki/Elliptic_Curve_Digital_Signature_Algorithm)
The same algorithm was used in the Sony PlayStation 3 to sign the software for
that console to run only sony signed software. however, also on the PS3 they
re-used the same random number, leading to the leak of the private key and with
that users could sign their own code to run on it.  

``` python
def reverse(z1, z2, s1, s2, r, n):
    num = (z2 - z1) % n
    den = (s2 - s1) % n
    den_inv = pow(den, -1, n)
    k = (num * den_inv) % n
    r_inv = pow(r, -1, n)
    dA = ((s1 * k - z1) * r_inv) % n
    return k, dA
```

## Finding the correct random number

Now that we know the period of the random numbers and the private key, the
player can take two samples for each of the random numbers and calculate all
possible random numbers. With that information, all possible signature/key
combinations can be calculated and tried out, until the correct file is found
and downloaded.

``` python
from safe_fileshare.ec import secp192k1

# Get a second sample for each key
for i in range(0, len(known_requests)):
    i = len(known_requests) + i + 1
    filename, sig, key = get_request_data(i)
    known_req = known_requests[key]
    known_req['z2'] = f"pippo/{filename}"
    known_req['s2'] = sig


# Calculate the random number and private key
for key in known_requests.keys():
    req = known_requests[key]
    z1 = secp192k1.hash_bits(bytes(req['z1'], 'utf8'))
    z2 = secp192k1.hash_bits(bytes(req['z2'], 'utf8'))
    s1 = int(req['s1'], 16)
    s2 = int(req['s2'], 16)
    r = int(req['r'], 16)
    k, dA = reverse(z1, z2, s1, s2, r, secp192k1.n)
    req['rn'] = k
    req['key'] = dA


# Try all random numbers with the private key to sign the filename
FLAG_FILE = b'intruder/flag.txt'
z = secp192k1.hash_bits(FLAG_FILE)
i = 0
for key in known_requests.values():
    print(f'\rTrying out random number and key: {i}', end='', flush=True)
    i += 1

    r, s = secp192k1.sign(z, key['rn'], key['key'])
    assert f'{r:x}' == key['r']

    url = f'http://localhost:5000/files/{s:x}?key={r:x}'
    req = session.get(url)
    if req.status_code == 200:
        print()
        print(f'Flag: {req.text}')
        break
```