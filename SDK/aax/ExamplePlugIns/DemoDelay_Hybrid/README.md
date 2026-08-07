# DemoDelay_Hybrid

Demonstrates AAX Hybrid architecture by splitting delay processing between low-latency (ProcessProc on host/DSP) and high-latency contexts (RenderAudio_Hybrid on host), with audio streaming between both contexts via dedicated hybrid input/output buffers. The delay line runs in high-latency context with automatic compensation for round-trip hybrid signal latency.
