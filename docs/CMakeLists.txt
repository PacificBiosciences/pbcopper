
find_package(Doxygen)

if (${DOXYGEN_FOUND})

    # add 'make doc' command to generate API docs
    add_custom_target(docs
        "echo" "Documentation not available yet. Coming soon"
        COMMENT "Generating API documentation via Doxygen"
    )

    # create 'make doc' synonym for 'make docs'
    add_custom_target(doc DEPENDS docs)

else()
    message("Doxygen not found. 'make doc' will not be available. Install "
            "Doxygen and re-run cmake if you would like to be able to build "
            "API documentation locally.")
endif()
