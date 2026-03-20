# ciot_c Release

### Changes

**Build and code generation improvements:**
* Added new targets and variables to `scripts/Makefile` for generating Python and MyPy stubs from `.proto` files using `protoc`, making it easier to regenerate protobuf code as needed.
* Updated `make/scripts.mk` with a new `mqtt-translator` target and improved path handling for `CIOT_PATH`, supporting more flexible script execution.

**Cleanup of generated files:**
* Removed the generated protobuf Python files `scripts/ciot/proto/v1/ble_pb2.py` and `scripts/ciot/proto/v1/ble_adv_pb2.py` from version control, encouraging regeneration via the new Makefile targets rather than tracking generated code. [[1]](diffhunk://#diff-7bdadad2b5341ca49c2a3da5e39a89803796a4d5b87169ceb7f4ef8c17f1c49dL1-L344) [[2]](diffhunk://#diff-b6419d15abc96580c8db4c5575da352136e088111a4f222e54e35a88d9e5ea64L1-L303)
