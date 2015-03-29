Client
======

Interface to broker
-------------------

**Client** sends *requests* to **worker**.
**Client** must use *REQ* or *DEALER* socket to send *requests*.

### Request

1. Return address, 0 or more messages (implicit if *REQ* socket is used)
2. Null message
3. Worker selection criteria (unspecified, TBD)
4. Null message
5. Request body, 0 or more messages

### Reply

1. Return address, verbatim from *request*
2. Null message
3. Reply body, 0 or more messages

### Reply acknowledge

**TODO**
