# localman

A mini script that helps you manage your local domains easier.
Not bored with editing hosts file anymore

## Installation
#### Homebrew
```bash
brew tap snowflk/tap
brew install localman
```
#### Build from source
```bash
git clone https://github.com/snowflk/localman
cd localman
make
```
Then move it to **/usr/bin** or any directory listed in your $PATH
```bash
sudo cp localman /usr/bin
```

## Usage

```bash
localman COMMAND [PARAM] 
```

| Command        | Description  | 
| -------------  |:-------------| 
| ls             | List all custom domains | 
| add <domain> [target]  | Add a domain pointing to target (default: localhost)      |
| update <domain> [target]  | Update a domain target (default: localhost)      |
| rm <domain>     | Remove a domain      | 
| clear          | Remove all managed domains      | 
| apply          | Apply changes (require root access)     | 
| revert         | Restore original hosts file (require root access)      | 

Note: changes are staged locally until you run `apply`; use `revert` to restore the original hosts file.

## Examples

Add a domain pointing to localhost:
```bash
localman add app.local
```

Add a domain pointing to a custom target:
```bash
localman add api.local 10.0.0.5
```

Update an existing domain with a new target:
```bash
localman update api.local 10.0.0.6
```

List managed domains:
```bash
localman ls
```

Example output:
```text
┌────────────────────────────┬────────────────────────────┐
│ Domain                     │ Target                     │
├────────────────────────────┼────────────────────────────┤
│ app.local                  │ localhost                  │
│ api.local                  │ 10.0.0.5                   │
└────────────────────────────┴────────────────────────────┘
```

Apply changes to the hosts file:
```bash
localman apply
```

## License

MIT License

Copyright (c) 2019 Thien Phuc Tran
