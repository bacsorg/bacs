# Common classes

## Credentials

```
[optional] string method
[optional] string login
[optional] string password
[optional] bytes certificate
[optional] bytes key
```

## Constraints

Empty list matches any.
Result matcher is conjunction of specified properties.

```
[repeated] string resource
```

## Task

```
[required] string type
[required] string package
[optional] bytes data
```

- `type` type of package, how it should be executed
- `package` package name for [bunsan::pm](/bunsanorg/pm)
- `data` data for processing
