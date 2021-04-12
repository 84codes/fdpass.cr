# FDPass

Pass a file descriptor over a UNIX socket.

## Installation

1. Add the dependency to your `shard.yml`:

   ```yaml
   dependencies:
     fdpass:
       github: 84codes/fdpass.cr
   ```

2. Run `shards install`

## Usage

```crystal
require "fdpass"
FDPass.send_fd(unix_socket.fd, socket_to_pass.fd)
```
