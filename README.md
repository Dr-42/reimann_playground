# Riemann Playground – fixed build

The original code omitted `-lm`, causing a linker error for `log()`.
`config_linux.toml` now lists `libs = "m"`. A plain `Makefile` is also provided.

Build:
```

# using the custom toml config (Reimann Playground tool)
<your-build-command>
# or plain make

make
./main

```

