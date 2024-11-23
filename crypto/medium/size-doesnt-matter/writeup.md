# Crypto Medium - Size doesn't matter

This endpoints allows you to execute commands on a remote machine, provided a verified execution
token from the validation endpoint. Can you forge a token and read the flag from the server?

## Walkthrough

### Testing out the waters
After starting up the server and looking at the server, start by trying out the endpoints. Start
with the validation endpoint:

``` bash
curl https://localhost:8000/validate -XPOST -k --data "ls"
```

Notice that it return a base64 encoded string. You can use that string to trigger the command on
the endpoint:

``` bash
curl https://localhost:8000/validate -XPOST --data "ls" -k | xargs curl https://localhost:8000/execute -XPOST -k --data
```

Inspecting the code for the validation endpoint closer, notice that the `VerifiedRequest` contains
a secret `server_token`, as well as a command and an optional parameter. The first step is to
retrieve that `server_token` to forge the message. The `server_token` cannot be read from the
execution token, as it is compressed using the servers public key, so only the server can decrypt
it.

### Retrieving the server token

The created message is compressed with the lz77 algorithm before it is encrypted. That algorithm
uses back-references to compress the data, meaning it tells to copy previous data from the string.
We can use that to our advantage to reconstruct the token.

Notice also that in the validation endpoint, the request can fail if the command is to long.

With this two pieces of information, we can reconstruct the token. 

First, we need a hook for the lz77, to make sure that it catches the right position for the
back-reference. Notice that the token starts with `validation-token-`. First create a string that
is long enough to trigger the endpoint to fail because of a too long command. It is important that
that string contains random characters, so that it can't be compressed. Otherwise you will need 
around 352886 characters to fill up the buffer compressed.

Use either `ls` or `cat` as the command, so it doesn't return an error because of an unknown
command. As the parameter, add `validation-token-<random-string>`. Decrease the length of the
random string until it barely fits the buffer.

Since the command includes `validation-token-` we can be sure that the compressed string contains a
back-reference there to the start of the server token. That back reference is always 33 bit long.

In the `Dockerfile` there can be found the command that generates the token, to narrow down the
search-space.

Now try making requests to the validation endpoint with the following command: 

`<command> validation-token-<character><random_string>`

All requests to that endpoint will fail, now that a character was added, the command is too long,
except for the first character of the server token. That character will be included in the
back-reference. Since the reference always is 33 bit long, it won't change the length of the
compressed command and be the only one that is still accepted.

Add the character to the validation token prefix and repeat that until all requests fail. Now you
have successfully retrieved the `server_token`.

### Create a valid request

Add the token to the `VerifiedRequest` json. Without the validation you can now set the command to
`cat` and the parameter to `flag.txt`. Now you can either reuse the code in the given zip to
compress the json. Otherwise you can just write a bitstream. Write the number of bytes in big-endian
and then one 0-bit followed by a character for all characters in the json.

Lastly you have to encrypt and base64-encode it. For that, get the public certificate from the
server, either through a openssl command or with any programming language, encrypt your compressed
request with it and base64-encode it.

With that you have forge an execution token from the server. Send the string as a payload to the
server and enjoy submitting the flag for your hard-earned points.
