# Client

## Interface to broker

 - **Client** sends *requests* to **worker**.
 - **Client** must use *REQ* or *DEALER* socket.
 - Each interaction is responded with *Confirmation*.
 - **Client** must start dialog with registration.
 - **Client** must send heartbeats if there is no other messages sent.

### Registration

1. Return address, 0 or more messages (implicit if *REQ* socket is used)
2. Null message (implicit if *REQ* socket is used)
3. 'REGISTER'
4. bunsan.broker.client.Registration

### Request

1. Return address, 0 or more messages (implicit if *REQ* socket is used)
2. Null message (implicit if *REQ* socket is used)
3. 'REQUEST'
4. bunsan.broker.client.Request
5. Null message
6. Request body, 0 or more messages

### Reply

1. Return address, verbatim from *request*
2. Null message
3. 'REPLY'
4. bunsan.broker.client.Reply
5. Null message
6. Reply body, 0 or more messages

### Confirmation

1. Return address, verbatim from message to be confirmed
2. Null message
3. 'CONFIRM'
4. bunsan.broker.client.Confirmation

### Heartbeat

1. Return address
2. Null message
3. 'HEARTBEAT'

### Error

1. Return address
2. Null message
3. 'ERROR'
4. bunsan.broker.client.Error

### Invalid

1. Return address
2. Null message
3. 'INVALID'
