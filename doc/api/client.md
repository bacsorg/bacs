# Client

Client establishes asynchronous session with distributed system.
User can send task and receive results using session object.

Session object is active and thread-safe. It may spawn
arbitrary number of internal threads and execute
given callbacks in any thread, including but not limited
to calling thread and internal threads.

## Related classes

### ConnectionOptions

```
[required] string identifier
[optional] string connection
[optional] Credentials credentials
```

See [Credentials](common.md#credentials).

## Methods

### Open()

```
Open(ConnectionOptions options)
```

Opens session.
Identifier must be globally unique and persistent
across application restarts so that replies can be delivered.

### Close()

Closes all connections and discards any unprocessed events.
Unprocessed status updates may be lost, unprocessed
results will be received again.

### Listen()

```
Listen(delegate Status(string id, Status status),
       delegate Result(string id, Result result))
```

Listen for results. Status will be called
for non-terminal events of task processing.
These events are not guaranteed to be delivered.
Result will be called exactly once for a task
once it was processed.

### Send()

```
Send(Constraints constraints,
     string id,
     Task task)
```

- `id` copied verbatim to task events, see [Listen()](#listen)

### Terminate()

Terminate `Listen()` call. It is safe to call
`Listen()` again after it has returned.
