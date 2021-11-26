# Service

Worker is a synchronous process that consumes
one task, issues multiple intermediate updates
and returns result.

## Related classes

### ConnectionOptions

```
[optional] string identifier
[optional] Constraints constraints
[optional] Connection connection
[optional] Credentials credentials
```

Constraints are used to specify what backend provides.
*Subject to refactoring.*

See [Credentials](common.md#credentials).

## Methods

### Open()

```
Open(ConnectionOptions)
```

### Close()

Terminates `Listen()` execution.
Closes connection to RabbitMQ.

### Listen(Callback)

```
delegate delegate void SendStatus(Status)
delegate Result Callback(Task, SendStatus)
```

User provides Callback. System provides `SendStatus()`,
which can be used to issue `Status` updates.
Callback returns single result. If exception
occurs, `Task` will be received again, possibly
by different service instance.
