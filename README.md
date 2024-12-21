# llvm-dimeta  &middot; [![License](https://img.shields.io/badge/License-BSD%203--Clause-blue.svg)](https://opensource.org/licenses/BSD-3-Clause) [![Coverage Status](https://coveralls.io/repos/github/ahueck/llvm-dimeta/badge.svg?branch=main)](https://coveralls.io/github/ahueck/llvm-dimeta)

llvm-dimeta is library to determine the type of stack (`AllocaInst`), globals (`GlobalVariables`) and heap allocations in LLVM IR based only on the LLVM debug information and metadata.

To that end, [Dimeta.h](lib/type/Dimeta.h) defines an API to query source location-specific type information with a 
custom type serialization as defined in [DimetaData.h](lib/type/DimetaData.h).
This representation can be transformed to a `yaml` format with [DimetaIO.h](lib/type/DimetaIO.h).

*Note*: For llvm-dimeta to work, LLVM debug information is required (additionally compile with the `-g` flag).


## 1. Usage

### 1.1 Code
Example function, querying the type layout of heap-like allocations with llvm-dimeta, and printing it to console as yaml.

```cpp
#include "Dimeta.h"
#include "DimetaIO.h"
...
void runOnFunction(llvm::Function& func) {
  for (const auto& inst : llvm::instructions(func)) {
    if (const auto* call_inst = llvm::dyn_cast<CallBase>(&inst)) {
      // not empty only if malloc- or new-like call:
      auto ditype = get_located_type(call_inst);
      if (ditype) {
        std::string yaml_string;
        llvm::raw_string_ostream yaml_oss(yaml_string);
        io::emit(yaml_oss, ditype.value());
        llvm::outs() << "Final heap type: \n" << yaml_oss.str() << "\n";
      }
    } else if (const auto* alloca_inst = ...) {
      ...
    }
  }
}
```

### 1.2 Example

Consider the simplified LLVM IR code of the [C file](test/pass/c/heap_struct_member.c):

```llvm
%struct.A = type { ptr }

define dso_local void @foo() #0 !dbg !9 {
    %a_struct = alloca %struct.A, align 8
    call void @llvm.dbg.declare(metadata ptr %a_struct, metadata !14, metadata !DIExpression()), !dbg !21
    %call = call noalias ptr @malloc(i64 noundef 4) #5, !dbg !22
    %a = getelementptr inbounds %struct.A, ptr %a_struct, i32 0, i32 0, !dbg !23
    store ptr %call, ptr %a, align 8, !dbg !24, !tbaa !25
    ret void, !dbg !30
}
...
!14 = !DILocalVariable(name: "a_struct", scope: !9, file: !10, line: 10, type: !15)
!15 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "A", file: !10, line: 5, size: 64, elements: !16)
!16 = !{!17}
!17 = !DIDerivedType(tag: DW_TAG_member, name: "a", scope: !15, file: !10, line: 6, baseType: !18, size: 64)
!18 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !19, size: 64)
!19 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
...
```

1. Calling `located_type_for` on the `AllocaInst` `%a_struct` results in: 

    ```yaml
    SourceLoc:
        File:            'path/to/heap_struct_member.c'
        Function:        foo
        Line:            10
    Builtin:         false
    Type:
        Compound:
            Name:            A
            Type:            struct
            Extent:          8
            Sizes:           [ 8 ]
            Offsets:         [ 0 ]
            Members:
            - Name:            a
                Builtin:         true
                Type:
                Fundamental:     { Name: int, Extent: 4, Encoding: signed_int }
                Qualifiers:      [ ptr ]
    ```

    The process searches `@foo` for the `dbg.declare` intrinsic of the `alloca` and parses the layout of `!14`.

2. Calling `located_type_for` on the `CallBase` `%@malloc` results in:

    ```yaml
    SourceLoc:
        File:            'path/to/heap_struct_member.c'
        Function:        foo
        Line:            12
    Builtin:         true
    Type:
        Fundamental:     { Name: int, Extent: 4, Encoding: signed_int }
        Qualifiers:      [ ptr ]
    ```
   
   The process starts with a forward data flow search from the `malloc` call, finding the store instruction. A subsequent backward search of the store target identifies the alloca `%a_struct` as the root node. The type of the root is identified as `struct A`. The GEP instruction determines the LHS of the assignment. By using the access indices `0 0`, we trace through the debug metadata of the root node and identify that it points to the member pointer `!18`, which has a base type `!19`.


### 1.3 Consuming llvm-dimeta
Example using CMake [FetchContent](https://cmake.org/cmake/help/latest/module/FetchContent.html) for consuming the llvm-dimeta library.

```cmake
FetchContent_Declare(
  llvm-dimeta
  GIT_REPOSITORY https://github.com/ahueck/llvm-dimeta
  GIT_TAG main
  GIT_SHALLOW 1
)
FetchContent_MakeAvailable(llvm-dimeta)

target_link_libraries(my_project_target PRIVATE dimeta::Types)
```


## 2. Building llvm-dimeta

llvm-dimeta is tested with LLVM version 12, 14, 15 and 18, and CMake version >= 3.20. Use CMake presets `develop` or `release` to build.

### 2.1 Build example

Release build recipe, installs to default prefix
`${dimeta_SOURCE_DIR}/install/dimeta`

```sh
$> cd llvm-dimeta
$> cmake --preset release
$> cmake --build build --target install --parallel
```

### 2.2 Testing
To execute the lit test suite:

```sh
$> cmake --build build --target check-dimeta
```
