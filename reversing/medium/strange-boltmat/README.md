# REV MED

## Description

One of your informants has tipped you off to a strange machine found discarded in the recycling depot behind the target company. Rumor has it that it might contain leftover sensitive data, and given the company's questionable privacy practices, you’re certain there could be something worth uncovering.

Curiosity piqued, you decide to take a closer look. As you examine the machine, you notice an odd humming noise—and is that… movement inside?

Your mission? Reverse engineer the binary and expose any secrets hidden within. Will you crack the code, or just end up with a pile of scrap?

## Packet types

REQ:

```xml
<metric>
    <keypress>k</keypress>
    <timestamp>1729777954.1234567</timestamp>
</metric>
```

RES:

```xml
<ack>
    <status>FillingBuffer</status>
    <registeredKeypress>k</registeredKeypress>
    <lastTimestamp>1729777954.1234567</lastTimestamp>
</ack>
```

RES (dopo n richieste):

```xml
<ack>
    <status>Success</status>
    <registeredKeypress>k</registeredKeypress>
    <lastTimestamp>1729777954.1234567</lastTimestamp>
    <transactionId>flag(n)</transactionId>
</ack>
```

## NOTES and how to build

Run `./build.sh <server_ip>` to build the project, second argument is the server ip.
It will generate a `wpctf2024-challenges:strange-boltmat-server` image and `wpctf2024-challenges:strange-boltmat-build` image.
The `build` one is only used to build the project and extract the binary so each time the it is built the same binary is generated.
The `server` one is the one that is used to run the challenge. It's only needed to have it up one time, there is no need to have one container per team.

To launch the container run `./run.sh` which will `docker compose up` the server.
