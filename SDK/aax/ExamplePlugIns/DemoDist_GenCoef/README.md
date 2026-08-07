# DemoDist_GenCoef

Demonstrates manual parameter update handling by directly overriding `UpdateParameterNormalizedValue()` and `GenerateCoefficients()` instead of using the `AAX_CPacketDispatcher` helper class. This approach uses dirty flags to track parameter changes and manually posts packets, providing more granular control over complex parameter-to-coefficient mappings.
