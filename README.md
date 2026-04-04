# llvm-dimeta  &middot; [![License](https://img.shields.io/badge/License-BSD%203--Clause-blue.svg)](https://opensource.org/licenses/BSD-3-Clause) [![Coverage Status](https://coveralls.io/repos/github/ahueck/llvm-dimeta/badge.svg?branch=main)](https://coveralls.io/github/ahueck/llvm-dimeta)

llvm-dimeta \[[DI25](#ref-llvm-dimeta-2025)\] is library to determine the type of stack (`AllocaInst`), globals (`GlobalVariables`) and heap allocations in LLVM IR based only on the LLVM debug information and metadata.

To that end, [Dimeta.h](lib/type/Dimeta.h) defines an API to
(a) query the LLVM debug metadata entry (`DIType`) for an allocation, or (b) query source location-specific type information with a custom type serialization as defined in [DimetaData.h](lib/type/DimetaData.h).
This representation can be transformed to a `yaml` format with [DimetaIO.h](lib/type/DimetaIO.h).

*Note*: For llvm-dimeta to work, LLVM debug information is required (additionally compile with the `-g` flag).


## 1. Usage

### 1.1 Code
Example function of some LLVM plugin using llvm-dimeta to query the type layout of heap-like allocations, and printing it to console as yaml.

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
    }
  }
}
```

### 1.2 Consuming llvm-dimeta
Example using CMake [FetchContent](https://cmake.org/cmake/help/latest/module/FetchContent.html) for consuming the llvm-dimeta library.
See [2. Building llvm-dimeta](#2-building-llvm-dimeta) for software requirements.

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


### 1.3 Algorithmic approach

Consider the simplified LLVM IR code of the [C file](test/pass/c/heap_struct_member.c) generated with Clang 19:

```llvm
%struct.A = type { ptr }

define dso_local void @foo() !dbg !10 {
entry:
  %a_struct = alloca %struct.A, align 8
    #dbg_declare(ptr %a_struct, !15, !DIExpression(), !21)
  %call = call noalias ptr @malloc(i64 noundef 4) #2, !dbg !22
  %a = getelementptr inbounds nuw %struct.A, ptr %a_struct, i32 0, i32 0, !dbg !23
  store ptr %call, ptr %a, align 8, !dbg !24
  ret void, !dbg !25
}

...
!15 = !DILocalVariable(name: "a_struct", scope: !10, file: !11, line: 8, type: !16)
!16 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "A", file: !11, line: 3, size: 64, elements: !17)
!17 = !{!18}
!18 = !DIDerivedType(tag: DW_TAG_member, name: "a", scope: !16, file: !11, line: 4, baseType: !19, size: 64)
!19 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !20, size: 64)
!20 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!21 = !DILocation(line: 10, column: 12, scope: !10)
!22 = !DILocation(line: 12, column: 16, scope: !10)
...
```

1. Calling `located_type_for` on the `llvm::AllocaInst` `%a_struct` results in: 

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

    The library searches the function `@foo` for the `#dbg_declare` debug record of the `alloca` and parses the layout of `!15`.

2. Calling `located_type_for` on the `llvm::CallBase` for `@malloc` results in:

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
   
   Since there is no debug type metadata mapping for the call, we use dataflow analysis starting from the `malloc` call to resolve the type:
   The process starts with a forward data flow search from the `malloc` call, finding the store instruction. A subsequent backward search of the store target identifies the alloca `%a_struct` as the root node. The type of the root is identified as `struct A` through the debug record (`!16`). 
   Using the root type, we analyze the rest of the instruction to refine the `DIType`: The `getelementptr` instruction determines the LHS of the assignment. By using the access indices `0 0`, we trace through the debug metadata of the root node (`!16`) and identify that it points to the member pointer `!18`, which has the base type `int` (`!19`).


## 2. Building llvm-dimeta

llvm-dimeta is tested with LLVM version 13-15 and 18-22, and CMake version >= 3.20. Use CMake presets `develop` or `release` to build.

### 2.1 Build example

Release build recipe, installs to default prefix
`${dimeta_SOURCE_DIR}/install/dimeta`

```sh
$> cd llvm-dimeta
$> cmake --preset release
$> cmake --build build --target install --parallel
```

### 2.2 Main build flags

| Option                     | Default | Description                                                              |
|----------------------------|:-------:|--------------------------------------------------------------------------|
| `DIMETA_USE_HEAPALLOCSITE` |  `ON`   | Use `!heapallocsite` metadata to type the allocation                     |
| `DIMETA_USE_TBAA`          |  `OFF`  | Use C/C++ type-based alias analysis data to type the allocation          |
| `DIMETA_TEST_CONFIG`       |  `OFF`  | Enable test config, see Section [2.3](#23-testing) (sets log level etc.) |
| `DIMETA_LOG_LEVEL`         |   `1`   | Granularity of pass logger. 3 is most verbose, 0 is least                |
| `DIMETA_ENABLE_COVERAGE`   |  `OFF`  | Enable collecting (test) coverage information                            |
| `DIMETA_ENABLE_FLANG`      |  `OFF`  | Enable Fortran type extraction support                                   |

### 2.3 Testing
For testing we require `lit` and `FileCheck`.
To execute the lit test suite:

```sh
$> cmake --build build --target check-dimeta
```

## References

<table style="border:0px">
<tr>
    <td valign="top"><a name="ref-llvm-dimeta-2025"></a>[DI25]</td>
    <td>Hück, Alexander and Kreutzer, Sebastian and Bischof, Christian.
    <b>llvm-dimeta: A library for extracting source-level type information in LLVM IR using debug metadata</b>.
    In <i>2025 IEEE International Conference on Source Code Analysis & Manipulation (SCAM)</i>,
    pages 116-121. IEEE, 2025. DOI: <a href="https://doi.org/10.1109/SCAM67354.2025.00019">10.1109/SCAM67354.2025.00019</a></td>
</tr>
</table>

