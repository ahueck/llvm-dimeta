function(dimeta_target_define_file_basename targetname)
  get_target_property(source_files "${targetname}" SOURCES)

  foreach(sourcefile ${source_files})
    get_property(compile_defs
      SOURCE "${sourcefile}"
      PROPERTY COMPILE_DEFINITIONS
    )

    get_filename_component(basename "${sourcefile}" NAME)

    list(APPEND compile_defs
      "DIMETA_LOG_BASENAME=\"${basename}\""
    )

    set_source_files_properties("${sourcefile}"
      PROPERTIES COMPILE_DEFINITIONS ${compile_defs}
    )
  endforeach()
endfunction()


function (dimeta_target_generate_file input output)
  file(READ ${input} contents)
  string(CONFIGURE "${contents}" contents @ONLY)
  file(GENERATE
    OUTPUT
      ${output}
    CONTENT
      "${contents}"
    FILE_PERMISSIONS
      OWNER_READ OWNER_WRITE OWNER_EXECUTE
      GROUP_READ
      WORLD_READ
  )
endfunction()
