# localman

A mini script that helps you manage your local domains easier.
Not bored with editing hosts file anymore

## Installation

```bash
brew install localman
```

## Usage

```bash
localman COMMAND [PARAM] 
```

| Command        | Description  | 
| -------------  |:-------------| 
| ls             | List all custom domains | 
| add &lt;domain&gt;   | Add a domain pointing to localhost      |
| rm &lt;domain&gt;     | Remove a domain      | 
| apply          | Apply changes (require root access)     | 
| revert         | Restore original hosts file (require root access)      | 

## License

MIT License

Copyright (c) 2019 Thien Phuc Tran
