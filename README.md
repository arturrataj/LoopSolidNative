# LoopSolidNative

Contains native code used by `LoopSolid` via Panama.

## C ABI Wrapper System

This project exposes C ABI wrappers that follow a consistent mesh-in/mesh-out
pattern with a handle-based API (compute once, then fetch buffers).

Key files:
- `cpp/loopsolid_native.h` defines the C ABI types and function signatures.
- `cpp/loopsolid_native.cpp` implements shared marshaling helpers and registers
  concrete wrappers via a macro.
- `cpp/loopsolid_native_wrapper.h` provides:
  - `LS_DEFINE_MESH_OP_HANDLE` for declaring the C ABI functions, and
  - `LS_DEFINE_MESH_OP_HANDLE_IMPL` for implementing them in a `.cpp`.

To add a new converter:
1. Implement the C++ converter with the expected signature in a `.cpp` file.
2. Add a params struct in `cpp/loopsolid_native.h` (if needed).
3. Register the wrapper with:
   `LS_DEFINE_MESH_OP_HANDLE_IMPL(name_here, ParamStruct, your_converter_function)`

Handle-based usage (compute once):
- `ls_offset_mesh_create(...)`
- `ls_offset_mesh_fetch(...)`
- `ls_offset_mesh_free(...)`

Example converter template:
- `cpp/openvdb/example_converter.cpp`
