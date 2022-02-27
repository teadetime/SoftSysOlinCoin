# SoftSysOlinCoin

# Building

- to build run
  ```console
  cmake --build . 
  ```
  in ```$ OlinCoin/src ```
- to clean build files run 
  ```console 
  cmake --build . --target clean 
  ```
Old Hardcoded example for cmake
```# # add the executable
    add_executable(OlinCoin
      ${CMAKE_SOURCE_DIR}/src/tests/core/txs/test_base_tx.c 
      ${CMAKE_SOURCE_DIR}/src/core/txs/base_tx.c)

    target_include_directories(OlinCoin PUBLIC
      "${CMAKE_SOURCE_DIR}/src/includes/blocks"
      "${CMAKE_SOURCE_DIR}/src/includes/txs"
      "${CMAKE_SOURCE_DIR}/src/includes/globals"
      )
````