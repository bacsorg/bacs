# Client

Client establishes asynchronous session with distributed system.
User can send task and receive results using session object.

Session object is active and thread-safe. It may spawn
arbitrary number of internal threads and execute
given callbacks in any thread, including but not limited
to calling thread and internal threads.

## Methods

### Open()

```
Open(bytes identifier)
```

Opens session.
Identifier must be globally unique and persistent
across application restarts so that replies can be delivered.

### Listen()

```
Listen(delegate Status(bytes id, bytes status),
       delegate Result(bytes id, bytes result))
```

Listen for results. Status will be called
for non-terminal events of task processing.
These events are not guaranteed to be delivered.
Result will be called exactly once for a task
once it was processed.

### Send()

```
Send(string resource, bytes id, bytes task)
```

Send *task* to a **worker** that supports *resource*.
`id` is not used by distributed system and returned verbatim
with *task* updates in `Listen()`.

### Terminate()

Terminate `Listen()` call. It is safe to call
`Listen()` again after it has returned.
