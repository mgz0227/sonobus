# DemoGain_UpMixer

This example demonstrates conversion between different stem formats by supporting multiple input/output channel combinations (up to 64 channels) with configurable per-channel routing and gain controls.

Implementation note: this example uses template metaprogramming to register all stem format combinations dynamically and generates unique entrypoints for each configuration. This is for convenience in the example, to cover all formats. Production plugins are not expected to follow this example.
