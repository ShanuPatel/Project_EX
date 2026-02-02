set(LIB_NAME SandboxApp)
add_executable(${LIB_NAME} 
    "SandboxApp.cpp"
    "Sandbox.cpp"
  )

target_link_libraries(${LIB_NAME} PRIVATE Engine)#Packages add this if want to link the library

# Copy precompiled Assimp DLL after build
set(ASSIMP_DLL_DIR "${CMAKE_SOURCE_DIR}/ExternalLibrary/binary/bin")

add_custom_command(TARGET SandboxApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${ASSIMP_DLL_DIR}/assimp-vc143-mtd.dll"
        $<TARGET_FILE_DIR:SandboxApp>/assimp-vc143-mtd.dll
    COMMENT "Copying Assimp DLL to SandboxApp output directory"
)

target_precompile_headers(${LIB_NAME} REUSE_FROM Engine)


