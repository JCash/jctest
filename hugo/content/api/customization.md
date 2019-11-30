---
title: "Customization"
weight: 5
---

## Custom type formatting

It's beneficial to be able to convert your custom types to a human readable string.
If your type doesn't match any of the supported types, it will default to the `"?"` output, which isn't very helpful.

All that is needed it to make a function available for the template matching to find it.

Here is an example that takes a custom enum and converts that to a readable output:

{{% codefile file="/static/code/example_custom_print.cpp" language="cpp" %}}

```bash
# Compile the app
$ clang++ -I src hugo/static/code/example_custom_print.cpp

# Run the app
$ ./a.out
```

![example_custom_print.png](/jctest/images/example_custom_print.png)